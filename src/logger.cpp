#include "logger.h";

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

Logger::Logger(const std::string& log_file_path)
  : log_file_path_(log_file_path), current_date_(getCurrentDate()) {
    
    filesystem::create_directories(filesystem::path(log_file_path_).parent_path());
    loadExistingLogs();
    cleanupOldLogs();

  };

Logger::~Logger() {
  flush();
}

