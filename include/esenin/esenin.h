#ifndef ESENIN_CLIENT_H
#define ESENIN_CLIENT_H

#include <string>
#include <vector>

struct PosWord {
    const std::string word;
    const std::string connection_label;
    const int connection_index;
    const std::string pos;

    PosWord(
        const std::string &word, 
        const std::string &connection_label, 
        int connection_index,
        const std::string &pos)
        : word(word), 
          connection_label(connection_label), 
          connection_index(connection_index), 
          pos(pos) 
    {}
};

class Client {

private:
    std::string ip;
    long port;

public:
    Client(std::string const& ip, long port);
    std::vector<PosWord> get_pos(std::string const& text);
    std::string fit_topics(std::vector<std::vector<std::string>> const& terms, int topics);
    std::vector<double> get_topics(std::string const& id, std::string const& term);
};

#endif