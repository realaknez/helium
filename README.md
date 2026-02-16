# Helium

Helium is a local digital continuity audit tool.

It models dependencies between digital assets and recovery anchors,
simulates cascading failures, and identifies structural weaknesses.

## Features

- Add anchors and assets
- Link dependencies
- Prevent circular dependencies
- Failure simulation
- Single Point of Failure (SPOF) report
- Structural audit tools

## Build

Requires C++17.

Compile:

g++ src/main.cpp src/graph.cpp src/audit.cpp -Iinclude -o helium

## Example Usage

Add anchor:
helium add-anchor primary_email

Add asset:
helium add-asset domain

Link:
helium link domain primary_email

Simulate:
helium simulate primary_email

Run SPOF report:
helium spof
