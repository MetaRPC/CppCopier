# MetaRPC C++ Trade Copier SDK

Authenticate, configure a master–follower relationship, inspect its state, pause and remove it.

**[Documentation](https://metarpc.github.io/CppCopier/) · [Quickstart](https://metarpc.github.io/CppCopier/quickstart/) · [Authentication](https://metarpc.github.io/CppCopier/authentication/) · [API reference](https://metarpc.github.io/CppCopier/reference/)**

## Install from source

Requires Windows with HTTP/2 support, Visual Studio C++ Build Tools and CMake 3.15+.

```bash
git clone https://github.com/MetaRPC/CppCopier.git
cd CppCopier
cmake -S . -B build
cmake --build build --config Release
```

## First request

Set `MRPC_API_KEY` to your real owner API key in your environment, then run:

```bash
.\build\Release\list_copiers.exe
```

This List example is read-only: no account creation or trades. Run CMake from a Visual Studio developer shell. The transport uses Windows WinHTTP.

## Authentication

Every SDK call sends `ApiKey` (`apikey` in gRPC metadata). The gRPC body also carries `user_key`; the header does not replace it. `TRIAL` is not an owner key. Read the [verified transport contract](https://metarpc.github.io/CppCopier/authentication/).

## Demo workflow and troubleshooting

Start with the read-only example above, then use disposable demo accounts for a copying test. The [lifecycle recipes](https://metarpc.github.io/CppCopier/recipes/) show the SDK workflow; [position sizing](https://metarpc.github.io/CppCopier/sizing/) explains the settings, and [troubleshooting](https://metarpc.github.io/CppCopier/troubleshooting/) covers rejected trades and uncertain outcomes.

- [Watch the recorded copier troubleshooting walkthrough](https://mrpc.pro/tutorials/console/copier-troubleshooting?utm_source=github&utm_medium=referral&utm_campaign=sdk_guides_nov12&utm_content=cppcopier) — a real demo rejection, corrected volume, and verified opening/closing. This video uses the control panel; use this SDK's recipes for code.
- [Open the control panel](https://mrpc.pro/my) to inspect your account pair and trade results.
- [Pricing](https://mrpc.pro/#pricing) — hosted terminals and copying can incur service charges, including on broker demo accounts. Demo funds are not MetaRPC service credit.

## Scope and verification

cTrader Spotware demo ↔ MT5 and TradeLocker HEROFX demo ↔ MT5 acceptance passed in both directions on November 10, 2026: five sizing modes, pause/resume, live/persisted logs and API-key errors. See the [provider guide](https://metarpc.github.io/CppCopier/providers/) and [dated results](https://metarpc.github.io/CppCopier/testing/).

MT4 FreshForex-Demo and MT5 MetaQuotes-Demo acceptance passed on November 10, 2026: opens/closes, five sizing modes, pause/resume, live/persisted logs and API-key failures. See the [separate broker test evidence](https://metarpc.github.io/CppCopier/testing/).

Core operations: Start · List · Pause · Remove. Advanced server features are distinguished from the supported wrapper surface. Review legacy replication examples before using them with disposable demos. Standard tests need no broker credentials.

See [testing and contributing](https://metarpc.github.io/CppCopier/testing/) for the regression suite, demo checklist and Pages build. Edit `docs/`; generated `site/` output is not source.
