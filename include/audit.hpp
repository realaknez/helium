#pragma once
#include "include/graph.hpp"

// Basic functions to get all the nodes from the current loaded graph and add nodes to it
void printNodes(const database_t& graph);

void spofReport(const database_t& graph);
void auditReport();

// Check what fails when a node fails
void simulateNodeFailure(char* arguments[], const database_t& graph);

