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

## Plan an MT4 or MT5 copier integration

Use these free resources before connecting disposable demo accounts:

- [Choose your setup and review service costs](https://mrpc.pro/start/copier?lang=en&utm_source=github&utm_medium=referral&utm_campaign=copier_sdk_setup_nov17&utm_content=cppcopier_docs_setup) — compare the control-panel workflow with SDK integration. Hosted terminals and running copiers are billed separately, including on broker demo accounts.
- [Check follower lot sizing](https://mrpc.pro/tools/copier-sizing?lang=en&utm_source=github&utm_medium=referral&utm_campaign=copier_sdk_setup_nov17&utm_content=cppcopier_docs_sizing) — preview requested volume and broker minimum/maximum/step conflicts. This is a worksheet, not a broker quote or a guarantee of equal financial risk.
- [Review symbol mapping](https://mrpc.pro/tools/symbol-mapping?lang=en&utm_source=github&utm_medium=referral&utm_campaign=copier_sdk_setup_nov17&utm_content=cppcopier_docs_symbols) — compare symbol names, then verify the actual contract specifications with each broker before copying.
- [Work through the migration checklist](https://mrpc.pro/guides/copier-migration?lang=en&utm_source=github&utm_medium=referral&utm_campaign=copier_sdk_setup_nov17&utm_content=cppcopier_docs_migration) — validate sizing, one demo opening and closing, pause/resume and rejection logs before moving an integration.

These tools do not place trades or connect your accounts. MetaRPC provides developer APIs and hosted software; we do not manage or invest customer funds. Use this SDK's quickstart and recipes for the actual API calls.

## Watch a real demo failure and recovery

[Copier troubleshooting video](https://mrpc.pro/tutorials/console/copier-troubleshooting) follows an oversized follower request from its broker rejection through a corrected demo opening and closing. It demonstrates the control-panel workflow; the [lifecycle recipes](recipes.md), [sizing guide](sizing.md) and [error guide](troubleshooting.md) explain how to apply the checks in this SDK.

Use disposable demo accounts first. Hosted service usage is paid; consult [current pricing](https://mrpc.pro/#pricing). A successful management response does not prove that a broker executed the copied trade.
