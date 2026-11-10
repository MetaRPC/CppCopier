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
