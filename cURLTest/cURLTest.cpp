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
#include <iostream>
#include <fstream>
#include <thread>

const char *content[10];

/*static void*/ 
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

char* strndup(const char* s, size_t len)
{
	char *p = (char*) malloc(len+1);
	memcpy(p, s, len+1);

	return p;
}

/* the function to invoke as the data recieved */
static size_t write_callback_func(void *buffer,
	size_t size,
	size_t nmemb,
	void *userp)
{
	char **response_ptr = (char**)userp;

	/* assuming the response is a string */
	*response_ptr = strndup((char*) buffer, (size_t)(size *nmemb));

	return size * nmemb;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

std::string format_account_number(int acct_no) {
	char buffer[4];
	std::snprintf(buffer, sizeof(buffer), "%03d", acct_no);
	return buffer;
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
	fflush(stdout);

	return buffer;
}

static void one_request_ocsp(int tid)
{
	char *base_path = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Save_Response_OCSP_";
	char *nomor = (char*) malloc(2);
	sprintf(nomor, "%02d", tid);
		
	std::string readBuffer;

	char *file_to_save = (char*)malloc(strlen(base_path) + strlen(nomor) + 4);

	int temp = 0, ptr = 0;
	while (base_path[temp] != '\0') {                  
		file_to_save[ptr] = base_path[temp];
		temp++;
		ptr++;
	}
	temp = 0;
	while (nomor[temp] != '\0') {                   
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

	//std::cout << "[[" << file_to_save << "]]" << std::endl;

	//FILE* f_stream;
	//errno_t err;
	//err = fopen_s(&f_stream, file_to_save, "w");

	CURL *curl;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/ocsp-request");

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://ca1.govca.id/ejbca/publicweb/status/ocsp");
	curl_easy_setopt(curl, CURLOPT_POST, (long)1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content[tid]);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bufferChar);
	//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, f_stream);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);
	//fclose(f_stream);
	fflush(stdout);

	std::cout << "---------------------RESPONSE---------------------" << std::endl;
	std::cout << readBuffer << std::endl;
	std::cout << readBuffer.size() << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	std::string extension = ".DER";
	std::string path_simpan = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\OCSP_Response___" + format_account_number(tid) + extension;
	std::cout << "Will be saved in : " << path_simpan << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	
	std::ofstream outfile(path_simpan, std::ofstream::binary);
	outfile << readBuffer;
	
	//const char * c = readBuffer.c_str();
	//printf("%s\n", c);
	//std::cout << "---------------------" << std::endl;
	////int panjang = (sizeof(char));
	////panjang = panjang * readBuffer.size();
	////char *pk = (char*)malloc(panjang);
	//std::cout << "Alokasi" << sizeof(unsigned char) * readBuffer.size() << std::endl;
	//unsigned char *bufferChar = (unsigned char*) malloc(sizeof(unsigned char) * readBuffer.size());
	//for (int i = 0; i < readBuffer.size(); i++)
	//{
	//	bufferChar[i] = readBuffer.at(i);
	//}
	//std::cout << "---------------------" << std::endl;
	//printf("%s\n", bufferChar);
	////std::cout << bufferChar << std::endl;
	//std::cout << "---------------------" << std::endl;
}

int main()
{
	int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

	BIO *derbio;
	BIO *bio_err;
	const int NUMT = 4;

	CURL *curl;
	CURLcode res;

	OCSP_RESPONSE *resp = NULL;
	std::string readBuffer;
	const char *filename = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\OCSP_Response___003.DER";
	const char *file_to_send = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Request_5d3d22c2-6d74-4bdb-9b93-11351fd5cfc6.DER";
	const char *file_to_save = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Save_response_OCSP.DER";
	
	for (int i = 0; i < NUMT; i++)
	{
		content[i] = load_file(file_to_send);
	}
	
	derbio = BIO_new_file(filename, "rb");
	resp = d2i_OCSP_RESPONSE_bio(derbio, NULL);
	BIO_free(derbio);

	if (!resp) {
		std::cout << "Error" << std::endl;
	}
	
	long l = ASN1_ENUMERATED_get(resp->responseStatus);
	std::cout << l << std::endl;
	std::cout << "Pembacaan Response Selesai" << std::endl;
		
	/*std::thread tid[NUMT];
	int i;
	int error;

	curl_global_init(CURL_GLOBAL_ALL);

	for (i = 0; i < NUMT; i++)
	{
		one_request_ocsp(i);
		Sleep(2000);
		tid[i] = std::thread(one_request_ocsp, i);
	}*/
/*
	for (i = 0; i < NUMT; i++)
	{
		tid[i].join();
	}*/
	
	system("pause");

	return 0;
}