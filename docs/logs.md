# Trade logs and observability

Connect to `wss://copy.mrpc.pro/OnTradeLog?id=<copier-id>` with the owner's `ApiKey` upgrade header. Use the same deployment and owner as the copier. This is a WebSocket JSON stream, not a gRPC streaming RPC.

The real payload includes timing, success/failure and nested orders. Fields can include `masterOrder`, `slaveOrder`, `slaveSymbol`, `slaveTicket`, `exceptionCode` and `exceptionMessage`. Do not assume a flat event with top-level `symbol` and numeric tickets. Preserve raw JSON for diagnostics and redact it before sharing.

## Consume correctly

- Assemble fragmented messages before parsing.
- Bound message size and connection timeout.
- Surface authentication and parsing errors.
- Treat disconnect as the end of the subscription; reconnect with bounded backoff.
- Reconcile positions/history after gaps; silence does not prove no trades occurred.
- Close the socket when its workflow stops.

Browser WebSocket APIs cannot add an arbitrary ApiKey header. Subscribe through a trusted backend instead of exposing the owner key in browser code.

Python yields real received events and retains their `raw` payload. C# provides awaited `StreamTradeLogsAsync` with complete JSON messages. Node's WebSocket helper sends authenticated headers. Other core wrappers do not implement streaming; use an authenticated WebSocket library in your application.
