
#include<string>
#include<vector>
#include"EngineConfig.h"

class Config {
public:
	//服务器配置
	static std::string g_ip;
	static int g_port;
	static std::string version;
	//引擎配置
	static std::vector<EngineConfig> engineConfigList;
	//棋盘定位信息
	static int centerX;
	static int topcenterY;
	static int bottomcenterY;
	static int marginX;
	static int marginY;
	static int chessWidth;
	static int chessHeight;
	//开局库配置
	static std::string openBookPath;
	static bool yunkuStatus;
	static bool openBookStatus;
	static int steps; //几步后脱离开局库
};




