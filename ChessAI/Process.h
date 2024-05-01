#pragma once
#include<string>
#include<vector>
#include<atlconv.h>
#include"Utils.h"
#include<chrono>
class Process
{
private:
	//设置新进程参数
	PROCESS_INFORMATION pi = PROCESS_INFORMATION();
	HANDLE hOutRead, hOutWrite;
	HANDLE hInRead, hInWrite;

	std::vector<std::string> cmdLines;

public:
	void createProcess(std::string process);
	void addCmdLine(std::string cmdLine);
	std::string execute(std::string endFlag);
	void execute(std::string endFlag, std::string endCmd ,float maxWaitingTime, std::string& ret);
	void exit();
};

