#include "jsonio/exceptions.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace jsonio {

// Thread-safe logger to stdout with colors
std::shared_ptr<spdlog::logger> io_logger = spdlog::stdout_color_mt("jsonio");

void JSONIO_THROW(const std::string &title, int id, const std::string &message)
{
    io_logger->error("{}.{}: {}", title, id, message);
    throw jsonio_exception::create(title, id, message);
}

void JSONIO_THROW_IF(bool error, const std::string &title, int id, const std::string &message)
{
    if(error) {
        io_logger->error("{}.{}: {}", title, id, message);
        throw jsonio_exception::create(title, id, message);
    }
}

} // namespace jsonio
