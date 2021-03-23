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
		printf("WSAStartup fail !");
	}

	_sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(9995);
	serAddr.sin_addr.S_un.S_addr = inet_addr("124.71.179.241");
	if (connect(_sclient, (struct sockaddr*) & serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(_sclient);
	}
}

SendData::~SendData() {
	closesocket(_sclient);
	WSACleanup();
}

int SendData::tcpSend()
{

	for (int i = 0; i < 10; i++)
	{

		char* sendData = "·¢ËÍÊý¾Ý²âÊÔ\n";
		send(_sclient, sendData, strlen(sendData), 0);
		char recData[255];
		/*int ret = recv(sclient, recData, 255, 0);
		if (ret > 0)
		{

			recData[ret] = 0x00;
			//printf(recData);
			printf("%d%s",i,recData);
		}
		*/
	}

	return 0;
}

int SendData::sendPcmData(char *pcm_data, int len)
{

	send(_sclient, pcm_data, len, 0);
	char recData[255];
	/*int ret = recv(sclient, recData, 255, 0);
	if (ret > 0)
	{

		recData[ret] = 0x00;
		//printf(recData);
		printf("%d%s",i,recData);
	}
	*/

	return 0;
}