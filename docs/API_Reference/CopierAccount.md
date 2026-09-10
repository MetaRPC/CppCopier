# CopierAccount API Reference

`CopierAccount` is the foundational gRPC protocol layer in **CppCopier**.

---

## Protocol Specifications

- **Transport**: HTTP/2 over TLS
- **Multiplexing**: Single persistent TCP connection supports concurrent calls and bidirectional streaming
- **Compression**: gzip / identity
- **Keep-Alive**: Ping interval 30s, ping timeout 10s

## Header Metadata

All gRPC requests automatically inject the following HTTP/2 metadata headers:

```http
authorization: Bearer <user_key>
x-metarpc-manager: <manager_key>
x-metarpc-client-sdk: CppCopier/1.0.0
```

## Error Handling

Standard gRPC status codes are translated into idiomatic C++ exceptions:
- `UNAUTHENTICATED (16)`: Invalid user key or manager key.
- `INVALID_ARGUMENT (3)`: Malformed account number, missing server, or unsupported risk type.
- `NOT_FOUND (5)`: Specified `copier_id` does not exist or has already been removed.
- `UNAVAILABLE (14)`: Temporary connection issue with automatic exponential backoff.
