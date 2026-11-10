# Testing and contributing

## cTrader and TradeLocker acceptance: November 10, 2026

**Passed with the actual CppCopier client in four directions:** HEROFX TradeLocker demo → MetaQuotes MT5 demo, MT5 → TradeLocker, Spotware cTrader demo → MT5, and MT5 → cTrader. These are executed broker trades, not just connection checks.

| Check | Result per direction |
|---|---|
| Open and close | Six EURUSD master/follower pairs opened and closed |
| LotMultiplier | 0.01 master lot copied as 0.02, including a sell after resume |
| FixedLot | Follower opened 0.03 lot |
| BalanceMultiplier / EquityMultiplier | Actual follower volume matched the formula using current account values |
| FixedBalanceMultiplier | Fixed master balance set to half the follower balance: 0.01 copied as 0.02 |
| Pause and resume | Separate master trade produced no follower during six seconds paused; new trade copied after resume |
| Logs | Live success event matched both exact tickets; persisted history matched the master |
| ApiKey errors | Empty/invalid owner keys refused; missing/invalid headers could not read the copier; invalid-key Pause refused |
| Cleanup | Seven master and six follower positions closed; five test copiers removed and absence checked |

This SDK completed **24 copied pairs**, four additional master-only pause checks and 20 removed copiers. Across all nine SDKs, the matrix completed **216 copied pairs**, closed 252 master and 216 follower positions, and removed 180 copiers. Final checks found all three demo accounts empty, with no working orders. Both test provider registrations were removed and all three test sessions disconnected.

### What the tests exercised

Copier lifecycle calls used this SDK. Broker mutations and position verification used provider/terminal REST. Python, Node.js and C# used their own streaming helpers; the other clients verified the authenticated WebSocket alongside their native copier calls. EURUSD metadata on both provider demos specified 100,000 units per lot and a 1,000-unit minimum/step. Other instruments may differ.

The cTrader demo uses EUR and has a much smaller balance than the MT5 demo. Balance/equity cases selected explicit multipliers from current account values to target 0.02 follower lot, then compared actual fills with the calculated volume. The copier currently compares raw numeric account values, **without currency conversion**. This run verifies that existing behavior; it does not establish equivalent monetary risk across currencies. The exact inputs and expected volumes are included in cTrader evidence.

### Issues found and corrected

- Website-owner registration no longer requires a duplicate local copier user row; ownership must still be verified.
- Provider-session Start accepts an empty MetaTrader password only for an owned, matching CT/TL registration. Mismatched accounts, sessions, servers and owners remain rejected.
- Node.js preserves unsafe JSON integers as strings, preventing TradeLocker ticket rounding in WebSocket logs. Its type definitions now include all supported routes and nested log orders.
- The runner distinguishes an in-flight market order from an unexpected pending order. A stopped exploratory run was reconciled, and the complete case rerun; final cleanup still requires no positions or orders.
- A C# reverse cTrader attempt stopped on an MT5 read timeout (HTTP 504) before sending its next order. Both accounts were confirmed flat, then the complete case reran. Staging Redis pods had just been recreated; this is a correlation, not a proven cause. The totals above cover successful acceptance runs; interrupted exploratory attempts are recorded separately in the evidence.
- A C++ reverse attempt also stopped on a direct WebSocket handshake timeout before its next trade. Cleanup and flat broker state were verified, then the entire case reran. The handshake timeout's root cause was not established; this matrix is not an availability or endurance certification.

- A cURL reverse attempt received a cTrader `MARKET_CLOSED` rejection. Its master position was closed and its copier removed. After maintenance, the full case passed; the rejected attempt is recorded separately and excluded from successful totals.

The backend regression suite passed **1,666 tests**, with 37 optional integration tests skipped. SDK offline suites and strict documentation checks are run separately from these broker tests.

### Evidence and limits

- [TradeLocker execution evidence](https://github.com/MetaRPC/CppCopier/blob/main/tests/live-tradelocker-trading-2026-11-10.json)
- [cTrader execution evidence](https://github.com/MetaRPC/CppCopier/blob/main/tests/live-ctrader-trading-2026-11-10.json)
- [Provider setup, identifiers and quantity conversion](providers.md)

Scope is EURUSD market positions on the named demos, with MT5 as the counterpart. This does not establish CT↔TL, same-provider CT/TL copying, every broker/instrument, pending-order copying, endurance, or production deployment. The two backend fixes were deployed to staging for these checks.


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

## MT4 demo acceptance: November 10, 2026

**Passed using the actual CppCopier client on MT4 FreshForex-Demo.** This is a separate broker execution run from the MT5 verification below. The runner checked the master and follower identities, demo-account status, currency and empty order lists before placing any trades.

| Check | Observed result |
|---|---|
| Start, List and Remove | Five test copiers created; all removed and their absence verified |
| Open and close | Six EURUSD master/follower pairs opened and closed |
| LotMultiplier = 2 | 0.01 master lot copied as 0.02 follower lot, including the sell after resume |
| FixedLot = 0.03 | Follower opened 0.03 lot |
| BalanceMultiplier / EquityMultiplier = 2 | Follower lots matched the calculation from current account balances/equities, rounded to the 0.01 lot step |
| FixedBalanceMultiplier = 1 | Fixed master balance set to half the follower balance; 0.01 copied as 0.02 lot |
| Pause | A separate master trade produced no follower order throughout a six-second observation window |
| Resume | A new sell copied successfully; closing the master also closed its follower |
| Live and persisted logs | Streamed success event matched both broker tickets; `/TradeLogs` contained the master ticket |
| ApiKey errors | Empty/invalid owner keys failed List; missing/invalid headers failed the copier read; an invalid-key Pause was rejected |
| Cleanup | Seven master and six follower trades closed; both order lists empty |

Trade events were checked through the authenticated WebSocket endpoint alongside this SDK’s copier-management calls. Start, List, Pause, Resume and Remove used this SDK. The runner placed and inspected demo broker orders through the MT4 terminal REST API. Sanitized tickets and check results are in [`tests/live-mt4-trading-2026-11-10.json`](https://github.com/MetaRPC/CppCopier/blob/main/tests/live-mt4-trading-2026-11-10.json).

### Repeating the MT4 check

Set the native acceptance adapter’s `COPIER_PLATFORM` environment variable to `MT4` before invoking it. The default remains `MT5`; CT/TL overrides are described in the provider setup section below. The 13 standard-input fields below are unchanged. Both account types will be `MT4`; provide MT4 demo credentials and their MT4 terminal GUIDs.

```bash
export COPIER_PLATFORM=MT4
```

In PowerShell, use `$env:COPIER_PLATFORM = 'MT4'`. Keep credentials outside the repository and supply them on standard input. Restore `MT5` or unset the variable before repeating the MT5 check.

For a custom runner, use the MT4 REST contract:

- Verify `/AccountSummary` reports the expected login and demo mode before any order.
- Read `/OpenedOrders` from `data.orderInfos`; order volume is `lots` and direction is `orderType`. Refuse unexpected pending orders.
- Send `/OrderSend` with `operationType=OC_OP_BUY` or `OC_OP_SELL`, the selected symbol and a broker-valid volume.
- Close the exact owned ticket using `/OrderCloseDelete` with `orderTicket` and `lots`.
- Wait for the follower order to appear and disappear, compare actual tickets/volumes, and verify its log event. A successful Start response alone is not evidence of replication.

The run covered MT4-to-MT4 EURUSD on this demo broker. It does not establish MT4-to-MT5 behavior, every broker’s symbol/lot rules, pending-order copying or long-running load behavior. No new copier-backend fault was found in this matrix; it used the previously fixed routing, cross-pod logs and durable pause checks.

## MT5 demo acceptance: November 10, 2026

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

This SDK exposes copier management operations; the test verified events through the authenticated WebSocket endpoint. The test used the SDK for Start, List, Pause, Resume and Remove; broker orders and position checks used MRPC terminal REST. This section records the MT5 demo run; the independent MT4 result appears above. Neither run is an extended load test. The auth-only checks and sanitized trade evidence are stored in `tests/live-auth-2026-11-10.json` and `tests/live-trading-2026-11-10.json`.

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
6. Master terminal/session ID for the copier API (preserve provider session IDs exactly).
7. Follower login.
8. Follower trading password.
9. Follower broker server.
10. Follower terminal/session ID for the copier API (preserve provider session IDs exactly).
11. Risk mode.
12. Risk value.
13. Fixed master balance, or a blank line when unused.

Keep the input in a private file outside the checkout, restrict its permissions and pass it through standard input. Do not put passwords in command arguments, source code, workflow files or logs. The output is a JSON result. A false `ok` is a failed check, even if the process exits successfully. Standard input must include the final blank field when the fixed balance is unused.

Build the SDK first, then invoke:

```text
.\build\Release\live_adapter.exe list
```

The adapter alone does not verify that an account is a demo or clean up trades. The acceptance runner must verify account identity and demo mode, start with empty positions, inspect actual follower tickets/lots and log events, and close only its own test trades before removing its copier. Do not run mutation commands against existing customer copiers.


### cTrader and TradeLocker setup

Choose each side independently with `COPIER_MASTER_PLATFORM` and `COPIER_SLAVE_PLATFORM` (`MT4`, `MT5`, `CT` or `TL`). If omitted, each uses `COPIER_PLATFORM`, which defaults to `MT5`.

Authenticate a CT/TL demo account through `POST /AddProviderAccount` first. Send the owner key in the `ApiKey` header; put provider credentials and the provider service key in the JSON body. The provider service key is separate from the copier owner's key. cTrader uses access/refresh tokens; TradeLocker uses email, password and server. Never put these credentials in a repository or a logged URL.

Use the registered account's provider, login/account ID, server and exact session ID for SDK Start. Leave its password field empty. The backend verifies the account belongs to the owner before attaching. A random session ID, another owner's session, a hidden account or mismatched login/server/provider is refused. MetaTrader credential starts continue to require their trading password. The backend must include the provider-session Start fix; older builds incorrectly reject the empty provider password.

Copier risk sizes are **lots**. CT/TL broker APIs use **units**: obtain `lotSize`, `minQuantity` and `quantityStep` from `/SymbolInfo`, rather than assuming every symbol is a forex contract. For the HEROFX EURUSD demo instrument checked on November 10, one lot is 100,000 units and the minimum/step is 1,000 units (0.01 lot). Do not generalize this mapping to other instruments.

These setup instructions describe the supported flow, not proof of broker execution. See the dated acceptance evidence for the providers and directions actually tested. Never treat a successful login or Start response as proof that a follower trade filled.
