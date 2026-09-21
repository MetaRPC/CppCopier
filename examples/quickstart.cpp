#include <copier/copier.hpp>
#include <copier/demo.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== MetaRPC CppCopier Trade Replication Quick Start ===" << std::endl;
    std::string api_key = "TRIAL";
    copier::DemoAccountClient demo("mt5.mrpc.pro");
    std::string master_guid;
    std::string slave_guid;
    std::string copier_id;

    try {
        copier::CopierService client("copy.mrpc.pro:443", api_key);

        // 1. Provision live demo accounts
        std::cout << "\n[1] Provisioning live demo accounts on MetaQuotes-Demo..." << std::endl;
        auto master = demo.OpenDemoAccount("MetaQuotes-Demo", api_key);
        std::cout << "    Master Account Provisioned: #" << master.login << " on " << master.server << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        auto slave = demo.OpenDemoAccount("MetaQuotes-Demo", api_key);
        std::cout << "    Slave Account Provisioned:  #" << slave.login << " on " << slave.server << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // 2. Connect terminals via ConnectEx with APIKey: TRIAL
        std::cout << "\n[2] Connecting terminals via ConnectEx (APIKey: " << api_key << ")..." << std::endl;
        auto conn_m = demo.ConnectEx(master.login, master.password, master.server, api_key);
        master_guid = conn_m.terminal_instance_guid;
        std::cout << "    Master Terminal Connected! GUID: " << master_guid << std::endl;

        auto conn_s = demo.ConnectEx(slave.login, slave.password, slave.server, api_key);
        slave_guid = conn_s.terminal_instance_guid;
        std::cout << "    Slave Terminal Connected!  GUID: " << slave_guid << std::endl;

        std::string master_session_id = copier::DemoAccountClient::ToHyphenGuid(master_guid);
        std::string slave_session_id = copier::DemoAccountClient::ToHyphenGuid(slave_guid);

        // 3. Start Trade Copier via gRPC on copy.mrpc.pro:443
        std::cout << "\n[3] Starting Trade Copier via gRPC on copy.mrpc.pro:443..." << std::endl;
        copier::StartRequest req;
        req.user_key = api_key;
        req.risk_type = "LotMultiplier";
        req.risk_value = "1.0";

        req.master.type = "MT5";
        req.master.user = master.login;
        req.master.password = master.password;
        req.master.server = master.server;
        req.master.id = master_session_id;

        req.slave.type = "MT5";
        req.slave.user = slave.login;
        req.slave.password = slave.password;
        req.slave.server = slave.server;
        req.slave.id = slave_session_id;

        auto start_reply = client.Start(req);
        std::cout << "    gRPC Start Reply: ok=" << (start_reply.ok ? "true" : "false")
                  << ", copierId=" << start_reply.copier_id << ", error=" << start_reply.error << std::endl;
        if (!start_reply.ok) {
            throw std::runtime_error("Failed to start copier: " + start_reply.error);
        }
        copier_id = start_reply.copier_id;

        std::this_thread::sleep_for(std::chrono::milliseconds(4000));

        // 4. Place Market Order on Master
        std::cout << "\n[4] Opening Market Order on Master (0.01 EURUSD BUY)..." << std::endl;
        uint64_t master_ticket = demo.OrderSend(master_guid, "EURUSD", "TMT5_ORDER_TYPE_BUY", 0.01, api_key);
        std::cout << "    Master Order Placed! Ticket: " << master_ticket << std::endl;

        // 5. Verify Trade Copied to Slave
        std::cout << "\n[5] Verifying replicated trade on Slave account..." << std::endl;
        bool replicated = false;
        for (int attempt = 1; attempt <= 15; ++attempt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            auto positions = demo.OpenedOrders(slave_guid, api_key);
            std::cout << "    Attempt " << attempt << ": Slave active positions count = " << positions.size() << std::endl;
            if (!positions.empty()) {
                auto first = positions[0];
                std::cout << "    --> CONFIRMED ON SLAVE: Ticket=" << first.ticket << ", Symbol=" << first.symbol << std::endl;
                replicated = true;
                break;
            }
        }

        if (!replicated) {
            std::cout << "    WARNING: Slave trade replication timed out." << std::endl;
        } else {
            std::cout << "    SUCCESS: Trade successfully replicated to slave account!" << std::endl;
        }

        // 6. Close Position on Master
        if (master_ticket != 0) {
            std::cout << "\n[6] Closing Master trade ticket #" << master_ticket << "..." << std::endl;
            std::string close_resp = demo.OrderClose(master_guid, master_ticket, api_key);
            std::cout << "    Master OrderClose result: " << close_resp << std::endl;

            // 7. Verify Trade Closed on Slave
            std::cout << "\n[7] Verifying trade closed on Slave..." << std::endl;
            for (int attempt = 1; attempt <= 15; ++attempt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                auto positions = demo.OpenedOrders(slave_guid, api_key);
                if (positions.empty()) {
                    std::cout << "    SUCCESS: Slave position closed by trade copier!" << std::endl;
                    break;
                }
                std::cout << "    Attempt " << attempt << ": Slave positions still open: " << positions.size() << std::endl;
            }
        }

        // 8. Remove Copier via gRPC
        if (!copier_id.empty()) {
            std::cout << "\n[8] Removing Copier " << copier_id << " via gRPC..." << std::endl;
            auto rem_reply = client.Remove(copier_id);
            std::cout << "    Copier Remove Reply: ok=" << (rem_reply.ok ? "true" : "false") << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Execution error: " << e.what() << std::endl;
    }

    // 9. Cleanly Disconnect Terminal Sessions
    std::cout << "\n[9] Disconnecting terminal sessions cleanly via /Disconnect..." << std::endl;
    if (!master_guid.empty()) {
        try {
            auto disc_m = demo.Disconnect(master_guid, api_key);
            std::cout << "    Master Terminal Cleanly Disconnected: " << disc_m.unique_identifier << " (Lifetime: " << disc_m.lifetime_seconds << "s)" << std::endl;
        } catch (const std::exception& ex) {
            std::cout << "    Master disconnect error: " << ex.what() << std::endl;
        }
    }
    if (!slave_guid.empty()) {
        try {
            auto disc_s = demo.Disconnect(slave_guid, api_key);
            std::cout << "    Slave Terminal Cleanly Disconnected:  " << disc_s.unique_identifier << " (Lifetime: " << disc_s.lifetime_seconds << "s)" << std::endl;
        } catch (const std::exception& ex) {
            std::cout << "    Slave disconnect error: " << ex.what() << std::endl;
        }
    }

    std::cout << "\n=== CppCopier Trade Replication Completed Successfully ===" << std::endl;
    return 0;
}
