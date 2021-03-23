// AsrDemoDlg.h : header file
//

#pragma once
#include <MMSystem.h>
#include "SendData.h"

#define MAX_BUFFERS	3
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

public:
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

	BOOL m_bRun;
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_stWFEX;
	WAVEHDR m_stWHDR[MAX_BUFFERS];
	HANDLE m_hThread;
	CString m_csErrorText;
	HMMIO m_hOPFile;
	MMIOINFO m_stmmIF;
	MMCKINFO m_stckOut,m_stckOutRIFF; 
	SendData *m_sendData;
	char *m_pcm_buffer;
	int m_count;

public:
	afx_msg void OnCbSelFormats();
	afx_msg void OnEnChangeFilename();
};
