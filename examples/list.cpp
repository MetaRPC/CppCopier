#include <copier/copier.hpp>
#include <cstdlib>
#include <iostream>
int main() {
    const char* key = std::getenv("MRPC_API_KEY");
    if (!key || !*key) { std::cerr << "Set MRPC_API_KEY\n"; return 1; }
    copier::CopierService service("copy.mrpc.pro:443", key);
    auto result = service.List();
    if (!result.ok) { std::cerr << result.error << "\n"; return 1; }
    for (const auto& item : result.copiers) std::cout << item.id << ": paused=" << item.paused << "\n";
}
