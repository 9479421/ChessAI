#include"Config.h"

std::string Config::g_ip = "192.168.89.136";
int Config::g_port = 9000;
std::vector<EngineConfig> Config::engineConfigList;
std::string Config::version = "1.1";

//全局配置
int Config::centerX;
int Config::topcenterY;
int Config::bottomcenterY;
int Config::marginX;
int Config::marginY;
int Config::chessWidth;
int Config::chessHeight;

bool Config::yunkuStatus;
bool Config::openBookStatus;
std::string Config::openBookPath;
int Config::steps;

