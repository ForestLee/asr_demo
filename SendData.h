#pragma once

class SendData {
private:
	SOCKET _socketFd;

public:
	SendData();
	~SendData();

	int tcpSend();
	int sendPcmData(char* pcmData, int len);
};