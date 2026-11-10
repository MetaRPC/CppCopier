---
hide:
  - toc
---
<div class="hero"><span class="eyebrow">METARPC / DEVELOPER GUIDES</span><h1>Trade copying.<br>Built with C++.</h1><p>From your first authenticated request to a monitored master–follower workflow. Clear examples, explicit sizing and practical recovery.</p><a class="md-button md-button--primary" href="quickstart/">Start building →</a> <a class="md-button" href="reference/">Explore the API</a><div class="hero-meta">TLS transport · Owner authentication · Demo-first validation</div></div>

<div class="grid cards" markdown>

- **01 · Connect with confidence**

    Understand keys, account IDs and transport before sending a request.

    [Authentication →](authentication.md)

- **02 · Control the lifecycle**

    Start once, inspect state, pause and clean up your test.

    [Copier lifecycle →](lifecycle.md)

- **03 · Make sizing explicit**

    Choose lot, balance or equity sizing and account for broker constraints.

    [Sizing and settings →](sizing.md)

- **04 · Diagnose the result**

    Separate network success from management success and broker execution.

    [Troubleshooting →](troubleshooting.md)

</div>

## Your first successful request

Requires Windows with HTTP/2 support, Visual Studio C++ Build Tools and CMake 3.15+. Install dependencies and run the read-only List example. Keep your key in `MRPC_API_KEY`.

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

[Full setup](quickstart.md) · [Testing guide](testing.md) · [All SDKs](sdks.md)
