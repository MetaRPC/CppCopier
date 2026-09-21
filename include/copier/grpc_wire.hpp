#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

#pragma comment(lib, "winhttp.lib")

namespace copier {

inline void write_varint(std::vector<uint8_t>& buf, uint64_t val) {
    while ((val & ~0x7FULL) != 0) {
        buf.push_back(static_cast<uint8_t>((val & 0x7F) | 0x80));
        val >>= 7;
    }
    buf.push_back(static_cast<uint8_t>(val & 0x7F));
}

inline void write_tag(std::vector<uint8_t>& buf, int field_num, int wire_type) {
    write_varint(buf, (static_cast<uint64_t>(field_num) << 3) | static_cast<uint64_t>(wire_type));
}

inline void write_string(std::vector<uint8_t>& buf, int field_num, const std::string& str) {
    if (str.empty()) return;
    write_tag(buf, field_num, 2);
    write_varint(buf, str.size());
    buf.insert(buf.end(), str.begin(), str.end());
}

inline void write_uint64(std::vector<uint8_t>& buf, int field_num, uint64_t val) {
    if (val == 0) return;
    write_tag(buf, field_num, 0);
    write_varint(buf, val);
}

inline void write_bool(std::vector<uint8_t>& buf, int field_num, bool val) {
    if (!val) return;
    write_tag(buf, field_num, 0);
    write_varint(buf, 1);
}

inline void write_message(std::vector<uint8_t>& buf, int field_num, const std::vector<uint8_t>& msg) {
    if (msg.empty()) return;
    write_tag(buf, field_num, 2);
    write_varint(buf, msg.size());
    buf.insert(buf.end(), msg.begin(), msg.end());
}

inline bool read_varint(const uint8_t*& ptr, const uint8_t* end, uint64_t& val) {
    val = 0;
    int shift = 0;
    while (ptr < end && shift < 64) {
        uint8_t b = *ptr++;
        val |= (static_cast<uint64_t>(b & 0x7F) << shift);
        if ((b & 0x80) == 0) return true;
        shift += 7;
    }
    return false;
}

inline bool skip_field(const uint8_t*& ptr, const uint8_t* end, int wire_type) {
    if (wire_type == 0) {
        uint64_t dummy;
        return read_varint(ptr, end, dummy);
    } else if (wire_type == 1) {
        if (end - ptr < 8) return false;
        ptr += 8;
        return true;
    } else if (wire_type == 2) {
        uint64_t len = 0;
        if (!read_varint(ptr, end, len)) return false;
        if (end - ptr < static_cast<ptrdiff_t>(len)) return false;
        ptr += len;
        return true;
    } else if (wire_type == 5) {
        if (end - ptr < 4) return false;
        ptr += 4;
        return true;
    }
    return false;
}

inline bool call_grpc_http2(const std::string& host, int port, const std::string& path,
                           const std::vector<uint8_t>& req_payload, std::vector<uint8_t>& resp_payload,
                           std::string& err_msg) {
    HINTERNET hSession = WinHttpOpen(L"CppCopier-gRPC/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        err_msg = "WinHttpOpen failed: " + std::to_string(GetLastError());
        return false;
    }

    DWORD http2_opt = WINHTTP_PROTOCOL_FLAG_HTTP2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &http2_opt, sizeof(http2_opt));

    std::wstring whost(host.begin(), host.end());
    HINTERNET hConnect = WinHttpConnect(hSession, whost.c_str(), static_cast<INTERNET_PORT>(port), 0);
    if (!hConnect) {
        err_msg = "WinHttpConnect failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hSession);
        return false;
    }

    std::wstring wpath(path.begin(), path.end());
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wpath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        err_msg = "WinHttpOpenRequest failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    WinHttpSetOption(hRequest, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &http2_opt, sizeof(http2_opt));
    WinHttpSetTimeouts(hRequest, 30000, 30000, 180000, 180000);

    std::wstring headers = L"Content-Type: application/grpc\r\nTE: trailers\r\n";
    WinHttpAddRequestHeaders(hRequest, headers.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

    // Frame: 5-byte header + req_payload
    std::vector<uint8_t> frame(5 + req_payload.size());
    frame[0] = 0; // Uncompressed
    uint32_t len = static_cast<uint32_t>(req_payload.size());
    frame[1] = static_cast<uint8_t>((len >> 24) & 0xFF);
    frame[2] = static_cast<uint8_t>((len >> 16) & 0xFF);
    frame[3] = static_cast<uint8_t>((len >> 8) & 0xFF);
    frame[4] = static_cast<uint8_t>(len & 0xFF);
    if (!req_payload.empty()) {
        std::copy(req_payload.begin(), req_payload.end(), frame.begin() + 5);
    }

    BOOL bSend = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                    frame.data(), static_cast<DWORD>(frame.size()), static_cast<DWORD>(frame.size()), 0);
    if (!bSend) {
        err_msg = "WinHttpSendRequest failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    BOOL bResp = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResp) {
        err_msg = "WinHttpReceiveResponse failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD status = 0;
    DWORD status_size = sizeof(status);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &status_size, WINHTTP_NO_HEADER_INDEX);
    if (status != 200) {
        err_msg = "gRPC HTTP status: " + std::to_string(status);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    std::vector<uint8_t> raw_resp;
    DWORD bytes_read = 0;
    uint8_t buf[4096];
    while (WinHttpReadData(hRequest, buf, sizeof(buf), &bytes_read) && bytes_read > 0) {
        raw_resp.insert(raw_resp.end(), buf, buf + bytes_read);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    // Unframe 5-byte header
    if (raw_resp.size() >= 5) {
        uint32_t payload_len = (static_cast<uint32_t>(raw_resp[1]) << 24) |
                               (static_cast<uint32_t>(raw_resp[2]) << 16) |
                               (static_cast<uint32_t>(raw_resp[3]) << 8) |
                               (static_cast<uint32_t>(raw_resp[4]));
        size_t actual_len = min(static_cast<size_t>(payload_len), raw_resp.size() - 5);
        resp_payload.assign(raw_resp.begin() + 5, raw_resp.begin() + 5 + actual_len);
    }
    return true;
}

} // namespace copier
