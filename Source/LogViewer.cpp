#include "LogViewer.h"
#include <fstream>
#include <streambuf>



static void ProcessLine(const char* ReadLine, LogLine* LogLine)
{
    static std::string ProcessingLine; // Reuse memory

    if (LogLine == nullptr || ReadLine == nullptr)
    {
        return;
    }

    ProcessingLine = ReadLine;
    std::string Category;
    if (ReadLine[0] == '[')
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
                LogLine->Category = ProcessingLine.substr(StartIndex, EndIndex - StartIndex);
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
            LogLine->Category = ProcessingLine.substr(0, FoundIndex);
        }
    }

    size_t FoundIndex = ProcessingLine.find("Warning");
    if (FoundIndex != std::string::npos)
    {
        LogLine->Severity = "Warning";
        LogLine->Color = ImVec4(1, 1, 0, 1);
    }
    else if (ProcessingLine.find("Error") != std::string::npos)
    {
        LogLine->Severity = "Error";
        LogLine->Color = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
    }
    else
    {
        LogLine->Severity = "Unknown";  // Note there are others but we don't really care about info vs. display, etc.
        LogLine->Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}





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

        char CurrLogLine[2048];
        mLogBuffer.clear(); // Clears flags.
        mLogBuffer.seekg(0);
        mLogBuffer.seekp(0);

        mLogLines.clear();
        int Index = 0;
        while (mLogBuffer.getline(CurrLogLine, 2048, '\n'))
        {
            LogLine NewLogLine;
            NewLogLine.LineNumber = Index++;
            NewLogLine.LineText = CurrLogLine;
            ProcessLine(CurrLogLine, &NewLogLine);
            mLogLines.emplace_back(NewLogLine);
        }

    }
}
