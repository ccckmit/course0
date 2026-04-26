#!/usr/bin/env bash
# detect_target.sh – Detect system LLVM target triple

detect_target() {
    local os arch
    
    os=$(uname -s)
    arch=$(uname -m)
    
    case "$os" in
        Darwin)
            case "$arch" in
                x86_64)  echo "x86_64-apple-darwin" ;;
                arm64|aarch64) echo "arm64-apple-darwin" ;;
                *)       echo "arm64-apple-darwin" ;;
            esac
            ;;
        Linux)
            case "$arch" in
                x86_64)  echo "x86_64-pc-linux-gnu" ;;
                aarch64|arm64) echo "aarch64-pc-linux-gnu" ;;
                i686|i386) echo "i686-pc-linux-gnu" ;;
                *)       echo "x86_64-pc-linux-gnu" ;;
            esac
            ;;
        MINGW*|MSYS*)
            case "$arch" in
                x86_64)  echo "x86_64-pc-windows-gnu" ;;
                i686|i386) echo "i686-pc-windows-gnu" ;;
                *)       echo "x86_64-pc-windows-gnu" ;;
            esac
            ;;
        *)
            echo "x86_64-pc-linux-gnu"
            ;;
    esac
}

if [ "${BASH_SOURCE[0]}" == "${0}" ]; then
    detect_target
fi
