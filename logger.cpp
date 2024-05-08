#include "logger.h"

#include <fstream>
#include <time.h>

_log::FileLogger::FileLogger(const std::filesystem::path& file)
    : log_file_(file) {
}

void _log::FileLogger::LogMsg(std::string msg) const {
    std::ofstream out{log_file_, std::ios_base::app};

    std::time_t now = std::time(NULL);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%H:%M:%S", ptm);

    out << "[" << buffer << "]:  ";
    out << msg << "\n";

    out.close();
}

const _log::FileLogger& _log::operator<<(const _log::FileLogger& fl, const std::string msg) {
    fl.LogMsg(msg);
    return fl;
}
