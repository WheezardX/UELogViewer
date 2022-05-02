#include "LogTab.h"
#include "LogViewer.h"
#include "imgui.h"
#include <string.h>


LogTab::LogTab(LogViewer* Parent, const std::string& TabName):
mParent(Parent),
mTabName(TabName)
{
    strcpy_s(mTabNameGui, sizeof mTabNameGui, mTabName.c_str());
    mInclusiveCategoriesGui[0] = 0;
    mExclusiveCategoriesGui[0] = 0;
}

void LogTab::NewFrame()
{
    if (ImGui::BeginTabItem(mTabName.c_str(), &mIsOpen, ImGuiTabItemFlags_None))
    {
        if (ImGui::TreeNode("Tab Settings"))
        {
            if (ImGui::InputText("TabName", mTabNameGui, kTabNameLength, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                mTabName = mTabNameGui;
            }

            if (ImGui::InputText("Inclusive Categories", mInclusiveCategoriesGui, kCategoriesLength, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                mInclusiveCategories = mInclusiveCategoriesGui;
            }

            if (ImGui::InputText("Exclusive Categories", mExclusiveCategoriesGui, kCategoriesLength, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                mExclusiveCategories = mExclusiveCategoriesGui;
            }

            ImGui::Separator();
            ImGui::TreePop();
        }


        ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetWindowHeight() - 100);
        ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

        char LogLine[2048];
        mParent->mLogBuffer.clear();
        mParent->mLogBuffer.seekg(0);
        mParent->mLogBuffer.seekp(0);

        while (mParent->mLogBuffer.getline(LogLine, 2048, '\n'))
        {
            //ImGui::TextColored(ImVec4(1, 1, 0, 1), LogLine);
            ProcessLine(LogLine);
        }

        float scroll_x = ImGui::GetScrollX();
        float scroll_max_x = ImGui::GetScrollMaxX();
        ImGui::EndChild();


        ImGui::EndTabItem();
    }

}

std::string FindCategory(const char* LogLine)
{
    static std::string ProcessingLine;
    ProcessingLine = LogLine;
    std::string Category;
    if (LogLine[0] == '[')
    {
        // Full normal Log line.
        // Ex. [2022.04.20-01.16.24:118][  0]LogConfig: Setting CVar [[r.TemporalAASamples:4]]
        size_t EndIndex = ProcessingLine.find(":");    // This finds the 1st one in the data.
        EndIndex = ProcessingLine.find(":", EndIndex + 1); // Ideally this find the one at the end of the category.
        if (EndIndex != std::string::npos)
        {
            size_t StartIndex = ProcessingLine.rfind("]", EndIndex);
            if (StartIndex != std::string::npos)
            {
                StartIndex++;
                Category = ProcessingLine.substr(StartIndex, EndIndex - StartIndex);
            }
        }
    }
    else
    {
        // Early lines don't have the timestamp.
        // Ex. LogPluginManager: Mounting plugin Bridge
        size_t FoundIndex = ProcessingLine.find(":");
        if (FoundIndex != std::string::npos)
        {
            Category = ProcessingLine.substr(0, FoundIndex);
        }
    }
    return Category;
}

static inline void CreateLogLine(const ImVec4& color, const char* LogLine, const char* Category)
{
    static std::string sContextLine;
    const bool ShouldOpenConext = sContextLine.size() == 0 || sContextLine == LogLine;

    ImGui::TextColored(color, LogLine);
    if (ShouldOpenConext && ImGui::BeginPopupContextItem("Line Context"))
    {
        sContextLine = LogLine;
        if (ImGui::Button("Copy"))
        {
            ImGui::SetClipboardText(LogLine);
            sContextLine.clear();
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::Button("Copy Category"))
        {
            if (Category == nullptr)
            {
                std::string Cat = FindCategory(LogLine);
                ImGui::SetClipboardText(Cat.c_str());
            }
            else
            {
                ImGui::SetClipboardText(Category);
            }
            sContextLine.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void LogTab::ProcessLine(const char* LogLine)
{
    if (mInclusiveCategories.size() == 0 && mExclusiveCategories.size() == 0)
    {
        // Fast Path
        CreateLogLine(ImVec4(1, 1, 1, 1), LogLine, nullptr);
    }
    else
    {
        std::string Category = FindCategory(LogLine);
        if (ShouldShowCategory(Category))
        {
            CreateLogLine(ImVec4(1, 1, 1, 1), LogLine, Category.c_str());
        }

    }
}

bool LogTab::ShouldShowCategory(const std::string& Category)
{
    if (mInclusiveCategories.size() > 0)
    {
        size_t FoundIndex = mInclusiveCategories.find(Category);
        return FoundIndex != std::string::npos;
    }
    else if (mExclusiveCategories.size() > 0)
    {
        size_t FoundIndex = mExclusiveCategories.find(Category);
        return FoundIndex == std::string::npos;
    }
    return true;
}
