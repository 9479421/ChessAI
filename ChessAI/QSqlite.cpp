#include "QSqlite.h"
#include<iostream>

QSqlite::QSqlite()
{
}

QSqlite::QSqlite(std::string dbName)
{
	int rc = sqlite3_open(dbName.c_str(), &db);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Cannot open datebase: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		db = nullptr;
	}
}

QSqlite::~QSqlite()
{
	if (db)
	{
		sqlite3_close(db);
	}
}

bool QSqlite::open(std::string dbName)
{
	int rc = sqlite3_open(dbName.c_str(), &db);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Cannot open datebase: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		db = nullptr;
		return false;
	}
	return true;
}

bool QSqlite::execute(const std::string& sql, int argNums, ...)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		va_list argList;
		va_start(argList, argNums);
		for (size_t i = 0; i < argNums; i++)
		{
			char* str = va_arg(argList, char*);
			sqlite3_bind_text(stmt, i + 1, str, -1, SQLITE_TRANSIENT);
		}
		va_end(argList);

		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return true;
		}
	}
	std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
	return false;
}
bool QSqlite::execute(const std::string& sql)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return true;
		}
	}
	sqlite3_finalize(stmt);
	std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
	return false;
}

std::vector<std::map<std::string, std::string>> QSqlite::query(const std::string& sql, int argNums, ...)
{
	
	std::vector<std::map<std::string, std::string>> retVec;

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		va_list argList;
		va_start(argList, argNums);
		for (size_t i = 0; i < argNums; i++)
		{
			char* str = va_arg(argList, char*);
			sqlite3_bind_text(stmt, i + 1, str, -1, SQLITE_TRANSIENT);
		}
		va_end(argList);


		while (sqlite3_step(stmt) == SQLITE_ROW) {

			std::map<std::string, std::string> map;
			//遍历字段
			int nCount = sqlite3_column_count(stmt);
			for (size_t i = 0; i < nCount; i++)
			{
				std::string name = sqlite3_column_name(stmt, i);
				int type = sqlite3_column_type(stmt, i);
				switch (type)
				{
				case 1:
					//SQLITE_INTEGER 
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), std::to_string(sqlite3_column_int(stmt, i))));
					break;
				case 2:
					//SQLITE_FLOAT 
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), std::to_string(sqlite3_column_double(stmt, i))));
					break;
				case 3:
					//SQLITE_TEXT 
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), (char*)sqlite3_column_text(stmt, i)));
					break;
				case 4:
					//SQLITE_BLOB 
					break;
				case 5:
					//SQLITE_NULL 
				default:
					break;

				}
			}
			retVec.push_back(map);
		}
	}
	return retVec;
}

std::vector<std::map<std::string, std::string>> QSqlite::query(const std::string& sql)
{
	std::vector<std::map<std::string, std::string>> retVec;

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			
			std::map<std::string, std::string> map;
			//遍历字段
			int nCount = sqlite3_column_count(stmt);
			for (size_t i = 0; i < nCount; i++)
			{
				std::string name = sqlite3_column_name(stmt, i);
				int type = sqlite3_column_type(stmt, i);
				switch (type)
				{
				case 1:
					//SQLITE_INTEGER
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), std::to_string(sqlite3_column_int(stmt, i))));
					break;
				case 2:
					//SQLITE_FLOAT
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), std::to_string(sqlite3_column_double(stmt, i))));
					break;
				case 3:
					//SQLITE_TEXT
					map.insert(std::make_pair<std::string, std::string>(name.c_str(), (char*)sqlite3_column_text(stmt, i)));
					break;
				case 4:
					//SQLITE_BLOB
					break;
				case 5:
					//SQLITE_NULL
				default:
					break;
				}
			}
			retVec.push_back(map);
		}
	}
	return retVec;
}
