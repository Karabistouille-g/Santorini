#!/bin/bash
# =============================================================
# setup.sh — Installe les dependances et compile Santorini
# Usage : bash setup.sh
# =============================================================

set -e

echo ""
echo "  ╔══════════════════════════════════╗"
echo "  ║       Santorini — Setup          ║"
echo "  ╚══════════════════════════════════╝"
echo ""

# --- Installation des dependances ---
echo "[1/2] Installation des dependances..."

if command -v apt-get &>/dev/null; then
    sudo apt-get update -qq
    sudo apt-get install -y build-essential cmake libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev libglm-dev
elif command -v dnf &>/dev/null; then
    sudo dnf install -y gcc-c++ cmake glfw-devel mesa-libGL-devel glm-devel
elif command -v pacman &>/dev/null; then
    sudo pacman -Sy --noconfirm base-devel cmake glfw mesa glm
else
    echo "[ERREUR] Gestionnaire de paquets non reconnu."
    echo "         Installez manuellement : cmake, libglfw3-dev, libgl1-mesa-dev, libglm-dev"
    exit 1
fi

echo "[OK] Dependances installees."

# --- Compilation ---
echo ""
echo "[2/2] Compilation..."

rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "  ╔══════════════════════════════════╗"
echo "  ║     Compilation reussie !        ║"
echo "  ╠══════════════════════════════════╣"
echo "  ║  cd build                        ║"
echo "  ║  ./santorini solo                ║"
echo "  ║  ./santorini server              ║"
echo "  ║  ./santorini client [IP] [PORT]  ║"
echo "  ╚══════════════════════════════════╝"
echo ""