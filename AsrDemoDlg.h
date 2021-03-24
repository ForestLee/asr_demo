#ifndef _ASR_DEMO_DLG_H_
#define _ASR_DEMO_DLG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <string>

#include <MMSystem.h>
#include "utils\NetworkTrans.h"

#define MAX_BUFFERS	3
#define MAX_PCM_BUFFER	1024*1024*1    // 1M

// AsrDemoDlg dialog
class AsrDemoDlg : public CDialog
{
// Construction
public:
	AsrDemoDlg(CWnd* pParent = NULL);	// standard constructor
	~AsrDemoDlg();

// Dialog Data
	enum { IDD = IDD_SOUNDREC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartStop();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnSelDevices();

	VOID StartRecording();
	VOID ProcessRecordData(WAVEHDR * pHdr);

private:
	VOID SetStatus(LPCTSTR lpszFormat, ...);
	VOID OpenDevice();
	VOID CloseDevice();
	VOID PrepareBuffers();
	VOID UnPrepareBuffers();
	UINT FillDevices();
	CString StoreError(MMRESULT mRes,BOOL bDisplay,LPCTSTR lpszFormat, ...);
	void DisplayAsrText(char* text);

private:
	BOOL m_bRun;
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_stWFEX;
	WAVEHDR m_stWHDR[MAX_BUFFERS];
	HANDLE m_hThread;
	CString m_csErrorText;
	HMMIO m_hOPFile;
	MMIOINFO m_stmmIF;
	MMCKINFO m_stckOut,m_stckOutRIFF; 

	std::string m_saveFile;
	ASR::NetworkTrans *m_networkTrans;
	char *m_pInBuffer;
	char *m_pOutBuffer;
	int m_count;

public:
	afx_msg void OnCbSelFormats();
	afx_msg void OnEnChangeFilename();
	afx_msg void OnEnChangeStatus();
	afx_msg void OnBnClickedSendFile();
};

#endif  // _ASR_DEMO_DLG_H_