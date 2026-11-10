# cTrader and TradeLocker

Use the same copier lifecycle with a different account route. Broker authentication happens once in the provider service; the copier then attaches to that registered session. Do not pass a TradeLocker password or cTrader token as a MetaTrader password.

!!! important "Deployment requirement"

    The copier backend needs the website-owner registration fix (`5a26a02`) and provider-session Start fix (`be35ccf`). These were deployed to staging for the November 10 checks. A push to main is not proof that production has deployed them. Older Start builds incorrectly require a password for CT/TL.

## Keep the identifiers separate

| Value | Used for |
|---|---|
| Copier owner API key | `ApiKey` header / `apikey` gRPC metadata and the owner field in copier requests |
| Provider service API key | Authenticating calls to the configured cTrader or TradeLocker API; stored during account registration |
| Broker credentials | cTrader access/refresh tokens, or TradeLocker email/password/server; sent only during authentication |
| Registered account ID | Copier database record returned by `AddProviderAccount`; used by REST `StartByAccountId` |
| Provider session ID | Registered record's `apiId`; supplied as SDK `Account.id`, without reformatting |
| Broker account ID | Registered record's `user`; supplied as SDK `Account.user` |

A registered account ID and a provider session ID are different identifiers. Keep large numeric IDs as strings in JavaScript. Use the server returned by registration in the copier request; do not replace it with the broker authentication server name.

## 1. Register the demo account

Send `POST /AddProviderAccount` to the **copier backend**, with the owner's `ApiKey` header and `Content-Type: application/json`. The following bodies contain placeholders, not credentials.

=== "TradeLocker"

    ```json
    {
      "userKey": "OWNER_API_KEY",
      "managerKey": "OWNER_API_KEY",
      "type": "TL",
      "name": "My TradeLocker demo",
      "accountId": "BROKER_DEMO_ACCOUNT_ID",
      "apiKey": "TRADELOCKER_SERVICE_API_KEY",
      "email": "demo-owner@example.com",
      "password": "DEMO_TRADING_PASSWORD",
      "server": "BROKER_AUTHENTICATION_SERVER"
    }
    ```

=== "cTrader"

    ```json
    {
      "userKey": "OWNER_API_KEY",
      "managerKey": "OWNER_API_KEY",
      "type": "CT",
      "name": "My cTrader demo",
      "accountId": "AUTHORIZED_DEMO_ACCOUNT_ID",
      "apiKey": "CTRADER_SERVICE_API_KEY",
      "accessToken": "DEMO_ACCESS_TOKEN",
      "refreshToken": "DEMO_REFRESH_TOKEN"
    }
    ```

The manager must be authorized to create accounts, and the owner must be an existing local or website-verified customer. The provider must authorize the requested demo account. Keep the request body in a secret store or private file; do not place it in shell history, browser code or a public repository.

The successful response identifies the registered copier account. Read that record through the existing `/Accounts` route using the same owner, and retain its `type`, `user`, `server` and `apiId`. Legacy REST routes may carry `userKey` in the query string: redact queries in access logs and never share the URL with its key.

## 2. Start with the registered session

For the CT/TL side of the SDK Start request:

| Account field | Value |
|---|---|
| `type` | `CT` or `TL`, as registered |
| `user` | Registered `user` / broker account ID |
| `server` | Registered `server`, such as `CT-demo` or `TL-demo` |
| `id` | Exact registered `apiId` |
| `password` | Empty string |
| `name` | An optional label |

The other side can use a supported MetaTrader route. MetaTrader credential starts still require a trading password. Set the risk mode, risk value and copy options explicitly. The backend checks the CT/TL session's owner, provider, account, server, service key and hidden status before creating a copier. A supplied ID alone is not authorization.

The SDK controls Start, List, Pause, Resume and Remove. Broker test orders use the corresponding provider or terminal API; the core copier wrapper is not a universal broker order client.

## 3. Check size in the correct units

Copier risk settings use **lots**. The cTrader and TradeLocker provider APIs use **units**. Read `/SymbolInfo` for the selected instrument:

```text
provider quantity = copier lots × lotSize
```

Check `minQuantity`, `maxQuantity` and `quantityStep` before an order. On the tested HEROFX and Spotware demo EURUSD instruments, `lotSize = 100000` and the step is `1000`, so 0.01 lot is 1,000 units. Metals, indices, crypto and another broker can have different rules. A symbol name alone does not establish the conversion.

Balance and equity modes currently compare raw numeric account values, without currency conversion. For accounts in different currencies, choose the multiplier deliberately; equal numeric balances are not equal monetary balances. The cTrader demo checks use explicit multipliers calculated from the current balances to keep follower volume near 0.02 lot.

## 4. Verify the broker result and logs

1. Confirm both accounts are demos and record their existing positions and working orders.
2. Start one copier and confirm it appears in List.
3. Open a small master demo position through its broker API. Inspect the follower's actual position, symbol, side and quantity.
4. Match the exact master and follower tickets to the live log, then read persisted `/TradeLogs` history.
5. Close the master and wait until its corresponding follower closes.
6. Pause and confirm the saved state. A new master position must not produce a new follower position while paused. Close that test position before resuming.
7. Resume, place a new small position, and verify copying and closure again.
8. Remove only the test copier, confirm it is absent, then disconnect only test sessions and remove only account records created for the test.

During execution, a market order may briefly appear in the provider order list before its position is visible. Distinguish it from a pending limit/stop order and wait for the broker state to settle; initial and final cleanup checks must still require no working orders.
A successful login, Start response or order submission is insufficient evidence of a fill. Provider commands have a request ID and execution state; on an uncertain result, query that same command and reconcile positions. Do not blindly repeat a mutation with a new request ID.

TradeLocker events use provider polling, so allow for delivery delay. A stream is not durable replay: after a disconnect, reconcile broker positions and persisted logs. See [trade logs](logs.md) for each SDK's streaming support.

## Authentication failures

Send the provider service key as `ApiKey` and the session as `id`. A missing key must not reveal a session belonging to another owner. In the staging checks, an absent key returned 404 (`TERMINAL_INSTANCE_NOT_FOUND`), an invalid key returned 401 (`API_KEY_INVALID`), and the correct key returned the account summary. Do not assume every missing-key path returns 401; verify that account data remains inaccessible.

## Verified scope

The [dated acceptance results](testing.md) list the SDK, broker and directions actually exercised. Protocol support, local contract tests and live broker execution are separate claims. Each provider and direction needs its own execution result; one successful route does not establish another.
