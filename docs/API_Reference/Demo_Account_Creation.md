# Creating Demo Accounts with gRPC

MetaRPC provides the `DemoAccount` gRPC service to automatically provision real MetaTrader 4 and MetaTrader 5 demo accounts on demand. This enables automated end-to-end testing, integration verification, and instant copier demonstrations without manual account registration.

---

## 1. The `DemoAccount` Service

- **gRPC Endpoint**: `mt5.mrpc.pro:443` (for MT5) or `mt4.mrpc.pro:443` (for MT4)
- **Method**: `rpc OpenDemoAccount (GuiDemoOpenAccountRequest) returns (GuiDemoOpenAccountReply);`

### Request Parameters (`GuiDemoOpenAccountRequest`)
- `company`: Broker company identifier (e.g. `"MetaQuotes Software Corp."` or `"IC Markets"`).
- `first_name`: Account holder first name.
- `last_name`: Account holder last name.
- `email`: Contact email address.
- `phone`: Contact phone number.
- `server`: Demo server cluster (e.g. `"MetaQuotes-Demo"`).
- `account_type`: Account specification (e.g. `"forex"`, `"demo-hedging"`).
- `timeout_seconds`: Operation timeout (typically 30 seconds).

### Response Fields (`GuiDemoOpenAccountReply`)
- `result_code`: `0` indicates success.
- `login`: Generated trading account number (`uint64`).
- `password`: Generated master trading password.
- `investor`: Generated read-only investor password.
- `server`: The server name the account was opened on.

---

## 2. Code Example in C++

```cpp
// Connect to gRPC gateway
var demoClient = new DemoAccountClient("https://mt5.mrpc.pro:443");

// Provision Master Account
var master = await demoClient.OpenDemoAccountAsync(new GuiDemoOpenAccountRequest
{
    Company = "MetaQuotes Software Corp.",
    FirstName = "Master",
    LastName = "Trader",
    Email = "master.trader@metarpc.pro",
    Server = "MetaQuotes-Demo",
    TimeoutSeconds = 30
});

Console.WriteLine($"Master Account Created: #{master.Login} (Pass: {master.Password}) on {master.Server}");

// Provision Slave Account
var slave = await demoClient.OpenDemoAccountAsync(new GuiDemoOpenAccountRequest
{
    Company = "MetaQuotes Software Corp.",
    FirstName = "Slave",
    LastName = "Follower",
    Email = "slave.follower@metarpc.pro",
    Server = "MetaQuotes-Demo",
    TimeoutSeconds = 30
});

Console.WriteLine($"Slave Account Created: #{slave.Login} (Pass: {slave.Password}) on {slave.Server}");
```
