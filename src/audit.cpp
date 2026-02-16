#include "include/audit.hpp"
#include <iostream>
#include <algorithm>

void printNodes(const database_t& graph){
    // Loop through every pair in graph and print the string which is basically the name
    for (auto const &i : graph){    
        std::cout << i.first << std::endl; 
    }
}

void simulateNodeFailure(char* arguments[], const database_t& graph){
    std::string nodeName = arguments[2];

    if (graph.find(nodeName) == graph.end()){
        std::cout << "Couldn't find node!" << std::endl;
        return;
    } else {
        // Start by saving the nodeName in failed
        std::unordered_set<std::string> failed = simulate(nodeName, graph);

        // Print results 
        int failedAssets = 0;
        int failedAnchors = 0;
        
        std::cout << '\n';
        std::cout << "[Helium -- Simulation: " << nodeName << "]" << std::endl;
        std::cout << "Failed Anchors:" << std::endl;
        for (const auto &itr : failed){
            if (graph.at(itr).type == NodeType::Anchor){
                std::cout << "- " << itr << std::endl;
                failedAnchors++;
            }
        }
        

        std::cout << '\n';
        std::cout << "Failed Assets:" << std::endl;
        for (const auto &itr : failed){
            if (graph.at(itr).type == NodeType::Asset){
                std::cout << "- " << itr << std::endl;
                failedAssets++;
            }
        }
        std::cout << '\n';
        std::cout << "Total failed anchors: " << failedAnchors << std::endl;
        std::cout << "Total failed assets: " << failedAssets << std::endl;
        int totalFailed = failedAnchors + failedAssets;
        std::cout << "Total failed:" << totalFailed << std::endl;
    } 
}

void spofReport(const database_t& graph){
    std::vector<std::pair<std::string, int>> riskRanking;

    for (const auto& itr : graph) {
        if (itr.second.type == NodeType::Anchor){
            std::unordered_set<std::string> failedSet = simulate(itr.second.id, graph);

            int assetsFailed = 0;
            for (const auto& i : failedSet){
                if (graph.at(i).type == NodeType::Asset){
                    assetsFailed++;
                }
            }
            
            if (assetsFailed > 0)
                riskRanking.emplace_back(itr.second.id, assetsFailed);
        }
    }

    std::sort(riskRanking.begin(), riskRanking.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::cout << "Helium SPOF report:" << std::endl;
    for (const std::pair<std::string, int>& itr : riskRanking){
        std::cout << itr.first << ": " << itr.second << std::endl;
    }
    std::cout << '\n' << std::endl;
}

void auditReport(){

}