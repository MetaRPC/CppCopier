# Input and Output Structs Reference

Complete reference for all gRPC messages, structs, and payloads across **CppCopier**.

---

## 1. Core Copier Messages (`copier.proto`)

### `Account`
Represents an individual MetaTrader terminal connection:
```protobuf
message Account {
  string type = 1;     // "MT4" or "MT5"
  uint64 user = 2;     // Account login
  string password = 3; // Account trading password
  string server = 4;   // Broker server name
  string name = 5;     // Optional friendly label
}
```

### `StartRequest` & `StartReply`
```protobuf
message StartRequest {
  string user_key = 1;
  string manager_key = 2;
  Account master = 3;
  Account slave = 4;
  string risk_type = 5;
  string risk_value = 6;
  string fixed_master_balance = 7;
  bool copy_sl = 8;
  bool copy_tp = 9;
  bool copy_pending_orders = 10;
  bool reverse_copy = 11;
}

message StartReply {
  bool ok = 1;
  string copier_id = 2;
  string error = 3;
}
```

### `ListRequest` & `ListReply`
```protobuf
message ListRequest {
  string user_key = 1;
}

message ListReply {
  bool ok = 1;
  repeated CopierSummary copiers = 2;
  string error = 3;
}

message CopierSummary {
  string id = 1;
  string master_type = 2;
  uint64 master_user = 3;
  string master_server = 4;
  string slave_type = 5;
  uint64 slave_user = 6;
  string slave_server = 7;
  string risk_type = 8;
  string risk_value = 9;
  bool paused = 10;
  string pause_reason = 11;
}
```

### `PauseRequest`, `RemoveRequest`, `SimpleReply`
```protobuf
message PauseRequest {
  string user_key = 1;
  string copier_id = 2;
  bool paused = 3;
}

message RemoveRequest {
  string user_key = 1;
  string copier_id = 2;
}

message SimpleReply {
  bool ok = 1;
  string error = 2;
}
```

---

## 2. Trade Log Struct (`TradeLog`)

Received over WebSocket stream `/OnTradeLog?id={copierId}` or `/CopierTradeLogs`:

```json
{
  "id": "664019a84b025f12e9b01934",
  "copierId": "3fa85f64-5717-4562-b3fc-2c963f66afa6",
  "userKey": "usr_99812",
  "masterUser": "500129",
  "slaveUser": "700982",
  "timeUTC": "2026-09-10T12:00:00Z",
  "symbol": "EURUSD",
  "updateType": "MarketOpen",
  "masterOrder": {
    "ticket": 12849102,
    "type": "Buy",
    "volume": 1.0,
    "openPrice": 1.08542,
    "sl": 1.08200,
    "tp": 1.09200
  },
  "slaveOrder": {
    "ticket": 99341021,
    "type": "Buy",
    "volume": 1.5,
    "openPrice": 1.08544,
    "sl": 1.08200,
    "tp": 1.09200
  },
  "success": true,
  "comment": "MetaRPC Copier execution"
}
```

---

## 3. Demo Account Creation Messages (`mt5_term_api.proto`)

```protobuf
message GuiDemoOpenAccountRequest {
  string company = 1;
  string first_name = 2;
  string last_name = 3;
  string email = 4;
  string phone = 5;
  string server = 6;
  string account_type = 7;
  int32 timeout_seconds = 8;
}

message GuiDemoOpenAccountReply {
  int32 result_code = 1;
  uint64 login = 2;
  string password = 3;
  string investor = 4;
  string server = 5;
  string debug_log = 6;
}
```
