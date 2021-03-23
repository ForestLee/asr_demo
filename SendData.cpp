#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <STDIO.H>
#include "SendData.h"

SendData::SendData() {
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		printf("WSAStartup fail !\n");
	}

	_socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socketFd == INVALID_SOCKET)
	{
		printf("invalid socket !\n");
	}
	int timeout = 3000; //3s
	int ret = setsockopt(_socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	ret = setsockopt(_socketFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(9995);
	serAddr.sin_addr.S_un.S_addr = inet_addr("124.71.179.241");
	if (connect(_socketFd, (struct sockaddr*) & serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !\n");
		closesocket(_socketFd);
	}
}

SendData::~SendData() {
	closesocket(_socketFd);
	WSACleanup();
}

int SendData::tcpSend()
{

	for (int i = 0; i < 10; i++)
	{

		char* sendData = "·¢ËÍÊý¾Ý²âÊÔ\n";
		send(_socketFd, sendData, strlen(sendData), 0);
		char recData[255];
		int ret = recv(_socketFd, recData, 255, 0);
		if (ret > 0)
		{
			printf("%s\n", recData);
		}
	}

	return 0;
}

int SendData::sendPcmData(char *pcmData, int len)
{

	send(_socketFd, pcmData, len, 0);
	char recData[255];
	int ret = recv(_socketFd, recData, 255, 0);
	if (ret > 0)
	{
		printf("%s\n", recData);
	}

	return 0;
}