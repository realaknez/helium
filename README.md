# Helium

Helium is a local digital continuity audit tool.

It models dependencies between digital assets and recovery anchors,
simulates cascading failures, and identifies structural weaknesses.

## Features

- Add anchors and assets
- Link dependencies
- Prevent circular dependencies
- Cascading failure simulation
- Single Point of Failure (SPOF) with % Fragility Score
- Zero Redundancy detection
- Anchor backup audit
- Markdown report export

## Build

Requires C++17.

Compile:

g++ src/main.cpp src/graph.cpp src/audit.cpp -Iinclude -o helium

## Usage

Add an anchor:
helium add-anchor primary_email

Add an asset:
helium add-asset domain

Link asset to anchor:
helium link domain primary_email

Run structural audit:
helium audit

Run SPOF report:
helium spof

Export Markdown report:
helium export
