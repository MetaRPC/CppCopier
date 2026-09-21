#include <copier/copier.hpp>
#include <copier/demo.hpp>
#include <iostream>

int main() {
    std::cout << "=== CppCopier Quick Start Demo ===" << std::endl;
    std::string api_key = "TRIAL";

    copier::DemoAccountClient demo("mt5.mrpc.pro");

    // 1. Provision live demo account
    std::cout << "\n[1] Provisioning live demo account on MetaQuotes-Demo..." << std::endl;
    auto master = demo.OpenDemoAccount("MetaQuotes-Demo", api_key);
    std::cout << "    Master Account Provisioned: #" << master.login << " on " << master.server << std::endl;

    // 2. Connect terminal via ConnectEx with APIKey: TRIAL
    std::cout << "\n[2] Connecting terminal via ConnectEx (APIKey: " << api_key << ")..." << std::endl;
    auto conn = demo.ConnectEx(master.login, master.password, master.server, api_key);
    std::cout << "    Terminal Connected! Instance GUID: " << conn.terminal_instance_guid << std::endl;

    // 3. Interacting with Copier Service
    std::cout << "\n[3] Interacting with Copier Service (user_key: " << api_key << ")..." << std::endl;
    copier::CopierService client("copy.mrpc.pro:443", api_key);
    auto list_rep = client.List();
    std::cout << "    Active copiers count: " << list_rep.copiers.size() << std::endl;

    // 4. Cleanly Disconnect Terminal Session
    std::cout << "\n[4] Disconnecting terminal session " << conn.terminal_instance_guid << "..." << std::endl;
    auto disc = demo.Disconnect(conn.terminal_instance_guid, api_key);
    std::cout << "    Terminal Cleanly Disconnected: " << disc.unique_identifier << " (Lifetime: " << disc.lifetime_seconds << "s)" << std::endl;

    std::cout << "\n=== CppCopier Quick Start Completed Successfully ===" << std::endl;
    return 0;
}
