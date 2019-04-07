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

std::string call_esenin_server(std::string const& url, 
                               long port, 
                               rapidjson::Document const& data)
{
  curl_global_init(CURL_GLOBAL_ALL);

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
    curl_global_cleanup();
  }

  return result;
}

Client::Client(std::string const& ip, long port)
{
  this->ip = ip;
  this->port = port;
}

std::vector<PosWord> Client::get_pos(std::string const& text)
{
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  doc.AddMember("text", rapidjson::Value(text.c_str(), allocator).Move(), allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/pos",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_words = result_doc["words"];

  std::vector<PosWord> words;
  words.reserve(value_words.Size());
  for (rapidjson::SizeType i = 0; i < value_words.Size(); i++) {
    const rapidjson::Value& value_word = value_words[i];
    words.emplace_back(PosWord(
      std::string(value_word["word"].GetString()), 
      std::string(value_word["connection_label"].GetString()),
      value_word["connection_index"].GetInt(),
      std::string(value_word["pos"].GetString())
    ));
  }

  return words;
}

std::string Client::fit_topics(std::vector<std::vector<std::string>> const& terms, int topics)
{
  rapidjson::Value terms_val;
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  terms_val.SetArray();
  for (size_t i = 0; i < terms.size(); i++) {
    rapidjson::Value nested_terms_val;
    nested_terms_val.SetArray();
    for (size_t j = 0; j < terms[i].size(); j++) {
      nested_terms_val.PushBack(rapidjson::Value(terms[i][j].c_str(), allocator), allocator);
    }
    terms_val.PushBack(nested_terms_val, allocator);
  }
  doc.AddMember("terms", terms_val, allocator);
  doc.AddMember("topics", topics, allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/tm/fit",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  return result_doc["id"].GetString();
}

std::vector<double> Client::get_topics(std::string const& id, std::string const& term)
{
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  doc.AddMember("id", rapidjson::Value(id.c_str(), allocator), allocator);
  doc.AddMember("term", rapidjson::Value(term.c_str(), allocator), allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/tm/topics",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_topics = result_doc["topics"];

  std::vector<double> topics;
  topics.reserve(value_topics.Size());
  for (rapidjson::SizeType i = 0; i < value_topics.Size(); i++) {
    topics.emplace_back(value_topics[i].GetDouble());
  }

  return topics;
}