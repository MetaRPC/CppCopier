# First request with C++

List your own copiers to check connectivity and identity without creating a terminal or placing a trade.

## 1. Install

Requires **Windows with HTTP/2 support, Visual Studio C++ Build Tools and CMake 3.15+**. These instructions build from source; they do not assume a registry release exists.

```bash
git clone https://github.com/MetaRPC/CppCopier.git
cd CppCopier
cmake -S . -B build
cmake --build build --config Release
```

Java on Windows uses `;` for classpath separators. Run C++ builds in a Visual Studio developer shell. Activate your Python virtual environment before installing.

## 2. Set your owner key

=== "Bash"
    ```bash
    export MRPC_API_KEY='your-owner-key'
    ```
=== "PowerShell"
    ```powershell
    $env:MRPC_API_KEY = 'your-owner-key'
    ```

Use the real owner API key, not a broker password, terminal ID or `TRIAL`. Read [authentication](authentication.md) if access is refused.

## 3. Run

```bash
.\build\Release\list_copiers.exe
```

Source: [`examples/list.cpp`](https://github.com/MetaRPC/CppCopier/blob/main/examples/list.cpp).

```cpp
#include <copier/copier.hpp>
#include <cstdlib>
#include <iostream>
int main() {
    const char* key = std::getenv("MRPC_API_KEY");
    if (!key || !*key) { std::cerr << "Set MRPC_API_KEY\n"; return 1; }
    copier::CopierService service("copy.mrpc.pro:443", key);
    auto result = service.List();
    if (!result.ok) { std::cerr << result.error << "\n"; return 1; }
    for (const auto& item : result.copiers) std::cout << item.id << ": paused=" << item.paused << "\n";
}
```

A successful empty list means this owner has no copiers. Treat failed requests as errors, not empty accounts.

## 4. Prepare a demo workflow

Read [the lifecycle guide](lifecycle.md), then [choose sizing](sizing.md). Existing replication examples under `examples/` are advanced integration examples: they may provision accounts and trade. Inspect them and their cleanup before running with disposable demos. List is the default starting point.
