//  Created by Pavel Sh on 23.12.2023.
//

#pragma once

#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

//creates unique id for the logger variable:
#define UNIQUE_ID_IMPL(lineno) logger_no_##lineno
//Implementation layer for the concat to work
#define UNIQUE_ID(lineno) UNIQUE_ID_IMPL(lineno)
//Translates to: "logger_no_123{message}";
#define LOG_DURATION(message) LogDuration UNIQUE_ID(__LINE__){message};

#define LOG_MSG(message) LogMsg(__FILE__, __LINE__, message);

namespace _log {

const std::filesystem::path LOG_FILE_NAME = "_log.txt";

class Duration {
public:
    using Clock = std::chrono::steady_clock;
    
    Duration() {}
    
    explicit Duration(const std::string& message = "")
    : message_(message) {
    }

    ~Duration() {
        using namespace std::chrono;
        using namespace std::literals;
       
        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        if(!message_.empty()) {
            std::cerr << message_ << ": "s;
        }
        std::cerr << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
   }

private:
    const Clock::time_point start_time_ = Clock::now();
    std::string message_;
};

class FileLogger {
public:
    //needs a local copy of the path, no point in passing by ref
    FileLogger(std::filesystem::path log_path = LOG_FILE_NAME);

    //Clear the log file located at path log_path, if it exists
    void ClearLog();
    void LogMsg(std::string msg) const;
private:
    std::filesystem::path log_path_;
};

}//namespace log

std::shared_ptr<_log::FileLogger> operator<<(const std::shared_ptr<_log::FileLogger>& fl, const std::string msg);

