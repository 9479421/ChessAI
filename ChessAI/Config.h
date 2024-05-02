
#include<string>
#include<vector>
#include"EngineConfig.h"

class Config {
public:
	static std::string g_ip;
	static int g_port;
	static std::vector<EngineConfig> engineConfigList;
	static std::string version;

	static int centerX;
	static int topcenterY;
	static int bottomcenterY;
	static int marginX;
	static int marginY;
	static int chessWidth;
	static int chessHeight;
};




