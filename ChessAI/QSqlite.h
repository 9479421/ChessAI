#pragma once
#include"sqlite3.h"
#pragma comment(lib,"sqlite3.lib")


#include<string>
#include<vector>
#include<map>
class QSqlite
{
private:
	sqlite3* db;
public:
	QSqlite();
	QSqlite(std::string dbName);
	~QSqlite();
	bool open(std::string dbName);
	bool execute(const std::string& sql, int argNums, ...);
	bool execute(const std::string& sql);
	std::vector<std::map<std::string,std::string>> query(const std::string& sql,int argNums,...);
	std::vector<std::map<std::string,std::string>> query(const std::string& sql);

};

