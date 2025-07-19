#!/bin/bash

FQBN="arduino:avr:nano:cpu=atmega328old"
PORT="/dev/cu.usbserial-140"
BAUDRATE=9600
SKETCH_DIR="$(pwd)"

# check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
  echo "Error: arduino-cli is not installed or not in PATH."
  exit 1
fi

echo "Compiling sketch in $SKETCH_DIR..."
arduino-cli compile --fqbn "$FQBN" "$SKETCH_DIR" || { echo "Compile failed!"; exit 2; }

echo ""
echo "Uploading to $PORT..."
arduino-cli upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_DIR" || { echo "Upload failed!"; exit 3; }

echo ""
echo "Opening serial monitor at $BAUDRATE baud. Press Ctrl+C to exit."
arduino-cli monitor -p "$PORT" -c baudrate="$BAUDRATE"
