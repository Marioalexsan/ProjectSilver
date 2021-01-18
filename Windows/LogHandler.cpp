#include "PCHeader.h"

#include "LogHandler.h"
#include "ConfigHandler.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <ctime>

#include "Globals.h"

namespace Game {
    bool LogHandler::hasStartedActionLog = false;

    void LogHandler::LogInfoToCrashLog(std::ofstream& errorLog) {
        if (errorLog.good()) {
            errorLog << "Game Information: " << Globals::Version() << endl;
            errorLog << "Resolution: " << ConfigHandler::RetrieveItem("resolution") << endl;
            errorLog << "Window Mode: " << ConfigHandler::RetrieveItem("videomode") << endl;
        }
    }

	void LogHandler::Log(string message, MessageType type) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;

        tm someTime;
        localtime_s(&someTime, &in_time_t);

        ss << std::put_time(&someTime, "%Y-%m-%d--%H-%M-%S");

        if (!std::filesystem::exists("Logs")) {
            std::filesystem::create_directory("Logs");
        }
        
        string actionFileName = "Logs/ActionLog.txt";
        std::ofstream actionLog;
        if (!hasStartedActionLog) {
            actionLog.open(actionFileName);
        }
        else {
            actionLog.open(actionFileName, std::ofstream::app | std::ofstream::ate);
        }

        if (!actionLog.good()) {
            // Definitely a bad error case, but I don't think throwing is a good idea here
            return;
        }

        if (!hasStartedActionLog) {
            actionLog << "Game Version: " << Globals::Version() << endl;
        }
        hasStartedActionLog = true;

        const static map<MessageType, string> translation = {
            {MessageType::Info, "Info"},
            {MessageType::Warn, "Warn"},
            {MessageType::Error, "Error"},
            {MessageType::Debug, "Debug"}
        };

        actionLog << "[ " << ss.str() << " ] [ " + translation.at(type) + " ] " + message << std::endl;
        actionLog.close();

        if (type == MessageType::Error) {
            // Also create an error crash log which is a bit more persistent due to naming scheme
            string errorFileName = "Logs/CrashLog-" + ss.str() + ".txt";
            std::ofstream errorLog(errorFileName);
            if (!errorLog) {
                    string fatalError = "LogHandler was unable to log an Error. The error message was: " + message;
                    throw std::exception(fatalError.c_str());
                return;
            }
            LogInfoToCrashLog(errorLog);
            errorLog << message << std::endl;
            errorLog.close();
        }
	}
}