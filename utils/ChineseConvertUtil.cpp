#include "../stdafx.h"
#include "ChineseConvertUtil.h"

namespace ASR {
	void ChineseConvertUtil::Utf8ToGB2312(const char* utf8, char* gb2312)
	{
		if (NULL == utf8 || NULL == gb2312) {
			return;
		}

		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

		memset(gb2312, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return;

	}

	void ChineseConvertUtil::GB2312ToUtf8(const char* gb2312, char* utf8)
	{
		if (NULL == gb2312 || NULL == utf8) {
			return;
		}

		int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);

		memset(utf8, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return;
	}
}