#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
if ! command -v g++ >/dev/null 2>&1; then
  echo "Please install a C++17 compiler (g++)."
  exit 1
fi
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o reason_act_reflect
./reason_act_reflect
