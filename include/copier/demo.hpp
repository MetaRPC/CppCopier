#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <iostream>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

#include <vector>
#include <algorithm>

namespace copier {

struct PositionInfo {
    uint64_t ticket{0};
    std::string symbol;
    double volume{0.0};
    std::string type;
};

struct DemoReply {
    int result_code{0};
    uint64_t login{0};
    std::string password;
    std::string investor;
    std::string server;
};

struct ConnectExReply {
    std::string terminal_instance_guid;
    std::string terminal_type{"MT5"};
};

struct DisconnectReply {
    std::string unique_identifier;
    int lifetime_seconds{0};
};

class DemoAccountClient {
public:
    explicit DemoAccountClient(const std::string& host = "mt5.mrpc.pro") : host_(host) {}

    DemoReply OpenDemoAccount(const std::string& server = "MetaQuotes-Demo", const std::string& api_key = "TRIAL") {
        std::string path = "/DemoAccount/Open?server=" + UrlEncode(server);
        std::string body = HttpRequest("GET", path, api_key);

        DemoReply rep;
        std::string login_str = ExtractString(body, "login");
        if (!login_str.empty()) {
            rep.login = std::stoull(login_str);
        }
        rep.password = ExtractString(body, "password");
        rep.investor = ExtractString(body, "investor");
        rep.server = ExtractString(body, "server");
        if (rep.server.empty()) rep.server = server;
        return rep;
    }

    ConnectExReply ConnectEx(uint64_t user, const std::string& password, const std::string& server = "MetaQuotes-Demo", const std::string& api_key = "TRIAL") {
        std::ostringstream oss;
        oss << "/ConnectEx?user=" << user << "&password=" << UrlEncode(password) << "&mtClusterName=" << UrlEncode(server);
        std::string body = HttpRequest("GET", oss.str(), api_key);

        ConnectExReply rep;
        rep.terminal_instance_guid = ExtractString(body, "terminalInstanceGuid");
        return rep;
    }

    DisconnectReply Disconnect(const std::string& terminal_id, const std::string& api_key = "TRIAL", bool delete_terminal = false) {
        std::string path = "/Disconnect?delete=" + std::string(delete_terminal ? "true" : "false");
        std::string body = HttpRequest("GET", path, api_key, terminal_id, delete_terminal);

        DisconnectReply rep;
        rep.unique_identifier = ExtractString(body, "uniqueIdentifier");
        std::string lt = ExtractNumber(body, "fullLifeTimeSeconds");
        if (!lt.empty()) {
            rep.lifetime_seconds = std::stoi(lt);
        }
        return rep;
    }

    static std::string ToHyphenGuid(std::string guid) {
        size_t p = guid.find("mt5_live_");
        if (p != std::string::npos) guid.erase(p, 9);
        guid.erase(std::remove(guid.begin(), guid.end(), '-'), guid.end());
        if (guid.length() < 32) return guid;
        return guid.substr(0, 8) + "-" + guid.substr(8, 4) + "-" + guid.substr(12, 4) + "-" + guid.substr(16, 4) + "-" + guid.substr(20, 12);
    }

    uint64_t OrderSend(const std::string& terminal_id, const std::string& symbol, const std::string& operation, double volume, const std::string& api_key = "TRIAL") {
        std::ostringstream oss;
        oss << "/OrderSend?id=" << UrlEncode(terminal_id) << "&symbol=" << UrlEncode(symbol) << "&operation=" << UrlEncode(operation) << "&volume=" << volume;
        std::string body = HttpRequest("GET", oss.str(), api_key, terminal_id);
        std::string ord = ExtractNumber(body, "order");
        if (ord.empty()) ord = ExtractNumber(body, "ticket");
        return ord.empty() ? 0 : std::stoull(ord);
    }

    std::vector<PositionInfo> OpenedOrders(const std::string& terminal_id, const std::string& api_key = "TRIAL") {
        std::string path = "/OpenedOrders?id=" + UrlEncode(terminal_id);
        std::string body = HttpRequest("GET", path, api_key, terminal_id);
        return ParsePositions(body);
    }

    static std::vector<PositionInfo> ParsePositions(const std::string& body) {
        ValidateResponse(body);
        std::vector<PositionInfo> list;
        auto data = nlohmann::json::parse(body).at("data");
        const auto& rows = data.at("positionInfos");
        if (rows.is_null()) return list;
        for (const auto& row : rows) {
            PositionInfo pi;
            const auto& ticket = row.at("ticket");
            pi.ticket = ticket.is_string() ? std::stoull(ticket.get<std::string>()) : ticket.get<uint64_t>();
            pi.symbol = row.at("symbol").get<std::string>();
            pi.volume = row.at("volume").get<double>();
            pi.type = row.at("type").get<std::string>();
            list.push_back(pi);
        }
        return list;
    }

    std::string OrderClose(const std::string& terminal_id, uint64_t ticket, const std::string& api_key = "TRIAL") {
        std::ostringstream oss;
        oss << "/OrderClose?id=" << UrlEncode(terminal_id) << "&ticket=" << ticket << "&volume=0&slippage=20";
        return HttpRequest("GET", oss.str(), api_key, terminal_id);
    }

private:
    std::string host_;

    static std::string UrlEncode(const std::string& str) {
        std::ostringstream escaped;
        for (char c : str) {
            if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c);
                escaped << buf;
            }
        }
        return escaped.str();
    }

    static std::string ExtractString(const std::string& body, const std::string& key) {
        auto root = nlohmann::json::parse(body);
        const auto& data = root.contains("data") && root["data"].is_object() ? root["data"] : root;
        if (!data.contains(key) || data[key].is_null()) return "";
        return data[key].is_string() ? data[key].get<std::string>() : data[key].dump();
    }
    static std::string ExtractNumber(const std::string& body, const std::string& key) { return ExtractString(body, key); }

public:
    // Shared by all terminal helpers, including HTTP 200 business errors.
    static void ValidateResponse(const std::string& body) {
        auto root = nlohmann::json::parse(body);
        if (!root.is_object()) throw std::runtime_error("Invalid MRPC response");
        if (root.contains("error") && !root["error"].is_null() && root["error"] != false) {
            const auto& error = root["error"];
            auto code = error.is_object() ? error.value("errorCode", std::string("API_ERROR")) : "API_ERROR";
            throw std::runtime_error("MRPC request failed: " + code);
        }
    }
private:
    std::string HttpRequest(const std::string& method, const std::string& path, const std::string& api_key, const std::string& id_header = "", bool delete_header = false) {
        HINTERNET hSession = WinHttpOpen(L"CppCopier/1.0.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) throw std::runtime_error("Cannot initialize WinHTTP");

        std::wstring wHost(host_.begin(), host_.end());
        HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            throw std::runtime_error("Cannot initialize HTTPS request");
        }

        std::wstring wMethod(method.begin(), method.end());
        std::wstring wPath(path.begin(), path.end());
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, wMethod.c_str(), wPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            throw std::runtime_error("Cannot initialize HTTPS request");
        }

        std::wstring headers = L"APIKey: " + std::wstring(api_key.begin(), api_key.end()) + L"\r\nUser-Agent: CppCopier/1.0.0\r\n";
        if (!id_header.empty()) {
            headers += L"id: " + std::wstring(id_header.begin(), id_header.end()) + L"\r\n";
        }
        if (delete_header) {
            headers += L"delete: true\r\n";
        }
        WinHttpAddRequestHeaders(hRequest, headers.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

        WinHttpSetTimeouts(hRequest, 30000, 30000, 120000, 120000);

        std::string response;
        DWORD status = 0, statusSize = sizeof(status);
        bool complete = false;
        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX);
            DWORD bytesRead = 0;
            char buffer[4096];
            while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead)) {
                if (!bytesRead) { complete = true; break; }
                response.append(buffer, bytesRead);
            }
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        if (!complete) throw std::runtime_error("HTTPS request failed or response was truncated");
        if (status < 200 || status >= 300) throw std::runtime_error("MRPC HTTP " + std::to_string(status));
        ValidateResponse(response);
        return response;
    }
};

} // namespace copier
