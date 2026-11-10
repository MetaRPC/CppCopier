# Errors, timeouts and recovery

Check both transport errors (DNS, TLS, timeout, gRPC status) and application failures (`ok=false`). An empty list is meaningful only when the operation succeeded.

| Symptom | Check |
|---|---|
| Cannot connect | Endpoint, TLS trust, HTTP/2, outbound firewall |
| Unauthorized REST/log stream | ApiKey header, real owner key and record ownership |
| Start refused | Broker server, login/password, permissions and ownership |
| Copier exists but no trade | Pause reason, event, mapping, filters, hours and margin |
| Wrong volume | Sizing, currency, contract size and lot constraints |
| No log events | Copier ID, environment, handshake and actual activity |
| Resume refused | Billing or another pause reason |
| Duplicate copying | Repeated Start after a timeout; reconcile List first |

## Retry deliberately

Retry transient **reads** with bounded exponential backoff and jitter. Do not retry invalid credentials, access denial or broker validation without fixing the cause. Reconcile state after an ambiguous Start, Pause or Remove failure before repeating a mutation.

Set a workflow deadline as well as per-call timeouts. Cancel work when its caller stops. Do not loop indefinitely against rejected broker credentials.

## Report a useful issue

Include SDK commit/version, runtime version, endpoint hostname, operation, UTC timestamp, sanitized error and a minimal demo reproduction. Redact keys, passwords, raw request bodies and credential-bearing URLs.

`ok=true` means the management operation succeeded. It does not certify that future trades will be accepted. Verify actual positions and logs.
