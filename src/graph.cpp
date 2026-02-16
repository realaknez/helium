#include "include/graph.hpp"
#include <iostream>
#include <fstream>

database_t loadGraph() {
    // Get graph.db and create an empty graph to be returned with the parsed info.
    std::ifstream GRAPH_FILE("graph.db");
    database_t graph;

    // Check if exists or opened
    if (!GRAPH_FILE.is_open()) {
        return graph;
    }
    
    // Parse every line, honestly i have no clue how the for loop works but it does work.
    for (std::string line; std::getline(GRAPH_FILE, line);){
        if (line.empty())
            continue;
        
        Node newNode;
        NodeType type;
        std::vector<std::string> dependencies;

        // File is structured basically id|type|dependencies so extract until | and then delete it to repeat
        std::string idString = line.substr(0, line.find('|'));
        line.erase(0, line.find('|')+1);
        std::string typeString = line.substr(0, line.find('|'));
        line.erase(0, line.find('|')+1);
        std::string dependenciesString = line;

        // Assing basically type based on the typeText
        if (typeString == "anchor"){
            type = NodeType::Anchor;
        } else {
            type = NodeType::Asset;
        }

        // Make dependency by expecting a coma in the end of it if not then the coma will be npos.
        while (!dependenciesString.empty()){
            size_t pos = dependenciesString.find(',');

            if (pos == std::string::npos){
                dependencies.push_back(dependenciesString.substr(0, dependenciesString.length()));
                break;
            } else {
                dependencies.push_back(dependenciesString.substr(0, pos));
                dependenciesString.erase(0, pos+1);
            }
        } 
        
        // Replace all the values in the newnode and put it in the graph with the name (id in the node) and the node itself
        newNode.id = idString;
        newNode.type = type;
        newNode.dependencies = dependencies;
        graph.emplace(idString, newNode);
    }  
    return graph;
}

void saveGraph(const database_t& graph) {
    std::ofstream GRAPH_FILE("graph.db");

    // Safety measure
    if (!GRAPH_FILE.is_open()){
        std::cout << ("Helium -- Graph couldn't be opened") << std::endl;
        return;
    }

    // Loop through every node in the graph to parse it on the file
    for (auto const &i : graph) {
        std::string type;
        std::string dependencies;

        if (i.second.type == NodeType::Anchor){
            type = "anchor";
        } else {
            type = "asset";
        }

        // Parse dependencies with a coma as a separator, and just this basic first boolean to not put coma at front
        bool first = true;
        for (auto itr : i.second.dependencies) {
            if (first){
                dependencies.append(itr);
                first = false;
            } else {
                dependencies.append(","+itr);
            }
        }

        // Replace it all and put in a string to be saved and continue.
        std::string savedData = i.second.id + "|" + type + "|" + dependencies;
        GRAPH_FILE << savedData << "\n";
    }
}

void addNode(database_t& graph, int amountArgs, char* arguments[], NodeType typeOfNode){
    // Check if name is empty or else define new node
    if (amountArgs < 3) {
            std::cout << "Helium -- Empty name" << std::endl;
    } else {
        std::string argument2 = arguments[2];
        auto itr = graph.find(argument2);

        if (itr == graph.end()){
            // Check if name already used/exists and create new node if not and add it to the unorganized map
            Node newNode;
            newNode.id = argument2;
            newNode.type = typeOfNode;

            graph.emplace(std::pair<std::string, Node> {argument2, newNode});
            std::cout << "Added " << argument2 << " succesfully!" << std::endl;
        } else {
            std::cout << "Helium -- Name already in use!" << std::endl;
        }
    }
}

void linkNodes(database_t& graph, int argc, char* arguments[]){
    if (argc < 4){
        std::cout << "Helium -- Low amount of arguments!" << std::endl;
        return;
    }

    // Again using string functions to make it easier.
    std::string sourceID = arguments[2];
    std::string targetID = arguments[3];

    // Check if in database initial node and final node exist
    if (graph.find(sourceID) != graph.end() && graph.find(targetID) != graph.end()){

        // An asset cannot be linked to an asset, as well as an anchor to an asset
        if (graph[sourceID].type == NodeType::Asset && graph[targetID].type == NodeType::Asset){
            std::cout << "Invalid type of nodes, you cannot link an asset to an asset!" << std::endl;
            return;
        } else if (graph[sourceID].type == NodeType::Anchor && graph[targetID].type == NodeType::Asset){
            std::cout << "Invalid type of nodes, you cannot link an anchor to an asset!" << std::endl;
            return;
        } else if (graph[sourceID].id == graph[targetID].id)
        {
            std::cout << "Invalid nodes, they are the same node!" << std::endl;
            return;
        }  // Check to see if useless linking is being done (line 32 for more information)
        else if (isReachable(graph, targetID, sourceID))
        {
            std::cout << "Helium -- This link would create a circular dependency!" << std::endl;
            return;
        } // Last check for if dependency already was added or exists
        if(std::find(graph[sourceID].dependencies.begin(), graph[sourceID].dependencies.end(), graph[targetID].id) != graph[sourceID].dependencies.end()){
            std::cout << "Helium -- Dependency already found in source!" << std::endl;
            return;
        } else{
            graph[sourceID].dependencies.push_back(graph[targetID].id);
            std::cout << "Added new link successfuly!";
        }
    } else {
        std::cout << "Helium -- Couldn't find node/s, use list to see all the nodes." << std::endl;
        return;
    }
}

// Loop through every string in dependencies to see if every dependency has failed if not it is still up.
bool failedDependenciesCheck(const Node& potentialFailedNode, const std::unordered_set<std::string>& failedNodes){
    for (const std::string i : potentialFailedNode.dependencies){
        if (failedNodes.find(i) == failedNodes.end()){
            return false;
        } 
    }

    return true;
}

std::unordered_set<std::string> simulate(const std::string& nodeName, const database_t& graph){
    std::unordered_set<std::string> failed;
    failed.emplace(nodeName);

    // Loop through graph to find failed nodes by checking dependencies
    bool changed = true;
    while (changed){
        changed = false;
        for (const auto& itr : graph){
            if(failed.find(itr.first) == failed.end()){
                if (!itr.second.dependencies.empty() && failedDependenciesCheck(itr.second, failed)){
                    failed.emplace(itr.first);
                    changed = true;
                } 
            } 
        }
    }

    return failed;
}

bool isReachable(const database_t& graph, const std::string& startID, const std::string& targetID) {
    std::unordered_set<std::string> visited;
    return dfs(graph, visited, startID, targetID);
}

// Dfs system for checking useless linking
bool dfs(const database_t& graph, std::unordered_set<std::string>& visited, const std::string& current, const std::string& target){
    if (current == target)
        return true;
    
    visited.insert(current);

    // Recursive iteration to navigate through the graph
    for (const auto& itr: graph.at(current).dependencies){ 
        if (visited.find(itr) == visited.end()){
            if (dfs(graph, visited, itr, target))
                return true;
        }
    }
    
    return false;
}
