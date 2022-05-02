#pragma once
#include <string>


class LogViewer;

class LogTab
{
public:
    LogTab(LogViewer* Parent, const std::string& TabName);

    void NewFrame();

    bool mIsOpen = true;

private:
    void ProcessLine(const char* LogLine);
    bool ShouldShowCategory(const std::string& Category);

    LogViewer* mParent;
    std::string mTabName;
    std::string mInclusiveCategories;
    std::string mExclusiveCategories;

    static const int kTabNameLength = 64;
    char mTabNameGui[kTabNameLength];

    static const int kCategoriesLength = 512;
    char mInclusiveCategoriesGui[kCategoriesLength];
    char mExclusiveCategoriesGui[kCategoriesLength];

};

