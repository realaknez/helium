#include "../include/audit.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <unordered_set>
#include <ctime>

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

std::vector<RiskEntry> computeSPOF(const database_t& graph){
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

    if (totalAssets == 0) {return ranking;}

    for (auto &i : ranking){
        i.percent = (i.assetsFailed / (double) totalAssets) * 100;
    }

    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        return a.percent > b.percent;
    });

    return ranking;
}

void spofReport(const database_t& graph){
    std::vector<RiskEntry> ranking = computeSPOF(graph);

    if (ranking.empty()) {
        std::cout << "No cascading failures detected." << std::endl;
        return;
    }

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Helium SPOF report:" << std::endl;
    for (const auto& itr : ranking){
        std::cout << itr.anchor << ": " << itr.assetsFailed << " assets " << "(" << itr.percent << "%)"<< std::endl;
    }
    std::cout << '\n' << std::endl;
}

AuditData computeAudit(const database_t& graph){
    AuditData dataPackage;

    for (const auto &itr : graph){
        if (itr.second.type == NodeType::Asset && itr.second.dependencies.size() == 1){
            dataPackage.fragileAssets.push_back(itr.second.id);
        }

        if (itr.second.type == NodeType::Anchor && itr.second.dependencies.size() == 0){
            dataPackage.vulnerableAnchors.push_back(itr.second.id);
        }
    }

    return dataPackage;
}

void auditReport(const database_t& graph){
    AuditData auditData = computeAudit(graph);

    std::cout << "[Helium -- Structural Audit]" << std::endl;
    std::cout << '\n';
    if (!auditData.fragileAssets.empty()){
        std::cout << "Assets with single dependency:" << std::endl;
        for (const auto &i : auditData.fragileAssets){
            std::cout << "- " << i << " which depends on " << graph.at(i).dependencies[0] << std::endl;
        }
        std::cout << '\n';
        std::cout << "Amount of fragile assets: " << auditData.fragileAssets.size() << std::endl;

    } else {
        std::cout << "No single-dependency assets found." << std::endl;
    } 

    std::cout << '\n';

    if (!auditData.vulnerableAnchors.empty()){
        std::cout << "Anchors with no backup:" << std::endl;
        for (const auto &i : auditData.vulnerableAnchors){
            std::cout << "- " << i << std::endl;
        }
        std::cout << '\n';
        std::cout << "Total vulnerable anchors: " << auditData.vulnerableAnchors.size() << std::endl;

    } else {
        std::cout << "All anchors have at least one backup." << std::endl;
    } 

    std::cout << '\n';

    std::string risk;

    if (auditData.vulnerableAnchors.size() > 0){
        risk = "HIGH";
    } else if (auditData.fragileAssets.size() > 0){
        risk = "MEDIUM";
    } else {
        risk = "LOW";
    }
    std::cout << "Overall Structural Risk Level: " << risk << std::endl;
    
}

void exportMarkdownReport(const database_t& graph){
    std::ofstream EXPORT_FILE("helium_report.md");
    std::vector<RiskEntry> spof = computeSPOF(graph);
    AuditData audit = computeAudit(graph);
    std::time_t now = std::time(nullptr);

    if(!EXPORT_FILE.is_open()) {
        std::cout << ("Helium -- Report couldn't be exported!") << std::endl;
        return;
    }
    
    EXPORT_FILE << "_Generated by Helium v0.2.0_\n"; 
    EXPORT_FILE << std::ctime(&now) << "\n\n";
    EXPORT_FILE << "# Helium Resilience Report\n\n";
    EXPORT_FILE << "## Structural Audit\n\n";

    if (!audit.fragileAssets.empty()){
        EXPORT_FILE << "### Assets with Single Dependency\n\n";
        for (const auto &i : audit.fragileAssets){
            EXPORT_FILE << "- " << i << " which depends on " << graph.at(i).dependencies[0] << '\n';
        }
        EXPORT_FILE << "Amount of fragile assets: " << audit.fragileAssets.size();

    } else {
        EXPORT_FILE << "No single-dependency assets found.";
    } 

    EXPORT_FILE << '\n';

    if (!audit.vulnerableAnchors.empty()){
        EXPORT_FILE << "### Anchors with no backup:\n\n";
        for (const auto &i : audit.vulnerableAnchors){
            EXPORT_FILE << "- " << i << '\n';
        }
        EXPORT_FILE << '\n';
        EXPORT_FILE << "Total vulnerable anchors: " << audit.vulnerableAnchors.size();

    } else {
        EXPORT_FILE << "All anchors have at least one backup.";
    } 

    EXPORT_FILE << '\n\n';

    std::string risk;

    if (audit.vulnerableAnchors.size() > 0){
        risk = "**HIGH**";
    } else if (audit.fragileAssets.size() > 0){
        risk = "**MEDIUM**";
    } else {
        risk = "**LOW**";
    }
    EXPORT_FILE << "## Overall Structural Risk Level: \n\n";
    EXPORT_FILE << risk;

    EXPORT_FILE << "---\n\n";

    EXPORT_FILE << "### Single Point of Failure (SPOF)\n\n";
    if (spof.empty()) {
        EXPORT_FILE << "No cascading failures detected." << '\n';
    }

    EXPORT_FILE << std::fixed << std::setprecision(1);
    for (const auto& itr : spof){
        EXPORT_FILE << "- " << itr.anchor << ": " << itr.assetsFailed << " assets (" << itr.percent << "%)\n";
    }
    EXPORT_FILE << '\n';

    std::cout << "Helium -- Report exported to helium_report.md" << std::endl;
}