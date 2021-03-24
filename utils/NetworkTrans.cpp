#include "NetworkTrans.h"

namespace ASR {
	
#define RECV_SECOND_DATA   false

NetworkTrans::NetworkTrans() {

}

NetworkTrans::~NetworkTrans() {

}

void NetworkTrans::_init() {
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
	//int timeout = 3000; //3s
	//int ret = setsockopt(_socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	//ret = setsockopt(_socketFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

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




void NetworkTrans::_close() {
	closesocket(_socketFd);
	WSACleanup();
}

int NetworkTrans::sendPcmData(char *fileName, char *out)
{
	_init();

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
	printf("send time=%dms\n", GetTickCount() - nTime);

	nTime = GetTickCount();
	char recData[255];
	memset(recData, 0, 255);
	int ret = recv(_socketFd, recData, 255, 0);
	//if (1 == ret) {
	//	printf("recv 1 byte %x\n", (unsigned char)recData[0]);
	//}
	ret = recv(_socketFd, recData, 255, 0);
	
	int recvTime = GetTickCount() - nTime;
	printf("1 recv time=%dms\n", recvTime);
	printf("1 recv %d bytes:\n", ret);
	if (ret > 0)
	{
		memcpy(out, recData, ret);
	}

	if (RECV_SECOND_DATA) {
		nTime = GetTickCount();
		ret = recv(_socketFd, recData, 255, 0);
		printf("2 recv time=%dms\n", GetTickCount() - nTime);
		printf("2 recv %d bytes:\n", ret);
		if (ret > 0)
		{
			memcpy(out, recData, ret);
			int ii = 0;
			while (ii++ < ret)
				printf("%x  ", (unsigned char)recData[ii - 1]);

			printf("\n");
		}
	}
	fclose(fpIn);
	free(pInBuffer);

	_close();
	return recvTime;
}

int NetworkTrans::sendPcmData(char *pcmData, int len, char *out)
{
	int nTime = GetTickCount();
	_init();
	printf("init time=%dms\n", GetTickCount() - nTime);
	nTime = GetTickCount();
	send(_socketFd, pcmData, len, 0);
	printf("send time=%dms\n", GetTickCount() - nTime);

	nTime = GetTickCount();
	char recData[255];
	memset(recData, 0, 255);
	int ret = recv(_socketFd, recData, 255, 0);
	//if (1 == ret) {
	//	printf("recv 1 byte %x\n", (unsigned char)recData[0]);
	//}
	ret = recv(_socketFd, recData, 255, 0);
	int recvTime = GetTickCount() - nTime;
	printf("1 recv time=%dms\n", recvTime);
	printf("1 recv %d bytes:\n", ret);
	if (ret > 0)
	{
		memcpy(out, recData, ret);
	}

	if (RECV_SECOND_DATA) {
		nTime = GetTickCount();
		ret = recv(_socketFd, recData, 255, 0);
		printf("2 recv time=%dms\n", GetTickCount() - nTime);
		printf("2 recv %d bytes:\n", ret);
		if (ret > 0)
		{
			memcpy(out, recData, ret);
		}
	}
	_close();
	return recvTime;
}

}