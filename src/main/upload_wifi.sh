#!/bin/bash

PORT="172.20.10.12"   # or "smart-irrigation.local"
FQBN="esp32:esp32:esp32doit-devkit-v1"
SKETCH_DIR="$(pwd)"
BUILD_DIR="/tmp/build"

if ! command -v arduino-cli &> /dev/null; then
  echo "arduino-cli non installato."
  exit 1
fi

echo "Compilazione sketch..."
arduino-cli compile --fqbn "$FQBN" --output-dir "$BUILD_DIR" "$SKETCH_DIR" || { 
  echo "Compile fallito!"; 
  exit 3; 
}

echo "Upload OTA a $PORT..."
arduino-cli upload \
  -p "$PORT" \
  --fqbn "$FQBN" \
  --input-dir "$BUILD_DIR" || { 
    echo "Upload fallito!"; 
    exit 5; 
}

echo "Upload completato!"