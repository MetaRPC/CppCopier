# Authentication and ownership

Use your real **MetaRPC owner API key** for every SDK request. Keep it on your server, in an environment variable or secret store. `TRIAL` is not an owner identity for a copier.

| Transport | What to send | What the server uses |
|---|---|---|
| REST | `ApiKey: <owner key>`; also the route's required `userKey` or `managerKey` | Route-specific checks; guarded record reads and writes recognize `ApiKey` |
| gRPC | `apikey` metadata on every call, plus `user_key` in the protobuf body | Customer RPCs validate the body owner key; Start also validates account ownership |
| WebSocket | `ApiKey` on the HTTP upgrade request to `/OnTradeLog?id=...` | The copier ownership check runs before the upgrade |

HTTP header names are case-insensitive. gRPC metadata keys are lowercase. The SDK sends `apikey`; cURL examples use `ApiKey`. They represent the same header.

!!! important "The header does not replace request fields"
    `user_key` identifies the owner. `manager_key` identifies the creating manager and normally equals the owner's key for self-service use. A copier ID, terminal ID or broker login is **not** an API key. Keep the configured key and body identity consistent.

## Verified server behavior

The audit used `tradecopy` commit `9d2571aaa49236f5463c539461625acdf577f9bd`: `CopierAccess.PresentedKeys`, `CopierAccess.Rules`, `CopierGrpcService`, and `MrpcOwnerGate`. The public `/GetCopier` route returned HTTP 401 with `API_KEY_REQUIRED` without a key during the November 10, 2026 audit.

There is no universal header-only check across every route in the source. Some routes authenticate explicit key parameters, and legacy deployments have different gates. These SDKs consistently send the header rather than relying on those differences. A Bearer header alone is not a substitute for `ApiKey` on guarded REST/WebSocket routes.

## Configure without committing a secret

=== "Bash"
    ```bash
    export MRPC_API_KEY='your-owner-key'
    ```
=== "PowerShell"
    ```powershell
    $env:MRPC_API_KEY = 'your-owner-key'
    ```

Do not put real keys into examples, screenshots, workflow YAML, browser bundles or issue reports. Never print metadata or broker passwords. Some legacy REST routes require key query parameters: redact query strings in proxy logs and prefer gRPC for credentials-bearing Start requests.

## Failure meanings

| Result | Meaning | Next step |
|---|---|---|
| `API_KEY_REQUIRED` / 401 | No usable owner key | Check the header and environment |
| `ACCESS_DENIED` / 403 | Caller cannot access the record | Verify owner and ID; retrying does not grant access |
| `ACCESS_CHECK_UNAVAILABLE` | Ownership could not be verified | Retry a read with bounded backoff; investigate service health |
| gRPC reply `ok=false` | Operation refused or failed | Read `error`; HTTP 200 alone is not success |
| gRPC `UNAUTHENTICATED` / `PERMISSION_DENIED` | Authorization failed | Correct credentials before retrying |

For delegated management, use an explicitly authorized manager key. Never distribute operator internal keys or super-manager keys in customer SDKs.
