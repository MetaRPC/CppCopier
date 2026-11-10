# Testing and contributing

## Three layers of evidence

1. **Offline tests:** authentication, request fields, response/error handling without broker credentials.
2. **Documentation checks:** strict builds and local links/assets/anchors.
3. **Explicit demo integration:** broker connection, replication, sizing and cleanup using disposable accounts.

Standard tests must not place trades or provision accounts. Legacy replication examples can change broker state: inspect their source and cleanup, supply a real key, and use disposable demos. Never use a production trading password.

## Demo acceptance checklist

- Record account IDs and servers without passwords.
- Verify Start, List, Pause, Resume and Remove for the same owner.
- Check that missing/invalid credentials do not expose another owner's records.
- Open and close one known test trade; verify both tickets and follower lots.
- Ensure failed requests do not become successful empty results.
- Verify log events originate from the server.
- Clean up owned test positions, copier and terminal sessions on success and failure.

## Build the docs

```bash
python -m pip install -r requirements-docs.txt
python -m mkdocs build --strict
python tools/check_docs.py
```

GitHub Actions validates changes and publishes Pages only from a passing build on `main`. Documentation builds require no API key. When changing a method, update its example, reference and regression test together. Do not claim broker integration passed on the strength of mocks.

## Run this SDK’s regression suite

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The November 10 audit passed the SDK build/tests and strict documentation/link checks in GitHub Actions. Offline tests cover transport, key validation and relevant regressions. The separate live acceptance results below verify broker replication.

## Live verification: November 10, 2026

The actual CppCopier client passed demo trading acceptance against `copyback-stg.mrpc.pro`, using a dedicated master/follower pair on **MT5 MetaQuotes-Demo**. The runner checked account identity, demo mode and empty positions before trading.

| Check | Result |
|---|---|
| Valid, missing and invalid owner key | Valid List succeeded; missing/invalid keys were refused |
| REST and WebSocket header checks | Valid key: HTTP 200/101; missing: 401/401; invalid: 403/403 |
| Invalid key attempting Pause | Refused; the owned copier remained usable |
| Open and close replication | Six EURUSD master/follower pairs opened and closed |
| LotMultiplier = 2 | 0.01 master lot copied as 0.02 follower lot, before and after resume |
| FixedLot = 0.03 | Follower opened 0.03 lot |
| BalanceMultiplier and EquityMultiplier = 2 | Follower volume matched the ratio calculated from current account values |
| FixedBalanceMultiplier = 1 | With fixed master balance set to half the follower balance, 0.01 copied as 0.02 lot |
| Pause | A new master trade produced no follower position during the six-second observation window |
| Resume | A subsequent sell trade copied and closed successfully |
| Trade logs | Successful streamed event matched the actual master and follower tickets |
| Cleanup | Seven master and six follower positions closed; all five test copiers removed and absence verified |

This SDK exposes copier management operations; the test verified events through the authenticated WebSocket endpoint. The test used the SDK for Start, List, Pause, Resume and Remove; broker orders and position checks used MRPC terminal REST. This is an MT5 demo result, not MT4 broker execution coverage or an extended load test. The auth-only checks and sanitized trade evidence are stored in `tests/live-auth-2026-11-10.json` and `tests/live-trading-2026-11-10.json`.

The run uncovered three backend faults: incorrectly escaped gRPC ingress routing, WebSocket events confined to one pod, and a pause handled by another pod leaving stale running state. Routing was corrected, log events now fan out through Redis, and follower sends check durable pause/removal state. The final matrix ran against backend commit `dcdaa53b` on all three staging pods. The terminal capacity blocker was resolved before this acceptance run. SDK demo helpers were also hardened so server errors cannot look like successful empty responses.

Terminal REST helpers now reject an MRPC `error` envelope even when the HTTP status is 200, rather than returning an empty terminal ID, zero order ticket or empty position list. Offline regression tests cover this behavior. The cURL repository has no terminal-helper library; its acceptance adapter reports unsuccessful responses as failures.

### Native acceptance adapter

The repository includes a small native adapter for an external acceptance runner. It invokes this SDK's actual client. It is not part of the normal test run and must only be used with disposable demo accounts for mutation checks.

Run the adapter with one of these argument lists: `list`, `start`, `pause COPIER_ID true`, `pause COPIER_ID false`, or `remove COPIER_ID`. The adapter consumes 13 newline-separated configuration fields on standard input, in this order:

1. Copier host and port, without a URL scheme (for example `copyback-stg.mrpc.pro:443`).
2. Owner API key.
3. Master login.
4. Master trading password.
5. Master broker server.
6. Master terminal GUID for the copier API.
7. Follower login.
8. Follower trading password.
9. Follower broker server.
10. Follower terminal GUID for the copier API.
11. Risk mode.
12. Risk value.
13. Fixed master balance, or a blank line when unused.

Keep the input in a private file outside the checkout, restrict its permissions and pass it through standard input. Do not put passwords in command arguments, source code, workflow files or logs. The output is a JSON result. A false `ok` is a failed check, even if the process exits successfully. Standard input must include the final blank field when the fixed balance is unused.

Build the SDK first, then invoke:

```text
.\build\Release\live_adapter.exe list
```

The adapter alone does not verify that an account is a demo or clean up trades. The acceptance runner must verify account identity and demo mode, start with empty positions, inspect actual follower tickets/lots and log events, and close only its own test trades before removing its copier. Do not run mutation commands against existing customer copiers.
