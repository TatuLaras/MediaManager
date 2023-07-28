#pragma once
#include "Helpers.h"

namespace HTTP {
	struct Response {
		int status_code = 0;
		std::string data;
	};

	size_t WriteFunction(void* ptr, size_t size, size_t nmemb, std::string* data);

	size_t ImageWriteFunction(char* ptr, size_t size, size_t nmemb, void* userdata);

	Response Get(std::string url, struct curl_slist* headers = NULL);

	void DownloadFileTo(const char* url, const char* destination_path);

	namespace RestJson {
		Response Get(std::string url);
		Response Get(std::string url, std::string bearer_token);
	} 
}