#pragma once

class SendData {
private:
	SOCKET _sclient;

public:
	SendData();
	~SendData();

	int tcpSend();
	int sendPcmData(char* pcm_data, int len);
};