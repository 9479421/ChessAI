#pragma once

#include<string>
class EngineConfig
{
public:
	EngineConfig() {

	}
	EngineConfig(std::string name, std::string author, std::string path, std::string threadNum = "16") {
		this->name = name;
		this->author = author;
		this->path = path;
		this->threadNum = threadNum;
	}
public:
	std::string name;
	std::string author;
	std::string path; 
	std::string threadNum;

};
