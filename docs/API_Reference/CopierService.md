# CopierService API Reference

`CopierService` is the high-level client wrapper providing structured asynchronous methods for all Trade Copier operations.

---

## Methods

### `StartAsync(StartRequest request)`
Initiates trade copying between master and slave accounts.
- **Parameters**: `StartRequest` with credentials, risk model, and copy preferences.
- **Returns**: `StartReply` containing `ok` flag, `copier_id` (UUID), and error string if rejected.

### `ListAsync()`
Retrieves all copiers registered under the current user key.
- **Returns**: `ListReply` containing a list of `CopierSummary` objects. Passwords are never returned.

### `PauseAsync(string copierId, bool paused)`
Temporarily suspends or resumes trade copying for an active copier without deleting account configurations.
- **Parameters**: `copierId`, `paused` (true to pause, false to resume).
- **Returns**: `SimpleReply` (`ok`, `error`).

### `RemoveAsync(string copierId)`
Permanently deletes a copier, cleans up broker server hooks, and halts all trade replication.
- **Parameters**: `copierId`.
- **Returns**: `SimpleReply` (`ok`, `error`).

### `StreamTradeLogs(string copierId, Action<TradeLog> onMessage)`
Opens a persistent WebSocket stream to `wss://copy.mrpc.pro/OnTradeLog?id={copierId}` and invokes `onMessage` for every trade copied, closed, or modified.
