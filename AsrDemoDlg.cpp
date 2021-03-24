// AsrDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AsrDemo.h"
#include "AsrDemoDlg.h"
#include "utils\FileUtil.h"
#include "utils\ChineseConvertUtil.h"
#include "utils\WebRtcWrapper.h"

using namespace ASR;

#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SAVE_WAV_FILE  false
#define FORCE_16K_MONO_16B  false

#define WAVE_FORMAT_16M08       0x00100000       /* 16 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_16S08       0x00200000       /* 16 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_16M16       0x00400000       /* 16 kHz, Mono,   16-bit  */
#define WAVE_FORMAT_16S16       0x00800000       /* 16 kHz, Stereo, 16-bit  */

AsrDemoDlg::AsrDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AsrDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRun=FALSE;
	m_hThread=NULL;
	m_hWaveIn=NULL;
	ZeroMemory(&m_stWFEX,sizeof(WAVEFORMATEX));
	ZeroMemory(m_stWHDR,MAX_BUFFERS*sizeof(WAVEHDR));

	m_sendData = new NetworkTrans();
	m_pInBuffer = (char *)malloc(MAX_PCM_BUFFER);
	m_pOutBuffer = (char*)malloc(MAX_PCM_BUFFER);
	m_count = 0;
}

AsrDemoDlg::~AsrDemoDlg()
{
	delete m_sendData;
	m_sendData = nullptr;

	free(m_pInBuffer);
	m_pInBuffer = nullptr;
	free(m_pOutBuffer);
	m_pOutBuffer = nullptr;
}


void AsrDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(AsrDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_REC, OnBnClickedStartStop)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_CBN_SELCHANGE(IDC_DEVICES, OnSelDevices)
	ON_CBN_SELCHANGE(IDC_FORMATS, &AsrDemoDlg::OnCbSelFormats)
	ON_EN_CHANGE(IDC_FILENAME, &AsrDemoDlg::OnEnChangeFilename)
	ON_EN_CHANGE(IDC_STATUS, &AsrDemoDlg::OnEnChangeStatus)
	ON_BN_CLICKED(IDC_SEND_FILE, &AsrDemoDlg::OnBnClickedSendFile)
END_MESSAGE_MAP()


// AsrDemoDlg message handlers

BOOL AsrDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(FillDevices()<=0)
	{
		AfxMessageBox("NO Input Devices Found..",MB_ICONERROR);
		CDialog::OnOK();
	}
	char szFileName[MAX_PATH]={0};
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	szFileName[lstrlen(szFileName)-3]='\0';
	lstrcat(szFileName,"pcm");
	GetDlgItem(IDC_FILENAME)->SetWindowText(szFileName);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
UINT AsrDemoDlg::FillDevices()
{
	CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_DEVICES);
	UINT nDevices,nC1;
	WAVEINCAPS stWIC={0};
	MMRESULT mRes;

	pBox->ResetContent();
	nDevices=waveInGetNumDevs();

	for(nC1=0;nC1<nDevices;++nC1)
	{
		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		mRes=waveInGetDevCaps(nC1,&stWIC,sizeof(WAVEINCAPS));
		if(mRes==0)
			pBox->AddString(stWIC.szPname);
		else
			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
	}
	if(pBox->GetCount())
	{
		pBox->SetCurSel(0);
		OnSelDevices();
	}
	return nDevices;
}


void AsrDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR AsrDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI ThFunc(LPVOID pDt)
{
	AsrDemoDlg*pOb=(AsrDemoDlg*)pDt;
	pOb->StartRecording();
	return 0;
}

void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
		case WIM_CLOSE:
			break;

		case WIM_DATA:
			{
			AsrDemoDlg*pDlg=(AsrDemoDlg*)dwInstance;
				pDlg->ProcessRecordData((WAVEHDR *)dwParam1);
			}
			break;

		case WIM_OPEN:
			break;

		default:
			break;
	}
}

VOID AsrDemoDlg::ProcessRecordData(WAVEHDR * pHdr)
{
	MMRESULT mRes=0;

	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{
		// m_sendData->sendPcmData(pHdr->lpData, pHdr->dwBytesRecorded);
		if (m_count + pHdr->dwBytesRecorded < MAX_PCM_BUFFER) {
			memcpy(m_pInBuffer + m_count, pHdr->lpData, pHdr->dwBytesRecorded);
			m_count += pHdr->dwBytesRecorded;
		}
		else {
			printf("pcm buffer overflow!\n");
			//OnBnClickedStartStop();
			//return;
		}
		mmioWrite(m_hOPFile,pHdr->lpData,pHdr->dwBytesRecorded);
		mRes=waveInAddBuffer(m_hWaveIn,pHdr,sizeof(WAVEHDR));
		if(mRes!=0)
			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
	}
}

VOID AsrDemoDlg::OpenDevice()
{
	int nT1=0;
	CString csT1;
	double dT1=0.0;
	MMRESULT mRes=0;
	CComboBox *pDevices=(CComboBox*)GetDlgItem(IDC_DEVICES);
	CComboBox *pFormats=(CComboBox*)GetDlgItem(IDC_FORMATS);

	nT1=pFormats->GetCurSel();
	if(nT1==-1)
		throw "";
	pFormats->GetLBText(nT1,csT1);
	sscanf_s((PCHAR)(LPCTSTR)csT1,"%lf",&dT1);
	dT1=dT1*1000;
	m_stWFEX.nSamplesPerSec=(int)dT1;
	if (FORCE_16K_MONO_16B)
		m_stWFEX.nSamplesPerSec = 16000;
	csT1=csT1.Right(csT1.GetLength()-csT1.Find(',')-1);
	csT1.Trim();
	if(csT1.Find("mono")!=-1)
		m_stWFEX.nChannels=1;
	if(csT1.Find("stereo")!=-1)
		m_stWFEX.nChannels=2;
	csT1=csT1.Right(csT1.GetLength()-csT1.Find(',')-1);
	csT1.Trim();
	sscanf_s((PCHAR)(LPCTSTR)csT1,"%hd",&m_stWFEX.wBitsPerSample);
	m_stWFEX.wFormatTag=WAVE_FORMAT_PCM;
	m_stWFEX.nBlockAlign=m_stWFEX.nChannels*m_stWFEX.wBitsPerSample/8;
	if (FORCE_16K_MONO_16B) {
		m_stWFEX.nBlockAlign = 2;
		m_stWFEX.wBitsPerSample = 16;
	}
	m_stWFEX.nAvgBytesPerSec=m_stWFEX.nSamplesPerSec*m_stWFEX.nBlockAlign;
	m_stWFEX.cbSize=sizeof(WAVEFORMATEX);
	mRes=waveInOpen(&m_hWaveIn,pDevices->GetCurSel(),&m_stWFEX,(DWORD_PTR)waveInProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
	if(mRes!=MMSYSERR_NOERROR)
	{
		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
		throw m_csErrorText;
	}
	GetDlgItem(IDC_FILENAME)->GetWindowText(csT1);
	ZeroMemory(&m_stmmIF,sizeof(MMIOINFO));
	DeleteFile((PCHAR)(LPCTSTR)csT1);
	m_saveFile = csT1;
	m_hOPFile=mmioOpen((PCHAR)(LPCTSTR)csT1, &m_stmmIF, MMIO_WRITE | MMIO_CREATE);
	if(m_hOPFile==NULL)
		throw "Can not open file...";

	if (SAVE_WAV_FILE) {
		ZeroMemory(&m_stckOutRIFF,sizeof(MMCKINFO));
		m_stckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
		mRes=mmioCreateChunk(m_hOPFile, &m_stckOutRIFF, MMIO_CREATERIFF);
		if(mRes!=MMSYSERR_NOERROR)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		ZeroMemory(&m_stckOut,sizeof(MMCKINFO));
		m_stckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');
		m_stckOut.cksize = sizeof(m_stWFEX);
		mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
		if(mRes!=MMSYSERR_NOERROR)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		nT1=mmioWrite(m_hOPFile, (HPSTR) &m_stWFEX, sizeof(m_stWFEX));
		if(nT1!=sizeof(m_stWFEX))
		{
			m_csErrorText.Format("Can not write Wave Header..File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		mRes=mmioAscend(m_hOPFile, &m_stckOut, 0);
		if(mRes!=MMSYSERR_NOERROR)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		m_stckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
		if(mRes!=MMSYSERR_NOERROR)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
	}
}
VOID AsrDemoDlg::CloseDevice()
{
	MMRESULT mRes=0;
	
	if(m_hWaveIn)
	{
		UnPrepareBuffers();
		mRes=waveInClose(m_hWaveIn);
	}
	if(m_hOPFile)
	{
		if (SAVE_WAV_FILE) {
			mRes = mmioAscend(m_hOPFile, &m_stckOut, 0);
			if (mRes != MMSYSERR_NOERROR)
			{
				StoreError(mRes, FALSE, "File: %s ,Line Number:%d", __FILE__, __LINE__);
			}
			mRes = mmioAscend(m_hOPFile, &m_stckOutRIFF, 0);
			if (mRes != MMSYSERR_NOERROR)
			{
				StoreError(mRes, FALSE, "File: %s ,Line Number:%d", __FILE__, __LINE__);
			}
		}
		mmioClose(m_hOPFile,0);
		m_hOPFile=NULL;

		WebRTCUtil::WebRtcWrapper::doNSAgc(m_saveFile);
	}
	m_hWaveIn=NULL;
}

VOID AsrDemoDlg::PrepareBuffers()
{
	MMRESULT mRes=0;
	int nT1=0;

	for(nT1=0;nT1<MAX_BUFFERS;++nT1)
	{
		m_stWHDR[nT1].lpData=(LPSTR)HeapAlloc(GetProcessHeap(),8,m_stWFEX.nAvgBytesPerSec);
		m_stWHDR[nT1].dwBufferLength=m_stWFEX.nAvgBytesPerSec;
		m_stWHDR[nT1].dwUser=nT1;
		mRes=waveInPrepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		mRes=waveInAddBuffer(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
	}
}

VOID AsrDemoDlg::UnPrepareBuffers()
{
	MMRESULT mRes=0;
	int nT1=0;

	if(m_hWaveIn)
	{
		mRes=waveInStop(m_hWaveIn);
		for(nT1=0;nT1<3;++nT1)
		{
			if(m_stWHDR[nT1].lpData)
			{
				mRes=waveInUnprepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(),0,m_stWHDR[nT1].lpData);
				ZeroMemory(&m_stWHDR[nT1],sizeof(WAVEHDR));
			}
		}
	}
}

VOID AsrDemoDlg::StartRecording()
{	
	MMRESULT mRes;
	SetStatus("Recording...");

	try
	{
		m_count = 0;
		memset(m_pInBuffer, 0, MAX_PCM_BUFFER);
		OpenDevice();
		PrepareBuffers();
		mRes=waveInStart(m_hWaveIn);
		if(mRes!=0)
		{
			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			throw m_csErrorText;
		}
		while(m_bRun)
		{
			SleepEx(100,FALSE);
		}
	}
	catch(PCHAR pErrorMsg)
	{
		AfxMessageBox(pErrorMsg);
	}
	CloseDevice();
	CloseHandle(m_hThread);
	m_hThread=NULL;
	SetStatus("Recording stopped...");
}

void AsrDemoDlg::OnBnClickedStartStop()
{
	CString csT1;
	BOOL bEnable=FALSE;

	GetDlgItem(ID_REC)->GetWindowText(csT1);
	GetDlgItem(ID_REC)->EnableWindow(FALSE);
	if(csT1.Compare("&Start")==0)
	{
		m_bRun=TRUE;
		GetDlgItem(ID_REC)->SetWindowText("&Stop");
		m_hThread=CreateThread(NULL,0,ThFunc,this,0,NULL);
	}
	else
	{
		m_bRun=FALSE;
		while(m_hThread)
		{
			SleepEx(100,FALSE);
		}
		bEnable=TRUE;// AfxMessageBox("Recording Stopped..");
		GetDlgItem(ID_REC)->SetWindowText("&Start");

		int nsTime = WebRTCUtil::WebRtcWrapper::doNS(m_pInBuffer, m_pOutBuffer, m_count);
		int agcTime = WebRTCUtil::WebRtcWrapper::doAgc(m_pOutBuffer, m_pInBuffer, m_count);
		int nTime = GetTickCount();

		char out[256] = {0};
		m_sendData->sendPcmData(m_pInBuffer, m_count, out);
		char out2[256] = { 0 };
		ChineseConvertUtil::Utf8ToGB2312(out, out2);
		DisplayAsrText(out2);
		nTime = GetTickCount() - nTime;
		printf("NS time = %dms, AGC time = %dms, ASR time = %dms\n", nsTime, agcTime, nTime);
		m_count = 0;
	}
	GetDlgItem(IDC_BROWSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_DEVICES)->EnableWindow(bEnable);
	GetDlgItem(IDC_FORMATS)->EnableWindow(bEnable);
	GetDlgItem(ID_REC)->EnableWindow(TRUE);
}

void AsrDemoDlg::OnBnClickedBrowse()
{
	//CFileDialog oDlg(FALSE,".wav","SndRec",OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,"WAV Files|*.wav||",this,0);
	CFileDialog oDlg(FALSE, ".pcm", "SndRec", OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, "pcm Files|*.pcm||", this, 0);
	if(oDlg.DoModal()==IDOK)
	{
		GetDlgItem(IDC_FILENAME)->SetWindowText(oDlg.GetPathName());
	}
}



void AsrDemoDlg::OnSelDevices()
{
	CComboBox *pDevices=(CComboBox*)GetDlgItem(IDC_DEVICES);
	CComboBox *pFormats=(CComboBox*)GetDlgItem(IDC_FORMATS);
	int nSel;
	WAVEINCAPS stWIC={0};
	MMRESULT mRes;


	SetStatus("Querying device informations...");
	pFormats->ResetContent();
	nSel=pDevices->GetCurSel();
	if(nSel!=-1)
	{
		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		mRes=waveInGetDevCaps(nSel,&stWIC,sizeof(WAVEINCAPS));
		if(mRes==0)
		{
			
			pFormats->SetItemData(pFormats->AddString("16 kHz, mono, 16-bit"), WAVE_FORMAT_16M16);
			pFormats->SetItemData(pFormats->AddString("16 kHz, stereo, 16-bit"), WAVE_FORMAT_16S16);
			pFormats->SetItemData(pFormats->AddString("16 kHz, mono, 8-bit"), WAVE_FORMAT_16M08);
			pFormats->SetItemData(pFormats->AddString("16 kHz, stereo, 8-bit"), WAVE_FORMAT_16S08);			

			if(WAVE_FORMAT_1M08==(stWIC.dwFormats&WAVE_FORMAT_1M08))
				pFormats->SetItemData(pFormats->AddString("11.025 kHz, mono, 8-bit"),WAVE_FORMAT_1M08);
			if(WAVE_FORMAT_1M16==(stWIC.dwFormats&WAVE_FORMAT_1M16))
				pFormats->SetItemData(pFormats->AddString("11.025 kHz, mono, 16-bit"),WAVE_FORMAT_1M16);
			if(WAVE_FORMAT_1S08==(stWIC.dwFormats&WAVE_FORMAT_1S08))
				pFormats->SetItemData(pFormats->AddString("11.025 kHz, stereo, 8-bit"),WAVE_FORMAT_1S08);
			if(WAVE_FORMAT_1S16==(stWIC.dwFormats&WAVE_FORMAT_1S16))
				pFormats->SetItemData(pFormats->AddString("11.025 kHz, stereo, 16-bit"),WAVE_FORMAT_1S16);
			if(WAVE_FORMAT_2M08==(stWIC.dwFormats&WAVE_FORMAT_2M08))
				pFormats->SetItemData(pFormats->AddString("22.05 kHz, mono, 8-bit"),WAVE_FORMAT_2M08);
			if(WAVE_FORMAT_2M16==(stWIC.dwFormats&WAVE_FORMAT_2M16))
				pFormats->SetItemData(pFormats->AddString("22.05 kHz, mono, 16-bit"),WAVE_FORMAT_2M16);
			if(WAVE_FORMAT_2S08==(stWIC.dwFormats&WAVE_FORMAT_2S08))
				pFormats->SetItemData(pFormats->AddString("22.05 kHz, stereo, 8-bit"),WAVE_FORMAT_2S08);
			if(WAVE_FORMAT_2S16==(stWIC.dwFormats&WAVE_FORMAT_2S16))
				pFormats->SetItemData(pFormats->AddString("22.05 kHz, stereo, 16-bit"),WAVE_FORMAT_2S16);
			if(WAVE_FORMAT_4M08==(stWIC.dwFormats&WAVE_FORMAT_4M08))
				pFormats->SetItemData(pFormats->AddString("44.1 kHz, mono, 8-bit"),WAVE_FORMAT_4M08);
			if(WAVE_FORMAT_4M16==(stWIC.dwFormats&WAVE_FORMAT_4M16))
				pFormats->SetItemData(pFormats->AddString("44.1 kHz, mono, 16-bit"),WAVE_FORMAT_4M16);
			if(WAVE_FORMAT_4S08==(stWIC.dwFormats&WAVE_FORMAT_4S08))
				pFormats->SetItemData(pFormats->AddString("44.1 kHz, stereo, 8-bit"),WAVE_FORMAT_4S08);
			if(WAVE_FORMAT_4S16==(stWIC.dwFormats&WAVE_FORMAT_4S16))
				pFormats->SetItemData(pFormats->AddString("44.1 kHz, stereo, 16-bit"),WAVE_FORMAT_4S16);
			if(WAVE_FORMAT_96M08==(stWIC.dwFormats&WAVE_FORMAT_96M08))
				pFormats->SetItemData(pFormats->AddString("96 kHz, mono, 8-bit"),WAVE_FORMAT_96M08);
			if(WAVE_FORMAT_96S08==(stWIC.dwFormats&WAVE_FORMAT_96S08))
				pFormats->SetItemData(pFormats->AddString("96 kHz, stereo, 8-bit"),WAVE_FORMAT_96S08);
			if(WAVE_FORMAT_96M16==(stWIC.dwFormats&WAVE_FORMAT_96M16))
				pFormats->SetItemData(pFormats->AddString("96 kHz, mono, 16-bit"),WAVE_FORMAT_96M16);
			if(WAVE_FORMAT_96S16==(stWIC.dwFormats&WAVE_FORMAT_96S16))
				pFormats->SetItemData(pFormats->AddString("96 kHz, stereo, 16-bit"),WAVE_FORMAT_96S16);
			if(pFormats->GetCount())
				pFormats->SetCurSel(0);
		}
		else
			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
	}
	SetStatus("Waiting to start...");
}

CString AsrDemoDlg::StoreError(MMRESULT mRes,BOOL bDisplay,LPCTSTR lpszFormat, ...)
{
	MMRESULT mRes1=0;
	char szErrorText[1024]={0};
	char szT1[2*MAX_PATH]={0};
	
	va_list args;
	va_start(args, lpszFormat);
	_vsntprintf_s(szT1, MAX_PATH, lpszFormat, args);
	va_end(args);

	m_csErrorText.Empty();
	if(m_bRun)
	{
		mRes1=waveInGetErrorText(mRes,szErrorText,1024);
		if(mRes1!=0)
			wsprintf(szErrorText,"Error %d in querying the error string for error code %d",mRes1,mRes);
		m_csErrorText.Format("%s: %s",szT1,szErrorText);
		if(bDisplay)
			AfxMessageBox(m_csErrorText);
	}
	return m_csErrorText;
}

VOID AsrDemoDlg::SetStatus(LPCTSTR lpszFormat, ...)
{
	CString csT1;
	va_list args;

	va_start(args, lpszFormat);
	csT1.FormatV(lpszFormat,args);
	va_end(args);
	if (IsWindow(m_hWnd) && GetDlgItem(IDC_STATUS)) {
		GetDlgItem(IDC_STATUS)->SetWindowText(csT1);
	}
}


void AsrDemoDlg::OnCbSelFormats()
{

}


void AsrDemoDlg::OnEnChangeFilename()
{

}


void AsrDemoDlg::OnEnChangeStatus()
{

}


void AsrDemoDlg::OnBnClickedSendFile()
{
	char out[200] = { 0 };
	char out2[200] = { 0 };
	m_sendData->sendPcmData("D:\\work\\asr\\T0055G0002S0001.pcm", out);
	FileUtil::WriteReadFileTest(out, out2);
	ChineseConvertUtil::Utf8ToGB2312(out, out2);
	SetStatus(out2);
}




void AsrDemoDlg::DisplayAsrText(char* text) {
	CEdit* Information_Box;
	Information_Box = (CEdit*)GetDlgItem(IDC_RECV_DATA);

	Information_Box->SetWindowTextA(_T(text));
}


