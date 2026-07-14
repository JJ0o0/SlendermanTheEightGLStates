#!/bin/bash

set -e

TARGET="$1"

if [ -z "$TARGET" ]; then
    echo "Uso: ./dist.sh [linux|windows]"
    exit 1
fi

DIST_DIR="dist/$TARGET"

rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

case "$TARGET" in
    linux)
        cp build/SlendermanTEGS "$DIST_DIR/"
        ;;
    windows)
        cp build-windows/SlendermanTEGS.exe "$DIST_DIR/"
        ;;
    *)
        echo "Plataforma inválida."
        exit 1
        ;;
esac

cp -r assets "$DIST_DIR/"

echo "Distribuição criada em $DIST_DIR"