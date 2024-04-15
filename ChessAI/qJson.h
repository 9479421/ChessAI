#pragma once

#include<unordered_map>
#include<map>
#include<vector>
#include<string>
#include<iostream>
#include<sstream>
#include<stdarg.h>

template<typename T1, typename T2>
class obj
{
public:
	int type; // 0 str     1 digital   2  jsonobject  3 jsonarray      -1考虑搞成Null后面再说
	std::string string;
	double digital;
	T1 jsonObject;
	T2 jsonArray;

public:
	obj(int type, std::string string, double digital, T1 jsonObject, T2 jsonArray) {
		this->type = type;
		this->string = string;
		this->digital = digital;
		this->jsonObject = jsonObject;
		this->jsonArray = jsonArray;
	}
private:

};


class qJsonArray;

class qJsonObject
{
public:
	qJsonObject() {

	}
	std::string getString(std::string key);
	void setString(std::string key, std::string value);
	qJsonObject getJsonObject(std::string key);
	void setJsonObject(std::string key, qJsonObject value);
	int getInt(std::string key);
	void setInt(std::string key, int value);
	double getDouble(std::string key);
	void setDouble(std::string key, double value);
	float getFloat(std::string key);
	void setFloat(std::string key, float value);
	void setJsonArray(std::string key, qJsonArray value);
	qJsonArray getJsonArray(std::string key);
	std::string toString();
private:
	std::map<std::string, obj<qJsonObject, qJsonArray>> maps_obj;
};


class qJsonArray
{
public:
	qJsonArray() {

	}
	void addJsonObject(qJsonObject jsonObject);
	qJsonObject getJsonObject(int idx);
	std::string getString(int idx);
	int getInt(int idx);
	double getDouble(int idx);
	double getFloat(int idx);
	int size();
	void addString(std::string str);
	void addDigital(double value);
	std::string toString();

public:
	std::vector <obj<qJsonObject, qJsonArray>> maps_obj;
};



class qJson
{
public:
	static qJsonObject parseJsonObject(std::string str);
	static qJsonArray parseJsonArray(std::string str);
};
