#pragma once
#include<string>
#include"http.h"
#include"Utils.h"
#include"Process.h"
#include "stepIdx.h"

class Engine
{
public:
	std::string enginePath;
	std::string name;
	std::string author;

private:
	Process process;


public:
	Engine();
	Engine(std::string enginePath);

	void open(std::string enginePath);

	static int getNumsByRowFlag(std::string rowFlag);
	static stepIdx getStepIdx(std::string step, std::string fen);

	std::pair<std::string, std::string> calcStep(std::string fen);
	void uci();
	std::pair<std::string, std::string> calcStep(std::string fen, float time, int depth, std::string& ret);
	
};

class Engine_yunku : public Engine
{
public :
	std::string calcStep(std::string fen);
};
