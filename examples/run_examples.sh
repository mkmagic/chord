#!/bin/bash
set -e

# Navigate to git root relative to this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$WORKSPACE_ROOT"

echo "=== Ensuring Output Directories Exist ==="
mkdir -p svg

echo "=== Checking Python Virtual Environment ==="
if [ ! -d ".venv" ]; then
    echo "Creating new virtual environment..."
    python3 -m venv .venv
fi

echo "=== Activating Virtual Environment ==="
source .venv/bin/activate

echo "=== Installing Dependencies ==="
if [ -f "build/_deps/kfr-src/requirements.txt" ]; then
    pip install -r build/_deps/kfr-src/requirements.txt
else
    echo "Missing requirements.txt. Make sure you have configured CMake (cmake -B build) first."
    exit 1
fi

echo "=== Running Plot Pulse Example ==="
# Export pythonpath so dspplot can be found
export PYTHONPATH=$WORKSPACE_ROOT/build/_deps/kfr-src/dspplot/dspplot:$PYTHONPATH

if [ -f "build/examples/plot_pulse" ]; then
    cd build && ./examples/plot_pulse
    echo "Done! The visualization SVGs are located in the /svg/ directory."
else
    echo "Executable build/examples/plot_pulse not found."
    echo "Please build the project first: cd build && make plot_pulse"
    exit 1
fi
