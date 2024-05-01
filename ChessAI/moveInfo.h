#pragma once
#include<string>
class moveInfo
{
public:

	moveInfo(std::string step, std::string qpstep, std::string fen,std::string score, std::string time) {
		this->step = step;
		this->qpstep = qpstep;
		this->fen = fen;
		this->score = score;
		this->time = time;
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
	std::string getScore() {
		return score;
	}
	std::string getTime() {
		return time;
	}
private:
	std::string step;
	std::string qpstep;
	std::string fen;
	std::string score;
	std::string time;
};

