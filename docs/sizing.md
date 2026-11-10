# Position sizing and copy settings

Pass decimal sizing values as strings (`"0.01"`, `"1.5"`) wherever the model accepts them. Avoid introducing floating-point rounding into the request.

## Sizing policies

| `risk_type` | Base calculation before broker constraints |
|---|---|
| `FixedLot` | Follower lots = `risk_value` |
| `LotMultiplier` | Master lots × `risk_value`; contract-size alignment applies when enabled |
| `BalanceMultiplier` | Master lots × follower balance / master balance × `risk_value` × contract-size ratio |
| `EquityMultiplier` | Master lots × follower equity / master equity × `risk_value` × contract-size ratio |
| `FixedBalanceMultiplier` | Master lots × follower balance / `fixed_master_balance` × `risk_value` × contract-size ratio |

`fixed_master_balance` must be positive for FixedBalanceMultiplier. Contract-size ratio means master contract size / follower contract size. Final lots may be refined, rounded, limited or rejected by broker limits and copier settings.

A 0.10-lot source with LotMultiplier `"0.5"` has a base 0.05 lots before contract alignment or broker constraints. FixedLot `"0.01"` requests a base 0.01 lots regardless of source size.

!!! warning "Different deposit currencies"
    The audited server compares raw balance/equity numbers; these ratios do not perform currency conversion. Do not describe them as equal monetary risk across currencies. Use matching deposit currencies for a simple verification and inspect actual lots.

## Copy switches

| Field | Meaning |
|---|---|
| `copy_sl` | Copy stop-loss changes, subject to broker constraints |
| `copy_tp` | Copy take-profit changes |
| `copy_pending_orders` | Include pending orders |
| `reverse_copy` | Reverse direction |

These switches do not guarantee broker acceptance. Symbols, digits, minimum stop distances, margin and sessions can affect execution. Compare the follower position with its log.

## Advanced settings and wrapper scope

The handwritten wrapper exposes Start, List, Pause and Remove. The current server also offers advanced settings and an Update RPC; this wrapper does not claim coverage of every service option. The bundled core proto is not an exhaustive copy of the latest server schema.

Review symbol mappings, limits, trading filters and equity protection in the control panel. Do not enable force-minimum-lot just to make a test pass: that can increase exposure.
