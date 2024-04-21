#include "Engine.h"

#include"stepIdx.h"

std::string Engine_yunku::calcStep(std::string fen)

	{
		http http;
		http.open("http://www.chessdb.cn/chessdb.php?action=queryall&showall=1&board=" + Utils::ReplaceAllText(fen, " ", "%20"));
		std::string body = http.get();

		printf("body: %s\n", body.c_str());

		int idx = body.find("move:") + 5;
		std::string runStep = body.substr(idx, 4);

		return runStep;
	}

Engine::Engine()
{
}

Engine::Engine(std::string enginePath)
{
	this->enginePath = enginePath;
	process.createProcess(this->enginePath);
}

void Engine::open(std::string enginePath)
{
	this->enginePath = enginePath;
	process.createProcess(this->enginePath);
}

int Engine::getNumsByRowFlag(std::string rowFlag)
{
	std::string rowFlags[9] = { "a","b","c","d","e","f","g","h","i" };
	for (int i = 0; i < 9; i++)
	{
		if (rowFlags[i].compare(rowFlag) == 0) {
			return i;
		}
	}
	return -1;
}

stepIdx Engine::getStepIdx(std::string step, std::string fen)
{
	stepIdx stepIdx;

	boolean isRed;
	if (fen.find("w") != std::string::npos)
	{
		//红
		isRed = true;
	}
	else {
		isRed = false;
	}

	int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;
	try {


		if (step.size() == 4)
		{
			std::string s1 = step.substr(0, 1);
			std::string s2 = step.substr(1, 1);
			std::string s3 = step.substr(2, 1);
			std::string s4 = step.substr(3, 1);
			row_begin = getNumsByRowFlag(s1);
			col_begin = atoi(s2.c_str());
			row_end = getNumsByRowFlag(s3);
			col_end = atoi(s4.c_str());
			if (isRed)
			{
				stepIdx.set(9 - col_begin, row_begin, 9 - col_end, row_end);
			}
			else {
				stepIdx.set(col_begin, 8 - row_begin, col_end, 8 - row_end);
			}

			printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
			stepIdx.print();
		}
	}
	catch (std::exception e) {

	}
	return stepIdx;
}

std::pair<std::string, std::string> Engine::calcStep(std::string fen)
{
	process.addCmdLine("position fen " + fen);
	process.addCmdLine("go depth 16");

	std::string runStep;
	std::string result;
	try {
		result = process.execute("bestmove");
		if (result.empty())
		{
			process.exit();
			process.createProcess(this->enginePath);

			goto end;
		}

		printf("result:%s\n", result.c_str());
		int idx1 = result.find("bestmove"); //报错
		int idx2 = result.find("ponder");
		if (idx1 != std::string::npos && idx2 == std::string::npos)
		{
			//最后一步绝杀
			runStep = result.substr(idx1 + 9, 4);
		}
		else {
			runStep = result.substr(idx1 + 9, idx2 - 1 - (idx1 + 9));
		}
	}
	catch (std::exception e) {

	}
end:

	return std::make_pair(runStep, result);
}

void Engine::uci()
{
	process.addCmdLine("uci");


	std::string runStep;

	std::string result = process.execute("uciok");

	int idx1, idx2;
	idx1 = result.find("id name ") + 8;
	idx2 = result.find("\r\n", idx1);
	if (idx1 != std::string::npos && idx2 != std::string::npos)
	{
		this->name = result.substr(idx1, idx2 - idx1);
	}
	idx1 = result.find("id author ") + 10;
	idx2 = result.find("\r\n", idx1);
	if (idx1 != std::string::npos && idx2 != std::string::npos)
	{
		this->author = result.substr(idx1, idx2 - idx1);
	}
	printf("name:%s\n", name.c_str());
	printf("author:%s\n", author.c_str());
}

std::pair<std::string, std::string> Engine::calcStep(std::string fen, float time, int depth, std::string& ret)
{
	ret = ""; //先清空

	process.addCmdLine("position fen " + fen);
	process.addCmdLine("go depth " + std::to_string(depth));

	try {

		process.execute("bestmove", "stop", time, ret);
		if (ret.empty())
		{
			process.exit();
			process.createProcess(this->enginePath);
			return std::make_pair("","");
		}

		printf("result:%s\n", ret.c_str());


		int idx1 = ret.find("bestmove"); //报错
		int idx2 = ret.find("ponder");
		if (idx1 != std::string::npos && idx2 == std::string::npos)
		{
			//最后一步绝杀 
			return std::make_pair(ret.substr(idx1 + 9, 4), "");
		}
		else {
			return std::make_pair(ret.substr(idx1 + 9, idx2 - 1 - (idx1 + 9)), ret.substr(idx2+7,4));
		}
	}
	catch (std::exception e) {

	}
	return std::make_pair("", "");
}
