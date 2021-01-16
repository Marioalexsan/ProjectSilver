#ifndef ERRORLOGGING_HEADER
#define ERRORLOGGING_HEADER

#include <string>

namespace Game {
	class LogHandler {
	public:
		enum class MessageType {
			Info,
			Warn,
			Error
		};
	private:
		static bool hasStartedActionLog;
	public:
		static void Log(string message, MessageType type);
	};
}

#endif