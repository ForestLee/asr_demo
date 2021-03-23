#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <STDIO.H>
#include "SendData.h"

SendData::SendData() {
	//_init();
}

void SendData::_init() {
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
	//serAddr.sin_port = htons(9995);
	//serAddr.sin_addr.S_un.S_addr = inet_addr("124.71.179.241");
	serAddr.sin_port = htons(5050);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(_socketFd, (struct sockaddr*) & serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !\n");
		closesocket(_socketFd);
	}
}

SendData::~SendData() {
	//_close();
}

void SendData::_close() {
	closesocket(_socketFd);
	WSACleanup();
}

int SendData::sendPcmData(char *fileName)
{
	_init();

	//int i = 0;
	int nFileSize = 0;

	FILE* fpIn = fopen(fileName, "rb");
	if (NULL == fpIn)
	{
		printf("open src file err \n");
	}
	fseek(fpIn, 0, SEEK_END);
	nFileSize = ftell(fpIn);
	fseek(fpIn, 0, SEEK_SET);

	char* pInBuffer = (char*)malloc(nFileSize);
	memset(pInBuffer, 0, nFileSize);
	fread(pInBuffer, sizeof(char), nFileSize, fpIn);

	int nTime = GetTickCount();
	send(_socketFd, pInBuffer, nFileSize, 0);
	nTime = GetTickCount() - nTime;
	printf("send time=%dms\n", nTime);

	char recData[255];
	memset(recData, 0, 255);
	int ret = recv(_socketFd, recData, 255, 0);
	nTime = GetTickCount() - nTime;
	printf("recv time=%dms\n", nTime);
	if (ret > 0)
	{
		printf("recv %d data: %s\n", ret, recData);
	}
	
	fclose(fpIn);
	free(pInBuffer);

	_close();
	return 0;
}

int SendData::sendPcmData(char *pcmData, int len)
{
	int nTime = GetTickCount();
	_init();
	nTime = GetTickCount() - nTime;
	printf("init time=%dms\n", nTime);
	send(_socketFd, pcmData, len, 0);
	nTime = GetTickCount() - nTime;
	printf("send time=%dms\n", nTime);
	char recData[255];
	memset(recData, 0, 255);
	int ret = recv(_socketFd, recData, 255, 0);
	nTime = GetTickCount() - nTime;
	printf("recv time=%dms\n", nTime);
	if (ret > 0)
	{
		printf("recv %d data: %s\n", ret, recData);
	}
	_close();
	return 0;
}