#include "stdafx.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <wincrypt.h>
#if defined(X509_NAME) && defined(__WINCRYPT_H__)
#  undef X509_NAME
#endif
#if defined(OCSP_RESPONSE) && defined(__WINCRYPT_H__)
#  undef OCSP_RESPONSE
#endif
#include <curl\curl.h>
#include <openssl/opensslv.h>
#include <openssl/conf.h>
#include <openssl/e_os2.h>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/ocsp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <thread>

const char *content;

/*static void*/ 
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

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

	return buffer;
}

static void one_request_ocsp(int tid)
{
	char *base_path = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Save_Response_OCSP_";
	char *nomor = (char*) malloc(2);
	sprintf(nomor, "%02d", tid);

	char *file_to_save = (char*)malloc(strlen(base_path) + strlen(nomor) + 4);

	int temp = 0, ptr = 0;
	while (base_path[temp] != '\0') {                   /* and string two */
		file_to_save[ptr] = base_path[temp];
		temp++;
		ptr++;
	}
	temp = 0;
	while (nomor[temp] != '\0') {                   /* and string two */
		file_to_save[ptr] = nomor[temp];
		temp++;
		ptr++;
	}
	file_to_save[ptr] = '.';
	ptr++;
	file_to_save[ptr] = 'D';
	ptr++;
	file_to_save[ptr] = 'E';
	ptr++;
	file_to_save[ptr] = 'R';
	ptr++;
	file_to_save[ptr] = '\0';

	std::cout << "[[" << file_to_save << "]]" << std::endl;

	FILE* f_stream;
	errno_t err;
	err = fopen_s(&f_stream, file_to_save, "w");

	CURL *curl;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/ocsp-request");

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://ca1.govca.id/ejbca/publicweb/status/ocsp");
	curl_easy_setopt(curl, CURLOPT_POST, (long)1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, f_stream);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);

}

int main()
{
	int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

	BIO *derbio;
	BIO *bio_err;

	CURL *curl;
	CURLcode res;

	OCSP_RESPONSE *resp = NULL;
	std::string readBuffer;
	const char *filename = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Response_5d3d22c2-6d74-4bdb-9b93-11351fd5cfc6.DER";
	const char *file_to_send = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Request_5d3d22c2-6d74-4bdb-9b93-11351fd5cfc6.DER";
	const char *file_to_save = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Save_response_OCSP.DER";
	
	content = load_file(file_to_send);

	derbio = BIO_new_file(filename, "rb");
	resp = d2i_OCSP_RESPONSE_bio(derbio, NULL);
	BIO_free(derbio);

	if (!resp) {
		std::cout << "Error" << std::endl;
	}
	
	long l = ASN1_ENUMERATED_get(resp->responseStatus);
	std::cout << l << std::endl;
	std::cout << "Pembacaan Response Selesai" << std::endl;

	const int NUMT = 5;
	std::thread tid[NUMT];
	int i;
	int error;

	curl_global_init(CURL_GLOBAL_ALL);

	for (i = 0; i < NUMT; i++)
	{
		tid[i] = std::thread(one_request_ocsp, i);
	}

	for (i = 0; i < NUMT; i++)
	{
		tid[i].join();
	}
	
	system("pause");

	return 0;
}