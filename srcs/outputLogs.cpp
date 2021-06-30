#include "Webserv.hpp"

// Get current date/time in this format YYYY-MM-DD HH:mm:ss
static std::string const currentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];

	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

void outputLogs(std::string logs)
{
	static bool isNotOpen;
	if (isNotOpen)
		return;
	std::fstream logsFile(LOGS_FILE, std::ios::app);
	if (!logsFile.is_open())
	{
		isNotOpen = true;
		log "Could NOT open the logs file" line;
		return;
	}
	std::ifstream logsFileIn(LOGS_FILE, std::ios::in);
	std::string buff;
	std::getline(logsFileIn, buff);
	logsFileIn.close();
	if (!buff.empty())
		while (logs.front() == '\n')
		{
			logsFile << std::endl;
			logs.erase(0, 1);
		}
	else
		while (logs.front() == '\n')
			logs.erase(0, 1);
	logsFile << "[" << currentDateTime() << "]: " << logs << std::endl;
	logsFile.close();
}
