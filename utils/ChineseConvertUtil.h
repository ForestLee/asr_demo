#ifndef _CHINESE_CONVERT_UTIL_H_
#define _CHINESE_CONVERT_UTIL_H_

namespace ASR {
	class ChineseConvertUtil {

	public:
		static void Utf8ToGB2312(const char* utf8, char* gb2312);
		static void GB2312ToUtf8(const char* gb2312, char* utf8);

	};

}
#endif //_CHINESE_CONVERT_UTIL_H_