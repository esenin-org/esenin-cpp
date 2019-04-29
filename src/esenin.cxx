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

std::vector<std::string> Client::tokenize(std::string const& text)
{
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  doc.AddMember("text", rapidjson::Value(text.c_str(), allocator).Move(), allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/token",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_tokens = result_doc["tokens"];

  std::vector<std::string> tokens;
  tokens.reserve(value_tokens.Size());
  for (rapidjson::SizeType i = 0; i < value_tokens.Size(); i++) {
    tokens.emplace_back(std::string(value_tokens[i].GetString()));
  }

  return tokens;
}

std::vector<std::string> Client::sentenize(std::string const& text)
{
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  doc.AddMember("text", rapidjson::Value(text.c_str(), allocator).Move(), allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/sentence",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_sentences = result_doc["sentences"];

  std::vector<std::string> sentences;
  sentences.reserve(value_sentences.Size());
  for (rapidjson::SizeType i = 0; i < value_sentences.Size(); i++) {
    sentences.emplace_back(std::string(value_sentences[i].GetString()));
  }

  return sentences;
}

std::vector<std::string> Client::get_pos(std::vector<std::string> const& tokens)
{
  rapidjson::Value tokens_val;
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  tokens_val.SetArray();

  for (size_t i = 0; i < tokens.size(); i++) {
    tokens_val.PushBack(rapidjson::Value(tokens[i].c_str(), allocator), allocator);
  }

  doc.AddMember("tokens", tokens_val, allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/pos",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_pos = result_doc["pos"];

  std::vector<std::string> pos;
  pos.reserve(value_pos.Size());
  for (rapidjson::SizeType i = 0; i < value_pos.Size(); i++) {
    pos.emplace_back(std::string(value_pos[i].GetString()));
  }

  return pos;
}

std::vector<DependencyTreeNode> Client::get_dependency_tree(std::vector<std::string> const& tokens)
{
  rapidjson::Value tokens_val;
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  tokens_val.SetArray();

  for (size_t i = 0; i < tokens.size(); i++) {
    tokens_val.PushBack(rapidjson::Value(tokens[i].c_str(), allocator), allocator);
  }

  doc.AddMember("tokens", tokens_val, allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/dtree",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_nodes = result_doc["nodes"];

  std::vector<DependencyTreeNode> nodes;
  nodes.reserve(value_nodes.Size());  
  for (rapidjson::SizeType i = 0; i < value_nodes.Size(); i++) {
    const rapidjson::Value& value_node = value_nodes[i];
    nodes.emplace_back(DependencyTreeNode(
      std::string(value_node["label"].GetString()),
      value_node["parent"].GetInt()
    ));
  }

  return nodes;
}

std::vector<NamedEntity> Client::get_named_entities(std::vector<std::string> const& tokens)
{
  rapidjson::Value tokens_val;
  rapidjson::Document doc;
  auto& allocator = doc.GetAllocator();

  doc.SetObject();
  tokens_val.SetArray();

  for (size_t i = 0; i < tokens.size(); i++) {
    tokens_val.PushBack(rapidjson::Value(tokens[i].c_str(), allocator), allocator);
  }

  doc.AddMember("tokens", tokens_val, allocator);

  std::string response = call_esenin_server(
    "http://" + this->ip + "/nlp/ne",
    this->port, 
    doc
  );

  rapidjson::Document result_doc;

  result_doc.Parse(response.c_str());
  const rapidjson::Value& value_entities = result_doc["entities"];

  std::vector<NamedEntity> entities;
  entities.reserve(value_entities.Size());  
  for (rapidjson::SizeType i = 0; i < value_entities.Size(); i++) {
    const rapidjson::Value& value_entity = value_entities[i];

    std::vector<int> indexes;
    const rapidjson::Value& value_indexes = value_entity["indexes"];
    for (rapidjson::SizeType j = 0; j < value_indexes.Size(); j++) {
      indexes.emplace_back(value_indexes[j].GetInt());
    }
    entities.emplace_back(NamedEntity(
      indexes,
      std::string(value_entity["kind"].GetString())
    ));
  }

  return entities;
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