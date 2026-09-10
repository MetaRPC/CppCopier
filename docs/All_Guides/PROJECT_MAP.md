# Project Map & Architecture

This document provides a comprehensive architectural map of **CppCopier** and the MetaRPC Trade Copier engine.

---

## 1. Copier Architecture Diagram

```mermaid
flowchart TD
    subgraph ClientLayer ["Client Application Layer (C++)"]
        App["Your Application / Trading Bot"]
        Sugar["CopierSugar (Fluent API)"]
        Svc["CopierService (Wrapper API)"]
        Acc["CopierAccount (gRPC Protocol Layer)"]
        App --> Sugar
        Sugar --> Svc
        Svc --> Acc
    end

    subgraph Gateway ["MetaRPC Edge Infrastructure"]
        Ingress["gRPC Gateway (copy.mrpc.pro:443)"]
        WSHub["WebSocket Event Hub (/OnTradeLog)"]
    end

    subgraph CoreEngine ["MetaRPC Trade Copier Core"]
        Engine["Replication Engine"]
        RiskEngine["Risk Sizing & Multipliers"]
        SymbolMap["Symbol Mapping Engine"]
        SlippageGuard["Slippage & Deviation Guard"]
        Filter["Magic Number & Order Filter"]
    end

    subgraph Terminals ["Trading Accounts Layer"]
        MasterTerm["Master Account (MT4/MT5)"]
        SlaveTerm["Slave Account (MT4/MT5)"]
    end

    Acc -->|HTTP/2 gRPC| Ingress
    Ingress --> Engine
    MasterTerm -->|Real-time Ticks/Orders| Engine
    Engine --> Filter
    Filter --> SymbolMap
    SymbolMap --> RiskEngine
    RiskEngine --> SlippageGuard
    SlippageGuard -->|Replicated Execution| SlaveTerm
    Engine -->|Emit Execution Log| WSHub
    WSHub -->|Real-time Frames| App
```

---

## 2. Order Replication Pipeline

1. **Trade Capture**:
   - The master terminal sends order execution notifications into the MetaRPC core pipeline with zero polling overhead.
2. **Filter & Normalization**:
   - Checks if order matches magic numbers, comment filters, and symbols.
   - Applies broker symbol mapping (e.g. `XAUUSD` <-> `GOLD`).
3. **Risk & Sizing Engine**:
   - Computes target lot size using selected model (`FixedLot`, `LotMultiplier`, `BalanceMultiplier`, `FixedBalanceMultiplier`, `EquityMultiplier`).
   - Rounds to broker minimum/maximum lot steps.
4. **Execution & Slippage Protection**:
   - Verifies market price against master fill price. If slippage exceeds tolerance, execution is aborted to protect capital.
5. **Real-time Logging & Callbacks**:
   - Emits a JSON `TradeLog` frame on `/OnTradeLog?id={copierId}` and dispatches to configured Webhook URLs.
