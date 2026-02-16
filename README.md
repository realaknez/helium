# Helium

Helium is a local digital continuity audit tool.

It models dependencies between digital assets and recovery anchors,
and simulates cascading failures.

## Features

- Add assets and anchors
- Link dependencies
- Prevent circular dependencies
- Simulate failure propagation
- Identify single points of failure (coming soon)

## Build

- Requires C++17.

- Compile with:
  - g++ main.cpp -o helium

## Usage Example

- ./helium add-anchor primary_email
- ./helium add-asset domain
- ./helium link domain primary_email
- ./helium simulate primary_email
