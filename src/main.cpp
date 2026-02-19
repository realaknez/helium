#include <iostream>
#include <cctype>
#include <algorithm>

#include "../include/audit.hpp"
#include "../include/graph.hpp"

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
    
    // Make argument 1 lowercase for better UX
    std::transform(argument1.begin(), argument1.end(), argument1.begin(), [](unsigned char c){ return std::tolower(c); });

    // Add either asset or anchor to make it more clearer for the user and then saving.
    if (argument1 == "add-asset"){ addNode(retrievedGraph, argc, argv, NodeType::Asset); saveGraph(retrievedGraph);} 
    else if (argument1 == "add-anchor"){ addNode(retrievedGraph, argc, argv, NodeType::Anchor); saveGraph(retrievedGraph);} 

    // Print all nodes.
    else if(argument1 == "list"){ printNodes(retrievedGraph); }
    // Link a node to another node.
    else if (argument1 == "link") {linkNodes(retrievedGraph, argc, argv); saveGraph(retrievedGraph);}

    // Simulate what happens when a node fails, which returns all dependendencies that fall
    else if (argument1 == "simulate") {simulateNodeFailure(argv, retrievedGraph);}

    // Return every anchor with its dependent nodes and percentage
    else if (argument1 == "spof") {spofReport(retrievedGraph);}

    // Return amount of assets with a single dependency and anchors without any backup
    else if (argument1 == "audit") {auditReport(retrievedGraph);}

    // Export to Markdown file as a report
    else if (argument1 == "export") {exportMarkdownReport(retrievedGraph);}

    // Return err
    else {std::cout << "Helium -- Command not found!" << std::endl;}
    return 0;
}
