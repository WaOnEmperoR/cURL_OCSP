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
#include "MyUtils.h"

const char *content[10];

struct url_data {
	size_t size;
	unsigned char* data;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
	size_t index = data->size;
	size_t n = (size * nmemb);
	unsigned char* tmp;

	data->size += (size * nmemb);

#ifdef DEBUG
	fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
#endif
	tmp = (unsigned char*)realloc(data->data, data->size + 1); /* +1 for '\0' */

	if (tmp) {
		data->data = tmp;
	}
	else {
		if (data->data) {
			free(data->data);
		}
		fprintf(stderr, "Failed to allocate memory.\n");
		return 0;
	}

	std::cout << ptr << std::endl;

	memcpy((data->data + index), ptr, n);
	data->data[data->size] = '\0';

	return size * nmemb;
}

std::string format_account_number(int acct_no)
{
	char buffer[4];
	std::snprintf(buffer, sizeof(buffer), "%03d", acct_no);
	return buffer;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
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

//size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
//	size_t written = fwrite(ptr, size, nmemb, stream);
//	return written;
//}

static void read_response_file(char* filename)
{
	BIO *derbio;
	BIO *bio_err;

	OCSP_RESPONSE *resp = NULL;

	derbio = BIO_new_file(filename, "rb");
	resp = d2i_OCSP_RESPONSE_bio(derbio, NULL);
	BIO_free(derbio);

	if (!resp) {
		std::cout << "Error" << std::endl;
	}
	
	long l = ASN1_ENUMERATED_get(resp->responseStatus);
	std::cout << l << std::endl;
	std::cout << "Pembacaan Response Selesai" << std::endl;
}

static void one_request_ocsp_C(int tid)
{
	//char *base_path = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Save_Response_OCSP_";
	//char *nomor = (char*)malloc(2);
	//sprintf(nomor, "%02d", tid);

	//char *file_to_save = (char*)malloc(strlen(base_path) + strlen(nomor) + 4);

	//int temp = 0, ptr = 0;
	//while (base_path[temp] != '\0') {
	//	file_to_save[ptr] = base_path[temp];
	//	temp++;
	//	ptr++;
	//}
	//temp = 0;
	//while (nomor[temp] != '\0') {
	//	file_to_save[ptr] = nomor[temp];
	//	temp++;
	//	ptr++;
	//}
	//file_to_save[ptr] = '.';
	//ptr++;
	//file_to_save[ptr] = 'D';
	//ptr++;
	//file_to_save[ptr] = 'E';
	//ptr++;
	//file_to_save[ptr] = 'R';
	//ptr++;
	//file_to_save[ptr] = '\0';

	//std::cout << "Response File will be saved in : [" << file_to_save << "]" << std::endl;
	//std::cout << "--------------------------------------------------" << std::endl;

	//FILE* f_stream;
	//errno_t err;
	//err = fopen_s(&f_stream, file_to_save, "w");
	
	CURL *curl;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/ocsp-request");

	curl = curl_easy_init();
	if (curl)
	{
		unsigned char* bufferChar;

		curl_easy_setopt(curl, CURLOPT_URL, "http://ca1.govca.id/ejbca/publicweb/status/ocsp");
		curl_easy_setopt(curl, CURLOPT_POST, (long)1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content[tid]);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bufferChar);
		curl_easy_perform(curl); /* ignores error */

		std::cout << bufferChar << std::endl;

		free(bufferChar);

		curl_easy_cleanup(curl);
	}
	
}

static void one_request_ocsp_CPP(int tid)
{
	std::string readBuffer;

	CURL *curl;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/ocsp-request");

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://ca1.govca.id/ejbca/publicweb/status/ocsp");
	curl_easy_setopt(curl, CURLOPT_POST, (long)1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content[tid]);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);
	fflush(stdout);

	std::cout << "---------------------RESPONSE IN C++---------------------" << std::endl;
	std::cout << readBuffer << std::endl;
	std::cout << readBuffer.size() << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	//std::string extension = ".DER";
	//std::string path_simpan = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\OCSP_Response___" + format_account_number(tid) + extension;
	//std::cout << "Response File will be saved in : [" << path_simpan << "]" << std::endl;
	//std::cout << "--------------------------------------------------" << std::endl;
	
	char* writable = new char[readBuffer.size() + 1];
	std::copy(readBuffer.begin(), readBuffer.end(), writable);
	writable[readBuffer.size()] = '\0'; // don't forget the terminating 0

	std::cout << "---------------------RESPONSE IN C---------------------" << std::endl;
	std::cout << writable << std::endl;
	std::cout << strlen(writable) << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	//std::ofstream outfile(path_simpan, std::ofstream::binary);
	//outfile << readBuffer;
	std::cout.flush();
}

int main()
{
	int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

	const char *file_to_send = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\Request_5d3d22c2-6d74-4bdb-9b93-11351fd5cfc6.DER";
	
	const int NUMT = 4;
	std::thread tid[NUMT];
	int i;
	int error;

	for (int i = 0; i < NUMT; i++)
	{
		content[i] = load_file(file_to_send);
	}

	char cat[7];
	cat[0] = 0x63;
	cat[1] = 0x61;
	cat[2] = 0x20;
	cat[3] = 0x00;
	cat[4] = 0x51;
	cat[5] = 0x59;
	cat[6] = 0x6A;

	std::cout << cat << std::endl;

	curl_global_init(CURL_GLOBAL_ALL);

	for (i = 0; i < NUMT; i++)
	{
		Sleep(2000);
		tid[i] = std::thread(one_request_ocsp_CPP, i);
	}

	for (i = 0; i < NUMT; i++)
	{
		tid[i].join();
	}
	
	// If want to test the OCSP response reading process
	// char *filename = "C:\\Users\\Rachmawan\\Documents\\NetBeansProjects\\iOCSP\\OCSP_Response___003.DER";
	// read_response_file(filename);

	system("pause");
	return 0;
}

//#include "stdafx.h"
//#include <stdio.h>
//#include <curl/curl.h>
//#include <string.h>
//#include <stdlib.h>

//struct url_data {
//	size_t size;
//	char* data;
//};
//
//size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
//	size_t index = data->size;
//	size_t n = (size * nmemb);
//	char* tmp;
//
//	data->size += (size * nmemb);
//
//#ifdef DEBUG
//	fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
//#endif
//	tmp = (char*) realloc(data->data, data->size + 1); /* +1 for '\0' */
//
//	if (tmp) {
//		data->data = tmp;
//	}
//	else {
//		if (data->data) {
//			free(data->data);
//		}
//		fprintf(stderr, "Failed to allocate memory.\n");
//		return 0;
//	}
//
//	memcpy((data->data + index), ptr, n);
//	data->data[data->size] = '\0';
//
//	return size * nmemb;
//}
//
//char *handle_url(char* url) {
//	CURL *curl;
//
//	struct url_data data;
//	data.size = 0;
//	data.data = (char*) malloc(4096); /* reasonable size initial buffer */
//	if (NULL == data.data) {
//		fprintf(stderr, "Failed to allocate memory.\n");
//		return NULL;
//	}
//
//	data.data[0] = '\0';
//
//	CURLcode res;
//
//	curl = curl_easy_init();
//	if (curl) {
//		curl_easy_setopt(curl, CURLOPT_URL, url);
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
//		res = curl_easy_perform(curl);
//		if (res != CURLE_OK) {
//			fprintf(stderr, "curl_easy_perform() failed: %s\n",
//				curl_easy_strerror(res));
//		}
//
//		curl_easy_cleanup(curl);
//
//	}
//	return data.data;
//}
//
//int main(int argc, char* argv[]) {
//	char* data;
//
//	data = handle_url("https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html");
//
//	if (data) {
//		printf("%s\n", data);
//		free(data);
//	}
//
//	system("pause");
//	return 0;
//}