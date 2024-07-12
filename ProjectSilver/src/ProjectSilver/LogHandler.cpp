#include <ProjectSilver/ConfigHandler.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/LogHandler.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace Game
{
    bool LogHandler::hasStartedActionLog = false;

    void LogHandler::LogInfoToCrashLog(std::ofstream& errorLog)
    {
        if (errorLog.good())
        {
            errorLog << "Game Information: " << Globals::Version() << std::endl;
            errorLog << "Resolution: " << ConfigHandler::RetrieveItem("resolution")
                     << std::endl;
            errorLog << "Window Mode: " << ConfigHandler::RetrieveItem("videomode")
                     << std::endl;
        }
    }

    void LogHandler::Log(std::string message, MessageType type)
    {
        auto now       = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;

        tm someTime;
        localtime_s(&someTime, &in_time_t);

        ss << std::put_time(&someTime, "%Y-%m-%d--%H-%M-%S");

        if (!std::filesystem::exists("Logs"))
        {
            std::filesystem::create_directory("Logs");
        }

        std::string   actionFileName = "Logs/ActionLog.txt";
        std::ofstream actionLog;
        if (!hasStartedActionLog)
        {
            actionLog.open(actionFileName);
        }
        else
        {
            actionLog.open(actionFileName, std::ofstream::app | std::ofstream::ate);
        }

        if (!actionLog.good())
        {
            // Definitely a bad error case, but I don't think throwing is a good idea here
            return;
        }

        if (!hasStartedActionLog)
        {
            actionLog << "Game Version: " << Globals::Version() << std::endl;
        }
        hasStartedActionLog = true;

        const static std::map<MessageType, std::string> translation =
            {{MessageType::Info, "Info"},
             {MessageType::Warn, "Warn"},
             {MessageType::Error, "Error"},
             {MessageType::Debug, "Debug"}};

        actionLog << "[ " << ss.str() << " ] [ " + translation.at(type) + " ] " + message
                  << std::endl;
        actionLog.close();

        if (type == MessageType::Error)
        {
            // Also create an error crash log which is a bit more persistent due to naming scheme
            std::string   errorFileName = "Logs/CrashLog-" + ss.str() + ".txt";
            std::ofstream errorLog(errorFileName);
            if (!errorLog)
            {
                std::string fatalError =
                    "LogHandler was unable to log an Error. The error message was: " + message;
                throw std::exception(fatalError.c_str());
                return;
            }
            LogInfoToCrashLog(errorLog);
            errorLog << message << std::endl;
            errorLog.close();
        }
    }
} // namespace Game