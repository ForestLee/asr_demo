#include "../stdafx.h"
#include "FileUtil.h"

namespace ASR {
	void FileUtil::WriteReadFileTest(char* str, char* out) {
		FILE* fp = NULL;

		fp = fopen("d:\\tmp\\test.txt", "w+");
		if (nullptr != str)
			fputs(str, fp);
		fclose(fp);

		fp = fopen("d:\\tmp\\test.txt", "r");
		if (nullptr != str) {
			fscanf(fp, "%s", out);
			printf("1: %s\n", out);

			fgets(out, 255, (FILE*)fp);
			printf("2: %s\n", out);

			fgets(out, 255, (FILE*)fp);
			printf("3: %s\n", out);
		}
		fclose(fp);
	}

	void FileUtil::WriteFileTest(char* str) {
		FILE* fp = NULL;

		fp = fopen("d:\\tmp\\test.txt", "w+");
		if (nullptr != str)
			fputs(str, fp);
		fclose(fp);
	}

}