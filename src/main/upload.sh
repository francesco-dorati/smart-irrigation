#!/bin/bash

PORT="/dev/cu.usbserial-10"
FQBN="esp32:esp32:esp32doit-devkit-v1"
SKETCH_DIR="$(pwd)"
BUILD_DIR="/tmp/build"
BAUDRATE=115200

if ! command -v arduino-cli &> /dev/null; then
  echo "arduino-cli non installato."
  exit 1
fi

if ! command -v esptool.py &> /dev/null; then
  echo "esptool.py non installato. Installa con 'pip install esptool'"
  exit 2
fi

echo "Compilazione sketch..."
arduino-cli compile --fqbn "$FQBN" --output-dir "$BUILD_DIR" "$SKETCH_DIR" || { echo "Compile fallito!"; exit 3; }

BIN_PATH=$(find "$BUILD_DIR" -type f -name "*.merged.bin" -print0 | xargs -0 ls -1 -t | head -n 1)

if [ ! -f "$BIN_PATH" ]; then
  echo "File .merged.bin non trovato."
  exit 4
fi

echo "File .merged.bin trovato: $BIN_PATH"

echo "Flash con esptool.py a offset 0x0..."
echo "Se fallisce, tieni premuto BOOT e premi RESET."
esptool --chip esp32 --port "$PORT" --baud 115200 write-flash 0x0 "$BIN_PATH" || { echo "Flash fallito!"; exit 5; }

echo "Flash completato! Apro monitor seriale..."
arduino-cli monitor -p "$PORT" -c baudrate="$BAUDRATE"