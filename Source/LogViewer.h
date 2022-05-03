#pragma once

#include "LogTab.h"
#include "imgui.h"
#include <sstream>
#include <vector>
#include <string>


struct LogLine
{
    std::string Category;
    std::string Severity;
    std::string LineText;
    ImVec4 Color;
    int LineNumber;
};


class LogViewer
{
public:
    LogViewer();

    void NewFrame();
    void LoadLog(const std::string& Filepath);

    std::vector<LogLine> mLogLines;

private:
    std::vector<LogTab> mTabs;
    std::string mFilepath;
    std::stringstream mLogBuffer;
};