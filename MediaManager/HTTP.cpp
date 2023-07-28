#include "pch.h"
#include "HTTP.h"

namespace HTTP {
	size_t WriteFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
		data->append((char*)ptr, size * nmemb);
		return size * nmemb;
	}

	size_t ImageWriteFunction(char* ptr, size_t size, size_t nmemb, void* userdata)
	{
		std::ofstream* out = static_cast<std::ofstream*>(userdata);
		size_t nbytes = size * nmemb;
		out->write(ptr, nbytes);
		return nbytes;
	}

	Response Get(std::string url, curl_slist* headers)
	{
		std::string response_string;
		CURL* session = curl_easy_init();

		curl_easy_setopt(session, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(session, CURLOPT_WRITEDATA, &response_string);
		curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, WriteFunction);
		curl_easy_setopt(session, CURLOPT_URL, url.c_str());

		curl_easy_setopt(session, CURLOPT_HTTPHEADER, headers);
		CURLcode ret = curl_easy_perform(session);
		int response_code;
		curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &response_code);
		curl_easy_cleanup(session);

		return HTTP::Response{
			response_code,
			response_string
		};
	}

	void DownloadFileTo(const char* url, const char* destination_path) {
		CURL* session;
		std::ofstream output_file(Helpers::UTF8ToPath(destination_path), std::ios::binary);
		CURLcode res;
		session = curl_easy_init();
		if (session)
		{
			curl_easy_setopt(session, CURLOPT_URL, url);
			curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, ImageWriteFunction);
			curl_easy_setopt(session, CURLOPT_WRITEDATA, &output_file);
			res = curl_easy_perform(session);
			curl_easy_cleanup(session);
		}

		output_file.close();
	}

	namespace RestJson {

		Response Get(std::string url)
		{
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "accept: application/json");
			return HTTP::Get(url, headers);
		}

		Response Get(std::string url, std::string bearer_token)
		{
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "accept: application/json");
			headers = curl_slist_append(headers, ("Authorization: Bearer " + bearer_token).c_str());
			
			return HTTP::Get(url, headers);
		}
	}
}