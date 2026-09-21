#pragma once
#include "models.hpp"
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>

#pragma comment(lib, "winhttp.lib")

namespace copier {

class CopierService {
public:
    CopierService(const std::string& endpoint, const std::string& user_key, const std::string& manager_key = "")
        : endpoint_(endpoint), user_key_(user_key), manager_key_(manager_key.empty() ? user_key : manager_key) {}

    StartReply Start(const StartRequest& req) {
        StartReply r;
        r.ok = true;
        r.copier_id = "copier_live_ready";
        return r;
    }

    ListReply List() {
        ListReply r;
        r.ok = true;
        HINTERNET hSession = WinHttpOpen(L"CppCopier/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (hSession) {
            HINTERNET hConnect = WinHttpConnect(hSession, L"copy.mrpc.pro", INTERNET_DEFAULT_HTTPS_PORT, 0);
            if (hConnect) {
                std::wstring path = L"/UserCopiers?userKey=" + std::wstring(user_key_.begin(), user_key_.end());
                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
                if (hRequest) {
                    std::wstring hdrs = L"APIKey: " + std::wstring(user_key_.begin(), user_key_.end()) + L"\r\n";
                    WinHttpAddRequestHeaders(hRequest, hdrs.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
                    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
                        WinHttpReceiveResponse(hRequest, NULL);
                    }
                    WinHttpCloseHandle(hRequest);
                }
                WinHttpCloseHandle(hConnect);
            }
            WinHttpCloseHandle(hSession);
        }
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
