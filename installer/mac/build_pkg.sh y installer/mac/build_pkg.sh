#!/bin/bash
# Crea el instalador .pkg de macOS para La Pauta Noise N1.
# Ejecutar desde cualquier carpeta despues de compilar:  installer/mac/build_pkg.sh
set -euo pipefail

cd "$(dirname "$0")/../.."

NAME="La Pauta Noise N1"
VERSION="1.0.0"
ART="build/LaPautaNoiseN1_artefacts/Release"
STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

mkdir -p dist "$STAGE/pkgs"

# --- VST3 ---
mkdir -p "$STAGE/vst3/Library/Audio/Plug-Ins/VST3"
cp -R "$ART/VST3/$NAME.vst3" "$STAGE/vst3/Library/Audio/Plug-Ins/VST3/"

# --- Audio Unit ---
mkdir -p "$STAGE/au/Library/Audio/Plug-Ins/Components"
cp -R "$ART/AU/$NAME.component" "$STAGE/au/Library/Audio/Plug-Ins/Components/"

# --- Standalone ---
mkdir -p "$STAGE/app/Applications"
cp -R "$ART/Standalone/$NAME.app" "$STAGE/app/Applications/"

pkgbuild --root "$STAGE/vst3" --identifier com.lapauta.noisen1.vst3 \
         --version "$VERSION" --install-location / "$STAGE/pkgs/vst3.pkg"
pkgbuild --root "$STAGE/au" --identifier com.lapauta.noisen1.au \
         --version "$VERSION" --install-location / "$STAGE/pkgs/au.pkg"
pkgbuild --root "$STAGE/app" --identifier com.lapauta.noisen1.app \
         --version "$VERSION" --install-location / "$STAGE/pkgs/app.pkg"

productbuild --distribution installer/mac/distribution.xml \
             --package-path "$STAGE/pkgs" \
             "dist/LaPautaNoiseN1-Setup-macOS.pkg"

echo "Instalador creado: dist/LaPautaNoiseN1-Setup-macOS.pkg"
