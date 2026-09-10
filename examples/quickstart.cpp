#include <copier/copier.hpp>
#include <iostream>

int main() {
    std::cout << "=== CppCopier Quick Start ===" << std::endl;
    copier::CopierService client("copy.mrpc.pro:443", "YOUR_USER_KEY");

    copier::StartRequest req;
    req.master = {"MT5", 10001, "pass1", "MetaQuotes-Demo", "Master"};
    req.slave = {"MT5", 10002, "pass2", "MetaQuotes-Demo", "Slave"};
    req.risk_type = "LotMultiplier";
    req.risk_value = "1.5";

    auto start_rep = client.Start(req);
    std::cout << "Copier ID: " << start_rep.copier_id << std::endl;

    auto list_rep = client.List();
    std::cout << "Active copiers: " << list_rep.copiers.size() << std::endl;
    return 0;
}
