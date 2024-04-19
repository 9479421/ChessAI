#pragma once
#include<string>
class moveInfo
{
public:

	moveInfo(std::string step, std::string qpstep, std::string fen) {
		this->step = step;
		this->qpstep = qpstep;
		this->fen = fen;
	}
	~moveInfo() {}
	std::string getStep() {
		return step;
	}
	std::string getQpStep() {
		return qpstep;
	}
	std::string getFen() {
		return fen;
	}

private:
	std::string step;
	std::string qpstep;
	std::string fen;
};

