#!/bin/bash
# =============================================================
# setup.sh — Vérifie les dépendances et compile Santorini
# Usage : bash setup.sh
# =============================================================

echo ""
echo "  ╔══════════════════════════════════╗"
echo "  ║       Santorini — Setup          ║"
echo "  ╚══════════════════════════════════╝"
echo ""

# --- Vérification des dépendances (sans sudo) ---
echo "[1/2] Vérification des dépendances..."
echo ""

MISSING=()

check_bin() {
    if ! command -v "$1" &>/dev/null; then
        MISSING+=("$2")
        echo "  [MANQUANT] $2"
    else
        echo "  [OK]       $2"
    fi
}

check_header() {
    if [ ! -f "$1" ]; then
        MISSING+=("$2")
        echo "  [MANQUANT] $2"
    else
        echo "  [OK]       $2"
    fi
}

check_bin    "cmake"       "cmake"
check_bin    "g++"         "build-essential / g++"
check_header "/usr/include/GLFW/glfw3.h"   "libglfw3-dev"
check_header "/usr/include/GL/gl.h"        "libgl1-mesa-dev"
check_header "/usr/include/glm/glm.hpp"    "libglm-dev"

echo ""

if [ ${#MISSING[@]} -gt 0 ]; then
    echo "  ╔══════════════════════════════════════════════════════════════╗"
    echo "  ║  Des dépendances sont manquantes.                           ║"
    echo "  ║  Consultez le README.md pour les commandes d'installation.  ║"
    echo "  ╚══════════════════════════════════════════════════════════════╝"
    echo ""
    echo "  Paquets manquants :"
    for pkg in "${MISSING[@]}"; do
        echo "    - $pkg"
    done
    echo ""
    exit 1
fi

echo "  [OK] Toutes les dépendances sont présentes."
echo ""

# --- Compilation ---
echo "[2/2] Compilation..."

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "  [ERREUR] cmake a échoué."
    exit 1
fi

make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "  [ERREUR] La compilation a échoué."
    exit 1
fi

cd ..

echo ""
echo "  ╔══════════════════════════════════╗"
echo "  ║     Compilation réussie !        ║"
echo "  ╠══════════════════════════════════╣"
echo "  ║  cd build                        ║"
echo "  ║  ./santorini solo                ║"
echo "  ║  ./santorini server              ║"
echo "  ║  ./santorini client [IP] [PORT]  ║"
echo "  ╚══════════════════════════════════╝"
echo ""