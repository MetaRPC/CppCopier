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

The November 10 audit passed the SDK build/tests and strict documentation/link checks in GitHub Actions. Tests cover the implemented transport, key validation and relevant regression cases; they are not a claim that live broker replication was performed.

## Live verification: November 10, 2026

The native client was tested against `copyback-stg.mrpc.pro` after correcting the server's gRPC ingress route.

| Check | Result |
|---|---|
| List using a valid owner key | Passed against the live service |
| List using an invalid owner key | Rejected |
| List using an empty owner key | Rejected, before connection where supported |
| Guarded REST read and WebSocket log upgrade with the valid owner key | HTTP 200 / 101 |
| Same REST read and WebSocket upgrade without ApiKey | HTTP 401 / 401 |
| Same REST read and WebSocket upgrade with an invalid ApiKey | HTTP 403 / 403 |
| Open and close replicated demo trades | Not run: staging terminal capacity reached |
| Sizing, pause/resume and server trade logs | Not run: requires connected demo terminals |

Two new disposable MT5 demo accounts were provisioned, but `ConnectEx` returned `MAX_TERMINALS_PER_POD_REACHED` at the staging host's 30-terminal limit. No trades were placed. These successful connection/authentication checks must not be presented as a completed trading acceptance test.

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
