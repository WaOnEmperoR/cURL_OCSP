# cURL_OCSP
This application is created to send OCSP request via HTTP using cURL library in desktop application

=== MANUAL INSTALASI cURL KE DALAM PROJECT ===

1.	Klik kanan pada project, pilih Properties. Kemudian Configuration Properties  VC++ Directories  Include Directories  Tambahkan entry path folder third-party/libcurl/include.
2.	Klik kanan pada project, pilih Properties. Kemudian Configuration Properties  VC++ Directories  VC++ Directories  Tambahkan entry path folder third-party/libcurl/lib/static-release-x86 dan third-party/libcurl/lib/static-debug-x86.
3.	Klik kanan pada project, pilih Properties. Kemudian C/C++  Preprocessor  Preprocessor Definitions  Tambahkan entry CURL_STATICLIB.
4.	Klik kanan pada project, pilih Properties. Kemudian Linker  General  Additional Library Directories  Tambahkan entry path folder third-party/libcurl/lib/static-release-x86 dan third-party/libcurl/lib/static-debug-x86.
5.	Klik kanan pada project, pilih Properties. Kemudian Linker  Input  Additional Dependencies  Tambahkan entry path file third-party\libcurl\lib\static-debug-x86/libcurl_a_debug.lib

=== MANUAL INSTALASI OpenSSL KE DALAM PROJECT ===

1.	 Klik kanan pada project, pilih Properties. Kemudian Configuration Properties  VC++ Directories  Include Directories  Tambahkan entry path folder openssl-1.0.2j-64bit-debug-dll-vs2015\include dan openssl-1.0.2j-64bit-release-dll-vs2015\include. 
2.	Klik kanan pada project, pilih Properties. Kemudian Configuration Properties  VC++ Directories  Library Directories  Tambahkan entry path folder opensl-1.0.2j-64bit-debug-dll-vs2015\lib dan opensl-1.0.2j-64bit-release-dll-vs2015\lib.
3.	Klik kanan pada project, pilih Properties. Kemudian Linker  General  Additional Library Directories  Tambahkan entry path folder openssl-1.0.2j-64bit-release-dll-vs2015\lib dan openssl-1.0.2j-64bit-debug-dll-vs2015\lib.
4.	Klik kanan pada project, pilih Properties. Kemudian Linker  Input  Additional Dependencies  Tambahkan entry library openssl-1.0.2j-64bit-release-dll-vs2015\lib\libeay32.lib, openssl-1.0.2j-64bit-release-dll-vs2015\lib\ssleay32.lib, openssl-1.0.2j-64bit-release-dll-vs2015\lib\libeay32MD.lib, openssl-1.0.2j-64bit-release-dll-vs2015\lib\ssleay32MD.lib, openssl-1.0.2j-64bit-release-dll-vs2015\lib\libeay32MT.lib, openssl-1.0.2j-64bit-release-dll-vs2015\lib\ssleay32MT.lib, dan C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\legacy_stdio_definitions.lib. Library-library MT dan MD diperlukan karena library ini di prakompilasi pada lingkungan multithread. Selain itu kita juga harus menambahkan definisi untuk library stdio lama, karena library OpenSSL ini juga dikompilasi dengan library stdio lama, yang berbeda dengan Visual Studio 2015 terbaru yang menggunakan definisi library stdio yang baru.
5.	Klik kanan pada project, pilih Properties. Kemudian C/C++  Preprocessor  Preprocessor Definitions  Tambahkan entry _CRT_SECURE_NO_WARNINGS.
6.	Tambahkan kode berikut di file stdafx.h, untuk mendeskripsikan library-library standar C lama.
    FILE _iob[] = { *stdin, *stdout, *stderr };
    extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
7.	Terakhir, untuk menggunakan definisi X509 dan OCSP Response dari library OpenSSL, lepaskan definisi untuk X509 dan OCSP Response dari library Wincrypt. Tambahkan kode berikut ini di antara kode-kode untuk #include.
    #if defined(X509_NAME) && defined(__WINCRYPT_H__)
    #  undef X509_NAME
    #endif
    #if defined(OCSP_RESPONSE) && defined(__WINCRYPT_H__)
    #  undef OCSP_RESPONSE
    #endif
