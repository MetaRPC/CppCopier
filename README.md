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

This List example is read-only: no account creation or trades. Java on Windows uses `;` instead of `:` in classpaths.

## Authentication

Every SDK call sends `ApiKey` (`apikey` in gRPC metadata). The gRPC body also carries `user_key`; the header does not replace it. `TRIAL` is not an owner key. Read the [verified transport contract](https://metarpc.github.io/CppCopier/authentication/).

## Scope and verification

Core operations: Start · List · Pause · Remove. Advanced server features are distinguished from the supported wrapper surface. Review legacy replication examples before using them with disposable demos. Standard tests need no broker credentials.

See [testing and contributing](https://metarpc.github.io/CppCopier/testing/) for the regression suite, demo checklist and Pages build. Edit `docs/`; generated `site/` output is not source.
