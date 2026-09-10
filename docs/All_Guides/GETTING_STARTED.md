# Getting Started with CppCopier

Welcome to **CppCopier**, the official C++ client library for MetaRPC's ultra-low latency Trade Copier engine.

## Installation

Install CppCopier using your language's package manager:

```bash
cmake: find_package(CppCopier REQUIRED)
```

## Infrastructure Endpoints

MetaRPC provides dedicated production and staging environments for the Trade Copier:

| Environment | gRPC Endpoint | REST Base URL | WebSockets Base URL | Purpose |
|:---|:---|:---|:---|:---|
| **Production** | `copy.mrpc.pro:443` | `https://copy.mrpc.pro` | `wss://copy.mrpc.pro` | Live and funded accounts |
| **Staging** | `copy-stg.mrpc.pro:443` | `https://copy-stg.mrpc.pro` | `wss://copy-stg.mrpc.pro` | Integration testing & demo accounts |

## Authentication & Credentials

To authenticate your API calls, obtain your API keys from the [MetaRPC Portal](https://mrpc.pro/portal):

1. **User Key (`user_key`)**: Identifies your MetaRPC account and subscription tier.
2. **Manager Key (`manager_key`)**: Identifies the copier manager role. In self-service setups, `manager_key` can be the same as your `user_key`.

## Why gRPC?

Traditional trade copiers expose trading account passwords in REST query strings (e.g. `GET /start?password=xyz`), leaving passwords exposed in ingress proxies, cloud logs, and browser caches.

MetaRPC Trade Copier solves this by utilizing **HTTP/2 gRPC request bodies**:
- Passwords are encrypted in transit over TLS and serialized into binary protobuf messages.
- No intermediary proxy or logging layer captures trading credentials.
- Multiplexed streams deliver sub-millisecond execution speeds.
