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

        for (const LogLine& LogLine : mParent->mLogLines)
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

void LogTab::ProcessLine(const LogLine& LogLine)
{
    static std::string sContextLine;

    if (ShouldShowCategory(LogLine.Category) == false)
    {
        return;
    }

    const bool ShouldOpenConext = sContextLine.size() == 0 || sContextLine == LogLine.LineText;
    ImGui::PushStyleColor(ImGuiCol_Text, LogLine.Color);
    ImGui::Text(LogLine.LineText.c_str());
    ImGui::PopStyleColor();
    if (ShouldOpenConext && ImGui::BeginPopupContextItem("Line Context"))
    {
        sContextLine = LogLine.LineText;
        if (ImGui::Button("Copy"))
        {
            ImGui::SetClipboardText(LogLine.LineText.c_str());
            sContextLine.clear();
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::Button("Copy Category"))
        {
            ImGui::SetClipboardText(LogLine.Category.c_str());
            sContextLine.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
