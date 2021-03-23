#pragma once
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

class WebRtcNS {

public:
	WebRtcNS();
	~WebRtcNS();

	int doNS(char* pInBuffer, char* pOutBuffer, int len, int nSample = 32000, int nMode = 2);
	int doAgc(char* pInBuffer, char* pOutBuffer, int length, int fs = 8000);
	void doNSAgc(std::string filePath);

private:
	void _noiseSuppression32(std::string input_path, std::string output_path, int nSample, int nMode);
	void _noiseSuppressionX32(char* szFileIn, char* szFileOut, int nSample, int nMode);
	void _agc(std::string input_path, std::string output_path, int fs);
	void _getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& files_alone);
};