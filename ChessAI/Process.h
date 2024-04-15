#pragma once
#include<string>
#include<vector>
#include<atlconv.h>
#include"Utils.h"
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
	void exit();
};

