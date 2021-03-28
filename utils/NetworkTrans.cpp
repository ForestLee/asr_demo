#include "NetworkTrans.h"

namespace ASR {
	
#define RECV_SECOND_DATA   true

//#define DEBUG  true

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
	serAddr.sin_addr.S_un.S_addr = inet_addr("10.43.16.31");
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

	int recvTime = sendPcmData(pInBuffer, nFileSize, out);

	fclose(fpIn);
	free(pInBuffer);

	_close();
	return recvTime;
}

void NetworkTrans::dumpBuffer(char *buffer, int len) {
#ifdef DEBUG
	// printf("dump %d bytes:\n", len);
	for (int i = 0; i < len; i++) {
		printf("%x  ", (unsigned char)buffer[i]);
	}
	printf("\n");
#endif
}

int NetworkTrans::sendPcmData(char *pcmData, int len, char *out)
{
	int nTime = GetTickCount();
	_init();
#ifdef DEBUG
	printf("init time=%dms\n", GetTickCount() - nTime);
#endif
	nTime = GetTickCount();
	send(_socketFd, pcmData, len, 0);
#ifdef DEBUG
	printf("send time=%dms\n", GetTickCount() - nTime);
#endif

//	nTime = GetTickCount();
//	char recData[255];
//	memset(recData, 0, 255);
//	int ret = recv(_socketFd, recData, 255, 0);
//#ifdef DEBUG
//	if (ret > 0) {
//		dumpBuffer(recData, ret);
//	}
//#endif
//	ret = recv(_socketFd, recData, 255, 0);
//	int recvTime = GetTickCount() - nTime;
//	if (ret > 0)
//	{
//#ifdef DEBUG
//		printf("1 recv time=%dms\n", recvTime);
//		dumpBuffer(recData, ret);
//#endif
//		memcpy(out, recData, ret);
//	}
//
//	if (RECV_SECOND_DATA) {
//		nTime = GetTickCount();
//		ret = recv(_socketFd, recData, 255, 0);
//		if (ret > 0)
//		{
//#ifdef DEBUG
//			printf("2 recv time=%dms\n", GetTickCount() - nTime);
//			dumpBuffer(recData, ret);
//#endif
//			memcpy(out, recData, ret);
//		}
//	}

	int recvTime = 0;
	int times = 0;
	bool isEnd = false;
	int i = 0;
	nTime = GetTickCount();
	while (true) {
	    char recData[255];
        memset(recData, 0, 255);
		
        int ret = recv(_socketFd, recData, 255, 0);
		
		if (ret > 0)
		{
			memcpy(&out[i], recData, ret);
			i += ret;
		}
		else {
			break;
		}

		if (out[i - 1] == 0x0a || out[i - 1] == 0x0d) {  // 0x0d, 中间结果; 0x0a, 最终结果
	#ifdef DEBUG
			printf("recv #%d time=%dms %d bytes:\t", times, recvTime, i);
			dumpBuffer(out, i);
	#endif
			//out[i - 1] = 0;
			i = 0;
		}
		times++;
	}
	recvTime = GetTickCount() - nTime;
	_close();
	return recvTime;
}

}