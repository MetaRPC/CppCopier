# C++ API reference

## Supported wrapper surface

**Start · List · Pause · Remove**. This focused core client does not expose every REST route, advanced option or operator billing method.

| Operation | Inputs | Successful result | Handling |
|---|---|---|---|
| Start | Owner/manager, master, follower, sizing and switches | `ok=true` and copier ID | Reconcile after timeout before retrying |
| List | Owner key | Summaries with paused state/reason | Check success; Ruby raises on failure |
| Pause/resume | Copier ID and paused bool | `ok=true` | Re-read state; billing can block resume |
| Remove | Copier ID | `ok=true` | Verify removal; positions are separate |

C# uses `Ok`, `Error`, `CopierId`; Python/Rust use `ok`, `error`, `copier_id`; Node/Java use `ok`, `error`, `copierId`; Go exports capitalized fields. Ruby Start/Pause/Remove return objects with `ok`/`error`, while List returns an array or raises.

## Start request

| Protocol field | Type | Meaning |
|---|---|---|
| `user_key` | string | Owner; defaults to configured identity |
| `manager_key` | string | Authorized creating manager; normally owner |
| `master`, `slave` | Account | Source and follower details |
| `risk_type` | string | One of five [sizing policies](sizing.md) |
| `risk_value` | decimal string | Lots or multiplier |
| `fixed_master_balance` | decimal string | Positive denominator for FixedBalanceMultiplier |
| `copy_sl`, `copy_tp` | bool | Copy protective levels |
| `copy_pending_orders` | bool | Include pending orders |
| `reverse_copy` | bool | Reverse direction |

Set booleans explicitly. Raw protobuf bools default to false; convenience defaults vary by wrapper.

## Account

| Field | Type | Meaning |
|---|---|---|
| `type` | string | `MT4` or `MT5` for the core model; use the deployment's supported account route |
| `user` | uint64 | Broker login, not API key |
| `password` | string | Broker password; secret |
| `server` | string | Exact broker server |
| `name` | string | Optional display label |
| `id` | string | Optional connected terminal/session ID |

Preserve 64-bit identifier precision. Node's underlying proto loader represents 64-bit response values as strings; do not round identifiers through floating-point conversions.

## Transport and version boundaries

Customer RPCs use `/copier.CopierService/Start`, `/List`, `/Pause`, `/Remove`. Default TLS target: `copy.mrpc.pro:443`. cURL examples use REST. C++ uses Windows WinHTTP, Java uses grpc-java; other SDKs use their language's gRPC library.

The checked-in [core proto](https://github.com/MetaRPC/CppCopier/blob/main/proto/copier.proto) defines this wrapper's fields. Additional server capabilities, including Update and operator billing calls, are not claimed as wrapper features. See [authentication](authentication.md) for separately verified server behavior.

## Errors and resources

Handle both configuration/transport exceptions and replies with `ok=false`. Close transports after use. Disposing a client does not stop a server-side copier. See [logs](logs.md) and [recovery](troubleshooting.md).
