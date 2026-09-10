# Glossary: Trade Copier Concepts & Terminology

A comprehensive reference for all concepts, risk models, and parameters in MetaRPC Trade Copier.

---

## 1. Risk Models (`risk_type`)

MetaRPC Trade Copier features 5 institutional-grade risk models:

### `FixedLot`
- **Behavior**: Every trade copied to the slave uses the exact volume specified in `risk_value`, regardless of the master's trade volume.
- **Formula**: `SlaveLots = risk_value`
- **Example**: `risk_value = "0.10"`. Master opens 5.0 lots -> Slave opens 0.10 lots.

### `LotMultiplier`
- **Behavior**: The slave trades a fixed multiple of the master's volume.
- **Formula**: `SlaveLots = MasterLots * risk_value`
- **Example**: `risk_value = "2.0"`. Master opens 0.50 lots -> Slave opens 1.00 lots.

### `BalanceMultiplier`
- **Behavior**: The slave trades proportionally based on the ratio of slave balance to current master balance.
- **Formula**: `SlaveLots = MasterLots * (SlaveBalance / MasterBalance) * risk_value`
- **Example**: Master balance $10,000, Slave balance $2,000, `risk_value = "1.0"`. Ratio is 0.2. Master trades 1.0 lot -> Slave trades 0.20 lots.

### `FixedBalanceMultiplier`
- **Behavior**: Scales trades against a static, user-defined master baseline balance (`fixed_master_balance`). This prevents wild swings if the master deposits or withdraws capital.
- **Formula**: `SlaveLots = MasterLots * (SlaveBalance / fixed_master_balance) * risk_value`
- **Example**: `fixed_master_balance = "10000"`, `risk_value = "1.0"`. Slave has $5,000. Slave trades 0.50x of master.

### `EquityMultiplier`
- **Behavior**: Proportional scaling based on floating equity rather than balance, taking active drawdowns and open floating profits into account.
- **Formula**: `SlaveLots = MasterLots * (SlaveEquity / MasterEquity) * risk_value`

---

## 2. Parameter Definitions

| Parameter | Type | Description | Default |
|:---|:---|:---|:---|
| `copy_sl` | `bool` | Automatically replicates Stop Loss adjustments from master to slave. | `true` |
| `copy_tp` | `bool` | Automatically replicates Take Profit adjustments from master to slave. | `true` |
| `copy_pending_orders` | `bool` | Copies pending limit and stop orders (`BUY_LIMIT`, `SELL_LIMIT`, `BUY_STOP`, `SELL_STOP`). | `false` |
| `reverse_copy` | `bool` | Inverts trade directions: `BUY` becomes `SELL`, `SELL` becomes `BUY`. Automatically inverts SL and TP offsets. | `false` |
| `fixed_master_balance` | `string` | Baseline dollar balance used when `risk_type` is `FixedBalanceMultiplier`. | `""` |
| `slippage_guard` | `int32` | Maximum allowable price difference in points between master open price and slave current price. | `15` |
