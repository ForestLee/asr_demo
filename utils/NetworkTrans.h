#ifndef _NETWORK_TRANS_H_
#define _NETWORK_TRANS_H_

#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <STDIO.H>

namespace ASR {

	class NetworkTrans {
	private:
		SOCKET _socketFd;

	public:
		NetworkTrans();
		~NetworkTrans();

		int sendPcmData(char* fileName, char* out);
		int sendPcmData(char* pcmData, int len, char* out = nullptr);

	private:
		void _init();
		void _close();
		void dumpBuffer(char* buffer, int len);
	};

}
#endif // _NETWORK_TRANS_H_