#ifndef ESENIN_CLIENT_H
#define ESENIN_CLIENT_H

#include <string>
#include <vector>

class Client {

private:
    std::string ip;
    long port;

public:
    Client(std::string const& ip, long port);
    std::string get_pos(std::string const& text);
    // std::string fit_topics(std::vector<std::vector<std::string>> const& terms, int topics);
    // std::string get_topics(std::string const& id, std::string const& term);
};

#endif