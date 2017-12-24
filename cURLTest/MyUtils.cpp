#include "stdafx.h"
#include "MyUtils.h"
#include <stdlib.h>
#include <string>

char* load_file(char const* path)
{
	char* buffer = 0;
	long length;
	FILE * f = fopen(path, "rb"); //was "rb"

	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc((length + 1) * sizeof(char));
		if (buffer)
		{
			fread(buffer, sizeof(char), length, f);
		}
		fclose(f);
	}
	buffer[length] = '\0';
	fflush(stdout);

	return buffer;
}

char* strndup(const char* s, size_t len)
{
	char *p = (char*)malloc(len + 1);
	memcpy(p, s, len + 1);

	return p;
}

