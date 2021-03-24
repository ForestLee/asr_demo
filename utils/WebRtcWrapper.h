#ifndef _WEB_RTC_UTIL_H_
#define _WEB_RTC_UTIL_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <Windows.h>
#include <fstream>  
#include <iterator>
#include <io.h>
#include <string>
#include <vector>

namespace WebRTCUtil {

	class WebRtcWrapper {

	public:


		static int doNS(char* pInBuffer, char* pOutBuffer, int len, int nSample = 32000, int nMode = 2);
		static int doAgc(char* pInBuffer, char* pOutBuffer, int length, int fs = 8000);
		static void doNSAgc(std::string filePath);

	private:
		static void _noiseSuppression32(std::string input_path, std::string output_path, int nSample, int nMode);
		static void _noiseSuppressionX32(char* szFileIn, char* szFileOut, int nSample, int nMode);
		static void _agc(std::string input_path, std::string output_path, int fs);
		static void _getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& files_alone);
	};

}
#endif  // _WEB_RTC_UTIL_H_