#pragma once
#include "models.hpp"
#include <iostream>

namespace copier {

class CopierService {
public:
    CopierService(const std::string& endpoint, const std::string& user_key, const std::string& manager_key = "")
        : endpoint_(endpoint), user_key_(user_key), manager_key_(manager_key.empty() ? user_key : manager_key) {}

    StartReply Start(const StartRequest& req) {
        StartReply r;
        r.ok = true;
        r.copier_id = "3fa85f64-5717-4562-b3fc-2c963f66afa6";
        return r;
    }

    ListReply List() {
        ListReply r;
        r.ok = true;
        CopierSummary c;
        c.id = "3fa85f64-5717-4562-b3fc-2c963f66afa6";
        c.master_type = "MT5";
        c.master_user = 10001;
        c.master_server = "MetaQuotes-Demo";
        c.slave_type = "MT5";
        c.slave_user = 10002;
        c.slave_server = "MetaQuotes-Demo";
        c.risk_type = "LotMultiplier";
        c.risk_value = "1.5";
        r.copiers.push_back(c);
        return r;
    }

    SimpleReply Pause(const std::string& copier_id, bool paused) {
        SimpleReply r;
        r.ok = true;
        return r;
    }

    SimpleReply Remove(const std::string& copier_id) {
        SimpleReply r;
        r.ok = true;
        return r;
    }

private:
    std::string endpoint_;
    std::string user_key_;
    std::string manager_key_;
};

} // namespace copier
