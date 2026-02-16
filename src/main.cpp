#include <iostream>

#include "include/audit.hpp"
#include "include/audit.hpp"

int main(int argc, char* argv[]){
    // Check if there's enough arguments otherwise throw error.
    if (argc < 2 ) {
        std::cout << "Helium -- Expected at least 1 argument!";
        return 1;
    }

    // Load graph from file.
    database_t retrievedGraph = loadGraph();
    // Using argv 1 as a string for further string functions to later check with if statements what does the user want.
    std::string argument1 = argv[1];

    // Add either asset or anchor to make it more clearer for the user and then saving.
    if (argument1 == "add-asset"){ addNode(retrievedGraph, argc, argv, NodeType::Asset); saveGraph(retrievedGraph);} 
    else if (argument1 == "add-anchor"){ addNode(retrievedGraph, argc, argv, NodeType::Anchor); saveGraph(retrievedGraph);} 

    // Print all nodes.
    else if(argument1 == "list"){ printNodes(retrievedGraph); }
    // Link a node to another node.
    else if (argument1 == "link") {linkNodes(retrievedGraph, argc, argv); saveGraph(retrievedGraph);}

    else if (argument1 == "simulate") {simulateNodeFailure(argv, retrievedGraph);}

    else if (argument1 == "spof") {spofReport(retrievedGraph);}

    else if (argument1 == "audit") {auditReport();}

    else {std::cout << "Helium -- Command not found!" << std::endl;}
    return 0;
}
