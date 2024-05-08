#include "logger.h"

#include <fstream>
#include <time.h>

log::FileLogger::FileLogger(const std::filesystem::path& file)
    : of_(file, std::ios_base::app) {
}

void log::FileLogger::LogMsg(std::string msg) const {
    std::ofstream out(LOG_FILE_PATH, std::ios_base::app);

    std::time_t now = std::time(NULL);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%H:%M:%S", ptm);

    out << "[" << buffer << "]:  ";
    out << msg << "\n";

    out.close();
}

const log::FileLogger& log::operator<<(const log::FileLogger& fl, const std::string msg) {
    fl.LogMsg(msg);
    return fl;
}
