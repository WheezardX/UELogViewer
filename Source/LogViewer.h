#pragma once

#include "LogTab.h"
#include <sstream>
#include <vector>
#include <string>


class LogViewer
{
public:
    LogViewer();

    void NewFrame();
    void LoadLog(const std::string& Filepath);

    std::stringstream mLogBuffer;

private:
    std::vector<LogTab> mTabs;
    std::string mFilepath;
};