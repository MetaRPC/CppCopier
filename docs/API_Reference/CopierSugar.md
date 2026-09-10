# CopierSugar API Reference

`CopierSugar` provides a fluent, ergonomic builder API for initializing and configuring trade copiers with zero boilerplate.

---

## Fluent Builder Example

```csharp
var copier = await CopierSugar.Create()
    .WithCredentials("YOUR_USER_KEY", "YOUR_MANAGER_KEY")
    .FromMaster(master => master
        .Type("MT5")
        .User(10001)
        .Password("masterPass")
        .Server("MetaQuotes-Demo"))
    .ToSlave(slave => slave
        .Type("MT5")
        .User(10002)
        .Password("slavePass")
        .Server("MetaQuotes-Demo"))
    .WithLotMultiplier(1.5)
    .CopyStopLoss(true)
    .CopyTakeProfit(true)
    .CopyPendingOrders(true)
    .WithSlippageGuard(15)
    .StartAsync();

Console.WriteLine($"Copier started: {copier.Id}");
```
