#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <regex>
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
        std::string path = "/DemoAccount/Open?server=" + server;
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

    DisconnectReply Disconnect(const std::string& terminal_id, const std::string& api_key = "TRIAL") {
        std::string body = HttpRequest("GET", "/Disconnect", api_key, terminal_id);

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
        std::vector<PositionInfo> list;
        std::regex re("\"ticket\"\\s*:\\s*([0-9]+)");
        auto words_begin = std::sregex_iterator(body.begin(), body.end(), re);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            PositionInfo pi;
            pi.ticket = std::stoull(match[1].str());
            pi.symbol = ExtractString(body, "symbol");
            pi.type = ExtractString(body, "type");
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

    static std::string ExtractString(const std::string& json, const std::string& key) {
        std::regex re("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
        std::smatch m;
        if (std::regex_search(json, m, re) && m.size() > 1) {
            return m[1].str();
        }
        return "";
    }

    static std::string ExtractNumber(const std::string& json, const std::string& key) {
        std::regex re("\"" + key + "\"\\s*:\\s*([0-9]+)");
        std::smatch m;
        if (std::regex_search(json, m, re) && m.size() > 1) {
            return m[1].str();
        }
        return "";
    }

    std::string HttpRequest(const std::string& method, const std::string& path, const std::string& api_key, const std::string& id_header = "") {
        HINTERNET hSession = WinHttpOpen(L"CppCopier/1.0.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return "";

        std::wstring wHost(host_.begin(), host_.end());
        HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "";
        }

        std::wstring wMethod(method.begin(), method.end());
        std::wstring wPath(path.begin(), path.end());
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, wMethod.c_str(), wPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "";
        }

        std::wstring headers = L"APIKey: " + std::wstring(api_key.begin(), api_key.end()) + L"\r\nUser-Agent: CppCopier/1.0.0\r\n";
        if (!id_header.empty()) {
            headers += L"id: " + std::wstring(id_header.begin(), id_header.end()) + L"\r\n";
        }
        WinHttpAddRequestHeaders(hRequest, headers.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

        WinHttpSetTimeouts(hRequest, 30000, 30000, 120000, 120000);

        std::string response;
        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD bytesRead = 0;
            char buffer[4096];
            while (WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                response.append(buffer, bytesRead);
            }
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return response;
    }
};

} // namespace copier
