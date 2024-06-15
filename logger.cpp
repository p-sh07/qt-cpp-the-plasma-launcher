#include "logger.h"

#include <fstream>
#include <time.h>

using namespace std::literals;

_log::FileLogger::FileLogger(std::ofstream& out)
    : out_(out) {
}

void _log::FileLogger::LogMsg(std::string msg) const {
    std::time_t now = std::time(NULL);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%H:%M:%S", ptm);

    out_ << "["s << buffer << "]: "s;
    out_ << msg << "\n"s;
}

const _log::FileLogger& _log::operator<<(const _log::FileLogger& fl, const std::string msg) {
    fl.LogMsg(msg);
    return fl;
}
