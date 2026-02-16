#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

// Create 2 types of nodes, an anchor is basically an email which an asset like a domain or an acc relies on 
enum class NodeType{
    Anchor, Asset
};

// Defining a node with an id, the type and what it is dependent on.
struct Node{
    std::string id;
    NodeType type;
    std::vector<std::string> dependencies;
};

// I'm tired of writing allat
using database_t = std::unordered_map<std::string, Node>; 

// Functions to return the database from the parsed file that will be later saved, basically a Saving system
database_t loadGraph();
void saveGraph(const database_t& graph);
void addNode(database_t& graph, int amountArgs, char* arguments[], NodeType typeOfNode);

// Linking system to add dependencies.
void linkNodes(database_t& graph, int argc, char* arguments[]);

std::unordered_set<std::string> simulate(const std::string& nodeName, const database_t& graph);

// Check if useless linking is done if A reaches B and B reaches C then A shouldn't reach C.
bool isReachable(const database_t& graph, const std::string& startID, const std::string& targetID);
bool dfs(const database_t& graph, std::unordered_set<std::string>& visited, const std::string& current, const std::string& target);

bool failedDependenciesCheck(const Node& potentialFailedNode, const std::unordered_set<std::string>& failedNodes);

