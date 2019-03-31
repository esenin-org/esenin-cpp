#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#include <curl/curl.h>

#include "esenin/esenin.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

size_t write_callback_esenin_server(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Client::call_esenin_server(std::string const& url, long port, rapidjson::Document const& data)
{
  CURL *curl = curl_easy_init();
  std::string result;

  if (curl) {

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
    curl_easy_setopt(curl, CURLOPT_PORT, port);    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString()); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_esenin_server);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result); 
 
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
 
    curl_easy_cleanup(curl);
  }

  return result;
}

Client::Client(std::string const& ip, long port)
{
  curl_global_init(CURL_GLOBAL_ALL);
  this->ip = ip;
  this->port = port;
}

Client::~Client()
{
  curl_global_cleanup();
}

std::string Client::get_pos(std::string const& text)
{
  rapidjson::Value json_val;
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  json_val.SetString(text.c_str(), allocator);
  doc.AddMember("string", json_val, allocator);

  return this->call_esenin_server(
    "http://" + this->ip + "/nlp/pos",
    this->port, 
    doc
  );
}

int main (int argc, char *argv[])
{
  Client client = Client(std::string("127.0.0.1"), 9000L);
  std::cout << client.get_pos(std::string("Мама мыла раму."));
  return 0;
}