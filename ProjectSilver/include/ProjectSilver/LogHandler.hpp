#pragma once

#include <string>

namespace Game {
	class LogHandler {
	public:
		enum class MessageType {
			Info,
			Warn,
			Error,
			Debug
		};
	private:
		static bool hasStartedActionLog;

		static void LogInfoToCrashLog(std::ofstream& errorLog);
	public:
		static void Log(std::string message, MessageType type);
	};
}
