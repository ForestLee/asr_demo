#pragma once

class SendData {
private:
	SOCKET _socketFd;

public:
	SendData();
	~SendData();

	int sendPcmData(char* fileName, char *out);
	int sendPcmData(char* pcmData, int len);

private:
	void _init();
	void _close();
};