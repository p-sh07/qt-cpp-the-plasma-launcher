#include "logger.h"

#include <time.h>

using namespace std::literals;

_log::FileLogger::FileLogger(std::filesystem::path log_path)
    : log_path_(log_path) {
}

void _log::FileLogger::ClearLog() {
    //Trunc -> will erase previous contents of file
    std::ofstream out(log_path_, std::ios_base::trunc);
    out.close();
}

void _log::FileLogger::LogMsg(std::string msg) const {
    //Append
    std::ofstream out(log_path_, std::ios_base::app);

    std::time_t now = std::time(NULL);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%H:%M:%S", ptm);

    out << "["s << buffer << "]: "s;
    out << msg << "\n"s;

    out.close();
}

std::shared_ptr<_log::FileLogger> operator<<(const std::shared_ptr<_log::FileLogger>& fl, const std::string msg) {
    fl->LogMsg(msg);
    return fl;
}
