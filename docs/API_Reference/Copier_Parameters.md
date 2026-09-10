# Complete Copier Parameters Reference

Exhaustive documentation for every parameter accepted by `CopierService.Start` and `tradecopy`.

---

## Input Parameters Table

| Field Name | Protobuf Field | Data Type | Required | Default | Description |
|:---|:---|:---|:---|:---|:---|
| `user_key` | `string` (1) | String | **Yes** | — | MetaRPC customer identifier key. Used for billing, rate limiting, and permission scoping. |
| `manager_key` | `string` (2) | String | **Yes** | — | Manager key with administrative rights to provision and control the copier. In self-service mode, matches `user_key`. |
| `master.type` | `string` (1) | String | **Yes** | — | Protocol version for the master terminal: `"MT4"` or `"MT5"`. |
| `master.user` | `uint64` (2) | UInt64 | **Yes** | — | Master trading account login ID / account number. |
| `master.password` | `string` (3) | String | **Yes** | — | Master account trading password (investor or master password). |
| `master.server` | `string` (4) | String | **Yes** | — | Master broker cluster/server name (e.g. `"ICMarketsSC-Demo"`, `"MetaQuotes-Demo"`). |
| `master.name` | `string` (5) | String | No | `""` | Optional friendly display name for logging and UI displays. |
| `slave.type` | `string` (1) | String | **Yes** | — | Protocol version for the slave terminal: `"MT4"` or `"MT5"`. |
| `slave.user` | `uint64` (2) | UInt64 | **Yes** | — | Slave trading account login ID / account number. |
| `slave.password` | `string` (3) | String | **Yes** | — | Slave account trading password (MUST have trading permissions). |
| `slave.server` | `string` (4) | String | **Yes** | — | Slave broker cluster/server name. |
| `slave.name` | `string` (5) | String | No | `""` | Optional friendly display name for the slave account. |
| `risk_type` | `string` (5) | Enum String | **Yes** | `"LotMultiplier"` | Sizing model: `FixedLot`, `LotMultiplier`, `BalanceMultiplier`, `FixedBalanceMultiplier`, `EquityMultiplier`. |
| `risk_value` | `string` (6) | Decimal String | **Yes** | `"1.0"` | Lot size or multiplier value formatted as string to preserve exact financial decimal precision (e.g. `"0.05"`, `"1.5"`). |
| `fixed_master_balance` | `string` (7) | Decimal String | Conditional | `""` | Required if `risk_type == "FixedBalanceMultiplier"`. Specifies reference balance in account currency. |
| `copy_sl` | `bool` (8) | Boolean | No | `true` | If true, updates slave order Stop Loss whenever master modifies Stop Loss. |
| `copy_tp` | `bool` (9) | Boolean | No | `true` | If true, updates slave order Take Profit whenever master modifies Take Profit. |
| `copy_pending_orders`| `bool` (10) | Boolean | No | `false` | If true, pending orders (`BUY_LIMIT`, `SELL_STOP`, etc.) are placed on slave immediately upon master placement. |
| `reverse_copy` | `bool` (11) | Boolean | No | `false` | Inverts trade direction (`BUY` -> `SELL`). SL/TP distances are inverted accordingly. |

---

## Advanced REST & Engine Parameters

| Parameter | Type | Endpoint | Description |
|:---|:---|:---|:---|
| `max_slippage` | Integer | `/UpdateCopier` | Maximum points of market slippage before order is dropped. |
| `symbol_mapping` | Key-Value | `/AddSymbolMapping` | Explicit translation between master instrument name and slave instrument name. |
| `lot_min` | Double | `/UpdateCopier` | Hard lower clamp for slave order volume (default: broker minimum, usually 0.01). |
| `lot_max` | Double | `/UpdateCopier` | Hard upper clamp for slave order volume (e.g. 50.0). |
