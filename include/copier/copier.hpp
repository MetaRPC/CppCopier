#pragma once
#include "models.hpp"
#include "grpc_wire.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace copier {

class CopierService {
public:
    CopierService(const std::string& endpoint, const std::string& user_key, const std::string& manager_key = "")
        : endpoint_(endpoint), user_key_(user_key), manager_key_(manager_key.empty() ? user_key : manager_key) {
        parse_endpoint(endpoint, host_, port_);
    }

    StartReply Start(const StartRequest& req) {
        StartReply r;
        std::vector<uint8_t> payload;
        std::string user_key = !req.user_key.empty() ? req.user_key : user_key_;
        std::string manager_key = !req.manager_key.empty() ? req.manager_key : manager_key_;

        write_string(payload, 1, user_key);
        write_string(payload, 2, manager_key);
        write_message(payload, 3, encode_account(req.master));
        write_message(payload, 4, encode_account(req.slave));
        write_string(payload, 5, !req.risk_type.empty() ? req.risk_type : "LotMultiplier");
        write_string(payload, 6, !req.risk_value.empty() ? req.risk_value : "1.0");
        write_string(payload, 7, req.fixed_master_balance);
        if (req.copy_sl) write_bool(payload, 8, true);
        if (req.copy_tp) write_bool(payload, 9, true);
        if (req.copy_pending_orders) write_bool(payload, 10, true);
        if (req.reverse_copy) write_bool(payload, 11, true);

        std::vector<uint8_t> resp;
        std::string err;
        if (!call_grpc_http2(host_, port_, "/copier.CopierService/Start", payload, resp, err)) {
            r.ok = false;
            r.error = err;
            return r;
        }

        const uint8_t* ptr = resp.data();
        const uint8_t* end = ptr + resp.size();
        while (ptr < end) {
            uint64_t tag = 0;
            if (!read_varint(ptr, end, tag)) break;
            int field = static_cast<int>(tag >> 3);
            int wire = static_cast<int>(tag & 7);
            if (wire == 0) {
                uint64_t v = 0;
                if (!read_varint(ptr, end, v)) break;
                if (field == 1) r.ok = (v != 0);
            } else if (wire == 2) {
                uint64_t len = 0;
                if (!read_varint(ptr, end, len)) break;
                if (end - ptr < static_cast<ptrdiff_t>(len)) break;
                std::string s(reinterpret_cast<const char*>(ptr), len);
                ptr += len;
                if (field == 2) r.copier_id = s;
                else if (field == 3) r.error = s;
            } else {
                if (!skip_field(ptr, end, wire)) break;
            }
        }
        return r;
    }

    ListReply List() {
        ListReply r;
        std::vector<uint8_t> payload;
        write_string(payload, 1, user_key_);

        std::vector<uint8_t> resp;
        std::string err;
        if (!call_grpc_http2(host_, port_, "/copier.CopierService/List", payload, resp, err)) {
            r.ok = false;
            r.error = err;
            return r;
        }

        const uint8_t* ptr = resp.data();
        const uint8_t* end = ptr + resp.size();
        while (ptr < end) {
            uint64_t tag = 0;
            if (!read_varint(ptr, end, tag)) break;
            int field = static_cast<int>(tag >> 3);
            int wire = static_cast<int>(tag & 7);
            if (wire == 0) {
                uint64_t v = 0;
                if (!read_varint(ptr, end, v)) break;
                if (field == 1) r.ok = (v != 0);
            } else if (wire == 2) {
                uint64_t len = 0;
                if (!read_varint(ptr, end, len)) break;
                if (end - ptr < static_cast<ptrdiff_t>(len)) break;
                if (field == 2) {
                    r.copiers.push_back(decode_copier_summary(ptr, ptr + len));
                } else if (field == 3) {
                    r.error.assign(reinterpret_cast<const char*>(ptr), len);
                }
                ptr += len;
            } else {
                if (!skip_field(ptr, end, wire)) break;
            }
        }
        return r;
    }

    SimpleReply Pause(const std::string& copier_id, bool paused) {
        SimpleReply r;
        std::vector<uint8_t> payload;
        write_string(payload, 1, user_key_);
        write_string(payload, 2, copier_id);
        write_bool(payload, 3, paused);

        std::vector<uint8_t> resp;
        std::string err;
        if (!call_grpc_http2(host_, port_, "/copier.CopierService/Pause", payload, resp, err)) {
            r.ok = false;
            r.error = err;
            return r;
        }
        return decode_simple_reply(resp);
    }

    SimpleReply Remove(const std::string& copier_id) {
        SimpleReply r;
        std::vector<uint8_t> payload;
        write_string(payload, 1, user_key_);
        write_string(payload, 2, copier_id);

        std::vector<uint8_t> resp;
        std::string err;
        if (!call_grpc_http2(host_, port_, "/copier.CopierService/Remove", payload, resp, err)) {
            r.ok = false;
            r.error = err;
            return r;
        }
        return decode_simple_reply(resp);
    }

private:
    std::string endpoint_;
    std::string user_key_;
    std::string manager_key_;
    std::string host_{"copy.mrpc.pro"};
    int port_{443};

    static void parse_endpoint(std::string ep, std::string& host, int& port) {
        if (ep.rfind("https://", 0) == 0) ep = ep.substr(8);
        else if (ep.rfind("http://", 0) == 0) ep = ep.substr(7);
        auto colon = ep.find(':');
        if (colon != std::string::npos) {
            host = ep.substr(0, colon);
            port = std::stoi(ep.substr(colon + 1));
        } else {
            host = ep;
            port = 443;
        }
    }

    static std::vector<uint8_t> encode_account(const Account& acc) {
        std::vector<uint8_t> b;
        write_string(b, 1, !acc.type.empty() ? acc.type : "MT5");
        write_uint64(b, 2, acc.user);
        write_string(b, 3, acc.password);
        write_string(b, 4, acc.server);
        write_string(b, 5, acc.name);
        write_string(b, 6, acc.id);
        return b;
    }

    static SimpleReply decode_simple_reply(const std::vector<uint8_t>& resp) {
        SimpleReply r;
        const uint8_t* ptr = resp.data();
        const uint8_t* end = ptr + resp.size();
        while (ptr < end) {
            uint64_t tag = 0;
            if (!read_varint(ptr, end, tag)) break;
            int field = static_cast<int>(tag >> 3);
            int wire = static_cast<int>(tag & 7);
            if (wire == 0) {
                uint64_t v = 0;
                if (!read_varint(ptr, end, v)) break;
                if (field == 1) r.ok = (v != 0);
            } else if (wire == 2) {
                uint64_t len = 0;
                if (!read_varint(ptr, end, len)) break;
                if (end - ptr < static_cast<ptrdiff_t>(len)) break;
                if (field == 2) r.error.assign(reinterpret_cast<const char*>(ptr), len);
                ptr += len;
            } else {
                if (!skip_field(ptr, end, wire)) break;
            }
        }
        return r;
    }

    static CopierSummary decode_copier_summary(const uint8_t* ptr, const uint8_t* end) {
        CopierSummary s;
        while (ptr < end) {
            uint64_t tag = 0;
            if (!read_varint(ptr, end, tag)) break;
            int field = static_cast<int>(tag >> 3);
            int wire = static_cast<int>(tag & 7);
            if (wire == 0) {
                uint64_t v = 0;
                if (!read_varint(ptr, end, v)) break;
                if (field == 3) s.master_user = v;
                else if (field == 6) s.slave_user = v;
                else if (field == 10) s.paused = (v != 0);
            } else if (wire == 2) {
                uint64_t len = 0;
                if (!read_varint(ptr, end, len)) break;
                if (end - ptr < static_cast<ptrdiff_t>(len)) break;
                std::string str(reinterpret_cast<const char*>(ptr), len);
                ptr += len;
                if (field == 1) s.id = str;
                else if (field == 2) s.master_type = str;
                else if (field == 4) s.master_server = str;
                else if (field == 5) s.slave_type = str;
                else if (field == 7) s.slave_server = str;
                else if (field == 8) s.risk_type = str;
                else if (field == 9) s.risk_value = str;
                else if (field == 11) s.pause_reason = str;
            } else {
                if (!skip_field(ptr, end, wire)) break;
            }
        }
        return s;
    }
};

} // namespace copier
