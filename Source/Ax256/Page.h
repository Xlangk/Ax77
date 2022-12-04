#include <map>
#include <string>

namespace Ax256 {
class Page {
    std::map<std::string, std::string> m_data;

public:
    Page(int mb);

    std::string read(const std::string &address);
    std::string read(uint64_t address);
};
}
