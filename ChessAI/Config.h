
#include<string>
#include<vector>
#include"EngineConfig.h"

class Config {
public:
	static std::string g_ip;
	static int g_port;
	static std::vector<EngineConfig> engineConfigList;
	static std::string version;
};




