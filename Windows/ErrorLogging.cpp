#include "PCHeader.h"

#include "ErrorLogging.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <ctime>

#include "Globals.h"

namespace Game {
	namespace Logger {
		void LogError(string message) {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;

            tm someTime;
            localtime_s(&someTime, &in_time_t);

            ss << std::put_time(&someTime, "%Y-%m-%d--%H-%M-%S");

            if (!std::filesystem::exists("Logs")) {
                std::filesystem::create_directory("Logs");
            }

            string fileName = "Logs/CrashLog-" + ss.str() + ".txt";

            if (!std::filesystem::exists("Logs")) {
                std::filesystem::create_directory("Logs");
            }

            std::ofstream errorLog(fileName);
            if (!errorLog) {
                string fatalError = "ErrorLog was unable to create a log file. The error message was: " + message;
                throw std::exception(fatalError.c_str());
            }
            errorLog << "Game Version: " << Globals::Version() << endl;
            errorLog << message << std::endl;
            errorLog.close();
		}

	}
}