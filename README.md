\# TARC GUI



<div align="center">



!\[TARC Logo](resources/icons/tarc.png)



\*\*Cross-platform GUI frontend for TARC - Hybrid Compression Engine\*\*



\[!\[Build Status](https://github.com/tuoaccount/tarc-gui/actions/workflows/build.yml/badge.svg)](https://github.com/tuoaccount/tarc-gui/actions)

\[!\[License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)



</div>



\## ✨ Features



\- 🖥️ \*\*Cross-platform\*\* - Windows, macOS, Linux

\- 📁 \*\*Drag \& Drop\*\* - Aggiungi file trascinandoli

\- 🔄 \*\*Real-time progress\*\* - Barra di avanzamento e velocità

\- 🎨 \*\*Modern UI\*\* - Tema scuro e interfaccia intuitiva

\- ⚡ \*\*Multi-threaded\*\* - Operazioni in background senza bloccare l'UI

\- 🎯 \*\*Auto codec selection\*\* - ZSTD, LZ4, LZMA automatici



\## 📥 Download



Pre-built binaries available in \[Releases](https://github.com/tuoaccount/tarc-gui/releases)



| Platform | Download |

|----------|----------|

| Windows | `tarc-gui-windows-x64.exe` |

| macOS | `tarc-gui-macos-universal.dmg` |

| Linux | `tarc-gui-linux-x86\_64.AppImage` |



\## 🛠️ Build from Source



\### Prerequisites



\- CMake 3.20+

\- C++17 compiler

\- Qt6 (6.5 or later)



\### Build Steps



```bash

\# Clone with submodules

git clone --recursive https://github.com/tuoaccount/tarc-gui.git

cd tarc-gui



\# Configure

mkdir build \&\& cd build

cmake .. -DCMAKE\_BUILD\_TYPE=Release



\# Build

cmake --build . --config Release --parallel 2

