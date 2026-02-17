#include "../include/audit.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

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
    int totalAssets = 0;
    std::vector<RiskEntry> ranking;
    for (const auto& itr : graph){
        if (itr.second.type == NodeType::Asset) {
            totalAssets++;
        }
    }
    
    for (const auto& itr : graph) {
        if (itr.second.type == NodeType::Anchor){
            std::unordered_set<std::string> failedSet = simulate(itr.second.id, graph);

            int assetsFailed = 0;
            for (const auto& i : failedSet){
                if (graph.at(i).type == NodeType::Asset){
                    assetsFailed++;
                }
            }
            
            if (assetsFailed > 0) {
                RiskEntry entry;
                entry.anchor = itr.second.id;
                entry.assetsFailed = assetsFailed;
                ranking.emplace_back(entry);
            }
        } 
    }

    if (totalAssets == 0) {
        std::cout << "No assets in graph." << std::endl;
        return;
    }

    for (auto &i : ranking){
        i.percent = (i.assetsFailed / (double) totalAssets) * 100;
    }

    if (ranking.empty()) {
        std::cout << "No cascading failures detected." << std::endl;
        return;
    }

    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        return a.percent > b.percent;
    });

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Helium SPOF report:" << std::endl;
    for (const auto& itr : ranking){
        std::cout << itr.anchor << ": " << itr.assetsFailed << " assets " << "(" << itr.percent << "%)"<< std::endl;
    }
    std::cout << '\n' << std::endl;
}

void auditReport(const database_t& graph){
    std::vector<std::string> fragileAssets;
    std::vector<std::string> vulnerableAnchors;

    for (const auto &itr : graph){
        if (itr.second.type == NodeType::Asset && itr.second.dependencies.size() == 1){
            fragileAssets.push_back(itr.second.id);
        }

        if (itr.second.type == NodeType::Anchor && itr.second.dependencies.size() == 0){
            vulnerableAnchors.push_back(itr.second.id);
        }
    }

    std::cout << "[Helium -- Structural Audit]" << std::endl;
    std::cout << '\n';
    if (!fragileAssets.empty()){
        std::cout << "Assets with single dependency:" << std::endl;
        for (const auto &i : fragileAssets){
            std::cout << "- " << i << " which depends on " << graph.at(i).dependencies[0] << std::endl;
        }
        std::cout << '\n';
        std::cout << "Amount of fragile assets: " << fragileAssets.size() << std::endl;

    } else {
        std::cout << "No single-dependency assets found." << std::endl;
    } 

    std::cout << '\n';

    if (!vulnerableAnchors.empty()){
        std::cout << "Anchors with no backup:" << std::endl;
        for (const auto &i : vulnerableAnchors){
            std::cout << "- " << i << std::endl;
        }
        std::cout << '\n';
        std::cout << "Total vulnerable anchors: " << vulnerableAnchors.size() << std::endl;

    } else {
        std::cout << "All anchors have at least one backup." << std::endl;
    } 

    std::cout << '\n';

    std::string risk;

    if (vulnerableAnchors.size() > 0){
        risk = "HIGH";
    } else if (fragileAssets.size() > 0){
        risk = "MEDIUM";
    } else {
        risk = "LOW";
    }
    std::cout << "Overall Structural Risk Level: " << risk << std::endl;
    
}