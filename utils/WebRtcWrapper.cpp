#include "WebRtcWrapper.h"

#include "../WebRtcMoudle/signal_processing_library.h"
#include "../WebRtcMoudle/noise_suppression_x.h"
#include "../WebRtcMoudle/noise_suppression.h"
#include "../WebRtcMoudle/gain_control.h"

using namespace std;

namespace WebRTCUtil {



	void WebRtcWrapper::_noiseSuppression32(string input_path, string output_path, int nSample, int nMode)
	{

		FILE* fpInputf, * fpOutput;
		const char* in_path = input_path.c_str();
		const char* out_path = output_path.c_str();

		int nRet = 0;
		NsHandle* pNS_inst = NULL;

		FILE* fpIn = NULL;
		FILE* fpOut = NULL;

		char* pInBuffer = NULL;
		char* pOutBuffer = NULL;

		do
		{
			int i = 0;
			int nFileSize = 0;
			int nTime = 0;
			if (0 != WebRtcNs_Create(&pNS_inst))
			{
				printf("Noise_Suppression WebRtcNs_Create err! \n");
				break;
			}

			if (0 != WebRtcNs_Init(pNS_inst, nSample))
			{
				printf("Noise_Suppression WebRtcNs_Init err! \n");
				break;
			}

			if (0 != WebRtcNs_set_policy(pNS_inst, nMode))
			{
				printf("Noise_Suppression WebRtcNs_set_policy err! \n");
				break;
			}

			fpIn = fopen(in_path, "rb");
			if (NULL == fpIn)
			{
				printf("open src file err \n");
				break;
			}
			fseek(fpIn, 0, SEEK_END);
			nFileSize = ftell(fpIn);
			fseek(fpIn, 0, SEEK_SET);

			pInBuffer = (char*)malloc(nFileSize);
			memset(pInBuffer, 0, nFileSize);
			fread(pInBuffer, sizeof(char), nFileSize, fpIn);

			pOutBuffer = (char*)malloc(nFileSize);
			memset(pOutBuffer, 0, nFileSize);

			int  filter_state1[6], filter_state12[6];
			int  Synthesis_state1[6], Synthesis_state12[6];

			memset(filter_state1, 0, sizeof(filter_state1));
			memset(filter_state12, 0, sizeof(filter_state12));
			memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
			memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

			nTime = GetTickCount();
			for (i = 0; i < nFileSize; i += 640)
			{
				if (nFileSize - i >= 640)
				{
					short shBufferIn[320] = { 0 };

					short shInL[160], shInH[160];
					short shOutL[160] = { 0 }, shOutH[160] = { 0 };

					memcpy(shBufferIn, (char*)(pInBuffer + i), 320 * sizeof(short));
					//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
					WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1, filter_state12);

					//将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
					if (0 == WebRtcNs_Process(pNS_inst, shInL, shInH, shOutL, shOutH))
					{
						short shBufferOut[320];
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
						WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
						memcpy(pOutBuffer + i, shBufferOut, 320 * sizeof(short));
					}
				}
			}

			nTime = GetTickCount() - nTime;
			printf("NS user time=%dms\n", nTime);
			fpOut = fopen(out_path, "wb");
			if (NULL == fpOut)
			{
				printf("open out file err! \n");
				break;
			}
			fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);
		} while (0);

		WebRtcNs_Free(pNS_inst);
		fclose(fpIn);
		fclose(fpOut);
		free(pInBuffer);
		free(pOutBuffer);
	}


	void WebRtcWrapper::_noiseSuppressionX32(char* szFileIn, char* szFileOut, int nSample, int nMode)
	{
		int nRet = 0;
		NsxHandle* pNS_inst = NULL;

		FILE* fpIn = NULL;
		FILE* fpOut = NULL;

		char* pInBuffer = NULL;
		char* pOutBuffer = NULL;

		do
		{
			int i = 0;
			int nFileSize = 0;
			int nTime = 0;
			if (0 != WebRtcNsx_Create(&pNS_inst))
			{
				printf("Noise_Suppression WebRtcNs_Create err! \n");
				break;
			}

			if (0 != WebRtcNsx_Init(pNS_inst, nSample))
			{
				printf("Noise_Suppression WebRtcNs_Init err! \n");
				break;
			}

			if (0 != WebRtcNsx_set_policy(pNS_inst, nMode))
			{
				printf("Noise_Suppression WebRtcNs_set_policy err! \n");
				break;
			}

			fpIn = fopen(szFileIn, "rb");
			if (NULL == fpIn)
			{
				printf("open src file err \n");
				break;
			}
			fseek(fpIn, 0, SEEK_END);
			nFileSize = ftell(fpIn);
			fseek(fpIn, 0, SEEK_SET);

			pInBuffer = (char*)malloc(nFileSize);
			memset(pInBuffer, 0, nFileSize);
			fread(pInBuffer, sizeof(char), nFileSize, fpIn);

			pOutBuffer = (char*)malloc(nFileSize);
			memset(pOutBuffer, 0, nFileSize);

			int  filter_state1[6], filter_state12[6];
			int  Synthesis_state1[6], Synthesis_state12[6];

			memset(filter_state1, 0, sizeof(filter_state1));
			memset(filter_state12, 0, sizeof(filter_state12));
			memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
			memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

			nTime = GetTickCount();
			for (i = 0; i < nFileSize; i += 640)
			{
				if (nFileSize - i >= 640)
				{
					short shBufferIn[320] = { 0 };

					short shInL[160], shInH[160];
					short shOutL[160] = { 0 }, shOutH[160] = { 0 };

					memcpy(shBufferIn, (char*)(pInBuffer + i), 320 * sizeof(short));
					//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
					WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1, filter_state12);

					//将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
					if (0 == WebRtcNsx_Process(pNS_inst, shInL, shInH, shOutL, shOutH))
					{
						short shBufferOut[320];
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
						WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
						memcpy(pOutBuffer + i, shBufferOut, 320 * sizeof(short));
					}
				}
			}

			nTime = GetTickCount() - nTime;
			printf("NS user time=%dms\n", nTime);
			fpOut = fopen(szFileOut, "wb");
			if (NULL == fpOut)
			{
				printf("open out file err! \n");
				break;
			}
			fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);
		} while (0);

		WebRtcNsx_Free(pNS_inst);
		fclose(fpIn);
		fclose(fpOut);
		free(pInBuffer);
		free(pOutBuffer);
	}

	void WebRtcWrapper::_agc(string input_path, string output_path, int fs)

	{
		FILE* infp = NULL;
		FILE* outfp = NULL;
		const char* in_path = input_path.c_str();
		const char* out_path = output_path.c_str();
		short* pData = NULL;
		short* pOutData = NULL;
		void* agcHandle = NULL;

		do
		{
			WebRtcAgc_Create(&agcHandle);

			int minLevel = 0;
			int maxLevel = 255;
			int agcMode = kAgcModeFixedDigital;
			WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, fs);

			WebRtcAgc_config_t agcConfig;
			agcConfig.compressionGaindB = 20;
			agcConfig.limiterEnable = 1;
			agcConfig.targetLevelDbfs = 3;
			WebRtcAgc_set_config(agcHandle, agcConfig);

			infp = fopen(in_path, "rb");
			int frameSize = 80;
			pData = (short*)malloc(frameSize * sizeof(short));
			pOutData = (short*)malloc(frameSize * sizeof(short));

			outfp = fopen(out_path, "wb");
			int len = frameSize * sizeof(short);
			int micLevelIn = 0;
			int micLevelOut = 0;
			while (TRUE)
			{
				memset(pData, 0, len);
				len = fread(pData, 1, len, infp);
				if (len > 0)
				{
					int inMicLevel = micLevelOut;
					int outMicLevel = 0;
					uint8_t saturationWarning;
					int nAgcRet = WebRtcAgc_Process(agcHandle, pData, NULL, frameSize, pOutData, NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
					if (nAgcRet != 0)
					{
						printf("failed in WebRtcAgc_Process\n");
						break;
					}
					micLevelIn = outMicLevel;
					fwrite(pOutData, 1, len, outfp);
				}
				else
				{
					break;
				}
			}
		} while (0);

		fclose(infp);
		fclose(outfp);
		free(pData);
		free(pOutData);
		WebRtcAgc_Free(agcHandle);
	}

	void WebRtcWrapper::_getFiles(string path, vector<string>& files, vector<string>& files_alone)
	{
		//文件句柄
		intptr_t hFile = 0;
		//文件信息
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之
				//如果不是,加入列表
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						_getFiles(p.assign(path).append("\\").append(fileinfo.name), files, files_alone);
				}
				else
				{
					files_alone.push_back(fileinfo.name);
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	void WebRtcWrapper::doNSAgc(std::string filePath)
	{
		string outAgcPath = filePath + ".agc.pcm";
		string outNsPath = filePath + ".ns.pcm";
		_noiseSuppression32(filePath.c_str(), outNsPath.c_str(), 32000, 2);
		_agc(outNsPath.c_str(), outAgcPath.c_str(), 8000);

		printf("保存增益、降噪文件\n");

	}

	int WebRtcWrapper::doNS(char* pInBuffer, char* pOutBuffer, int len, int nSample, int nMode)
	{
		int nRet = 0;
		NsHandle* pNS_inst = NULL;
		int nTime = 0;
		do
		{
			int i = 0;
			if (0 != WebRtcNs_Create(&pNS_inst))
			{
				printf("Noise_Suppression WebRtcNs_Create err! \n");
				break;
			}

			if (0 != WebRtcNs_Init(pNS_inst, nSample))
			{
				printf("Noise_Suppression WebRtcNs_Init err! \n");
				break;
			}

			if (0 != WebRtcNs_set_policy(pNS_inst, nMode))
			{
				printf("Noise_Suppression WebRtcNs_set_policy err! \n");
				break;
			}

			memset(pOutBuffer, 0, len);

			int  filter_state1[6], filter_state12[6];
			int  Synthesis_state1[6], Synthesis_state12[6];

			memset(filter_state1, 0, sizeof(filter_state1));
			memset(filter_state12, 0, sizeof(filter_state12));
			memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
			memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

			nTime = GetTickCount();
			for (i = 0; i < len; i += 640)
			{
				if (len - i >= 640)
				{
					short shBufferIn[320] = { 0 };

					short shInL[160], shInH[160];
					short shOutL[160] = { 0 }, shOutH[160] = { 0 };

					memcpy(shBufferIn, (char*)(pInBuffer + i), 320 * sizeof(short));
					//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
					WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1, filter_state12);

					//将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
					if (0 == WebRtcNs_Process(pNS_inst, shInL, shInH, shOutL, shOutH))
					{
						short shBufferOut[320];
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
						WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
						memcpy(pOutBuffer + i, shBufferOut, 320 * sizeof(short));
					}
				}
			}

			nTime = GetTickCount() - nTime;
			// printf("NS time=%dms\n", nTime);

		} while (0);

		WebRtcNs_Free(pNS_inst);
		return nTime;
	}



	int WebRtcWrapper::doAgc(char* pInBuffer, char* pOutBuffer, int length, int fs)
	{
		int count = 0;
		int i = 0;
		short* pData = (short*)pInBuffer;
		short* pOutData = (short*)pOutBuffer;
		void* agcHandle = NULL;
		int nTime = GetTickCount();
		do
		{
			WebRtcAgc_Create(&agcHandle);

			int minLevel = 0;
			int maxLevel = 255;
			int agcMode = kAgcModeFixedDigital;
			WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, fs);

			WebRtcAgc_config_t agcConfig;
			agcConfig.compressionGaindB = 20;
			agcConfig.limiterEnable = 1;
			agcConfig.targetLevelDbfs = 3;
			WebRtcAgc_set_config(agcHandle, agcConfig);

			int frameSize = 80;
			int len = frameSize * sizeof(short);

			int micLevelIn = 0;
			int micLevelOut = 0;
			while (count + len <= length)
			{
				int inMicLevel = micLevelOut;
				int outMicLevel = 0;
				uint8_t saturationWarning;
				int nAgcRet = WebRtcAgc_Process(agcHandle, &pData[frameSize * i], NULL, frameSize, &pOutData[frameSize * i], NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
				count += len;
				i++;
				if (nAgcRet != 0)
				{
					printf("failed in WebRtcAgc_Process\n");
					break;
				}
				micLevelIn = outMicLevel;
			}
		} while (0);

		WebRtcAgc_Free(agcHandle);
		nTime = GetTickCount() - nTime;
		//printf("AGC time=%dms\n", nTime);
		return nTime;
	}

}