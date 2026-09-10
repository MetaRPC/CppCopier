#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace copier {

struct Account {
    std::string type{"MT5"};
    uint64_t user{0};
    std::string password;
    std::string server;
    std::string name;
};

struct StartRequest {
    std::string user_key;
    std::string manager_key;
    Account master;
    Account slave;
    std::string risk_type{"LotMultiplier"};
    std::string risk_value{"1.0"};
    std::string fixed_master_balance;
    bool copy_sl{true};
    bool copy_tp{true};
    bool copy_pending_orders{false};
    bool reverse_copy{false};
};

struct StartReply {
    bool ok{true};
    std::string copier_id;
    std::string error;
};

struct CopierSummary {
    std::string id;
    std::string master_type;
    uint64_t master_user{0};
    std::string master_server;
    std::string slave_type;
    uint64_t slave_user{0};
    std::string slave_server;
    std::string risk_type;
    std::string risk_value;
    bool paused{false};
    std::string pause_reason;
};

struct ListReply {
    bool ok{true};
    std::vector<CopierSummary> copiers;
    std::string error;
};

struct SimpleReply {
    bool ok{true};
    std::string error;
};

} // namespace copier
