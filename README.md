# CppCopier

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Docs](https://img.shields.io/badge/docs-online-green.svg)](https://github.com/MetaRPC/CppCopier/tree/main/docs)

Official C++ SDK for the MetaRPC Trade Copier high-performance trade replication engine via gRPC (`copy.mrpc.pro:443`).

## Installation

```bash
cmake: find_package(CppCopier REQUIRED)
```

---

## 🏃 How to Run Examples

### 1. Clone & Build
```bash
git clone https://github.com/MetaRPC/CppCopier.git
cd CppCopier
cmake -B build -S .
cmake --build build --config Release
```

### 2. Run with Default TRIAL Key
```bash
# On Linux / macOS:
./build/quickstart

# On Windows:
.\build\Release\quickstart.exe

# Or using runner script:
.\run.bat
```

### 3. Run with Your Own API Key

Pass your API key directly as an argument:
```bash
# Windows
.\run.bat <YOUR_API_KEY>
# or
.\build\Release\quickstart.exe <YOUR_API_KEY>

# Linux / macOS
./build/quickstart <YOUR_API_KEY>
```

Or set the `MRPC_API_KEY` environment variable:
```bash
# Linux / macOS
export MRPC_API_KEY="<YOUR_API_KEY>"
./build/quickstart

# Windows PowerShell
$env:MRPC_API_KEY="<YOUR_API_KEY>"
.\run.bat

# Windows CMD
set MRPC_API_KEY=<YOUR_API_KEY>
run.bat
```

---

## Quick Start

See [Quick Start Documentation](https://github.com/MetaRPC/CppCopier/blob/main/docs/All_Guides/Your_First_Project.md) for a 10-minute walkthrough.

