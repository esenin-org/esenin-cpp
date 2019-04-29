#ifndef ESENIN_CLIENT_H
#define ESENIN_CLIENT_H

#include <string>
#include <vector>

struct DependencyTreeNode {
    const std::string label;
    const int parent;

    DependencyTreeNode(const std::string &label, int parent): label(label), parent(parent) {}
};

struct NamedEntity {
    const std::vector<int> indexes;
    const std::string kind;

    NamedEntity(const std::vector<int> &indexes, const std::string &kind): indexes(indexes), kind(kind) {}
};

class Client {

private:
    std::string ip;
    long port;

public:
    Client(std::string const& ip, long port);
    std::vector<std::string> tokenize(std::string const& text);
    std::vector<std::string> sentenize(std::string const& text);
    std::vector<std::string> get_pos(std::vector<std::string> const& tokens);
    std::vector<DependencyTreeNode> get_dependency_tree(std::vector<std::string> const& tokens);
    std::vector<NamedEntity> get_named_entities(std::vector<std::string> const& tokens);
    std::string fit_topics(std::vector<std::vector<std::string>> const& terms, int topics);
    std::vector<double> get_topics(std::string const& id, std::string const& term);
};

#endif