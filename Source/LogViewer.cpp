#include "LogViewer.h"
#include "imgui.h"
#include <fstream>
#include <streambuf>


LogViewer::LogViewer()
{
    mTabs.push_back(LogTab(this, "OGTab"));
    mFilepath = "UELogViewer";
}

void LogViewer::NewFrame()
{
    ImGuiIO& IO = ImGui::GetIO();
    ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
    ImVec2 WindowSize(IO.DisplaySize.x - 100, IO.DisplaySize.y - 100);
    ImGui::SetNextWindowPos(Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(WindowSize, ImGuiCond_FirstUseEver);

    ImGui::Begin(mFilepath.c_str());

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            //if (ImGui::MenuItem("Open", "Ctrl+O")) 
            if (ImGui::Button("Open")) 
            {
                if (!ImGui::IsPopupOpen("Open Logfile..."))
                {
                    ImGui::OpenPopup("Open Logfile...");
                }
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(500, 1980), ImGuiCond_FirstUseEver);

            if (ImGui::BeginPopupModal("Open Logfile...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static const int kFilepathLength = 512;
                static char Filepath[kFilepathLength];
                if (ImGui::InputText("Filepath", Filepath, kFilepathLength, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    LoadLog(Filepath);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            static bool show_demo_window = false;
            ImGui::Checkbox("Show Demo Window", &show_demo_window);
            if (show_demo_window)
            {
                ImGui::ShowDemoWindow(&show_demo_window);
            }

            ImGui::EndMenu();
        }

    }
    ImGui::EndMainMenuBar();

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        for (int Index = 0; Index < mTabs.size(); ++Index)
        {
            LogTab& Tab = mTabs[Index];
            Tab.NewFrame();

            if (Tab.mIsOpen == false)
            {
                mTabs.erase(mTabs.begin() + Index);
                Index--;
            }
        }
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
        {
            mTabs.push_back(LogTab(this, "New Tab"));
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void LogViewer::LoadLog(const std::string& Filepath)
{
    mFilepath = Filepath;
    if (mFilepath.size() == 0)
    {
        mFilepath = "Empty";
        mLogBuffer.str(std::string());
    }
    else
    {
        std::ifstream fs(Filepath);
        mLogBuffer << fs.rdbuf();
    }
}
