#include "qJson.h"


std::string trim(const std::string& str) {
	// �ҵ���һ�����ǿո���ַ���λ��
	auto start = str.find_first_not_of(' ');
	// ����ַ���ȫ�ǿո񣬷���һ�����ַ���
	if (start == std::string::npos)
		return "";

	// �ҵ����һ�����ǿո���ַ���λ��
	auto end = str.find_last_not_of(' ');

	// ���شӿ�ʼ������λ�õ����ַ���
	return str.substr(start, end - start + 1);
}

int findFirstNotOf(const std::string& str, char ch...) {
	std::vector<char> chVector;
	chVector.push_back(ch);

	va_list chList;
	va_start(chList, ch);

	char nArgVal;
	for (;;)
	{
		nArgVal = va_arg(chList, char);
		if (nArgVal == '\0') //������־
		{
			break;
		}
		else {
			chVector.push_back(nArgVal);
		}

	}

	va_end(chList);

	for (int i = 0; i < str.size(); i++)
	{
		bool flag = false;
		for (int j = 0; j < chVector.size(); j++)
		{
			if (chVector[j] == str[i]) //���κ�һ����ȵģ���������
			{
				flag = true;
			}
		}
		if (flag == false)
		{
			return i;
		}
	}
	return -1;
}

bool isDigit(std::string str) {
	if (str[0] == '.' || str[str.size() - 1] == '.')
	{
		return false;
	}
	for (size_t i = 0; i < str.size(); i++)
	{
		if (i == 0 && str[i] == '-' && str.size() > 1) {
			continue;
		}
		if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.')) {
			return false;
		}
	}
	return true;
}


qJsonObject qJson::parseJsonObject(std::string str)
{
	qJsonObject retObject;


	str = trim(str);
	int size = str.size();
	if (str[0] == '{' && str[size - 1] == '}') //�жϷ����ַ����淶
	{
		str = str.substr(1, size - 2); //ȥ������β������

		bool end = false;
		while (true)
		{
			if (end) //������json�ַ�����β
			{
				break;
			}

			////ǰ׺�ж��ŵ������ʹ���� ����Ҳ���Զ�pass��

			int idx = str.find(":"); //ֻҪkey�ﲻҪ����:��OK

			std::string front = str.substr(0, idx);
			int yin1 = front.find_first_of('"');
			int yin2 = front.find_last_of('"');
			std::string key = front.substr(yin1 + 1, yin2 - (yin1 + 1));

			std::string after = str.substr(idx + 1);
			int firstCharIdx = after.find_first_not_of(' ');
			char firstChar = after.at(firstCharIdx);
			if (firstChar == '"') //�ַ���
			{
				for (int i = firstCharIdx + 1; i < after.size(); i++)
				{
					if (after[i] == '"' && after[i - 1] != '\\')  //�ҵ�value�Ľ�β��
					{

						std::string value = after.substr(firstCharIdx + 1, i - (firstCharIdx + 1));
						////�ַ�����β��
						str = str.substr(idx + 1 + i + 1);  // + firstCharIdx

						//printf("key:%s value:%s\n", key.c_str(), value.c_str());
						//д��
						retObject.setString(key, value);


						if (str.find_first_not_of(' ') == -1) //������β��
						{
							end = true;
						}
						break;
					}
				}
			}
			else if (firstChar == '{') //jsonObject
			{
				//Ѱ��after�Ľ�β;
				int middle = 1, big = 0, shuangyinhao = 0;

				for (int i = firstCharIdx + 1; i < after.size(); i++) {


					if (after[i] == '"' && after[i - 1] != '\\')
					{
						shuangyinhao++;
					}

					if (shuangyinhao % 2 != 1)  //˵�����ڲ����ַ�����
					{
						if (after[i] == '{')
						{
							middle++;
						}
						if (after[i] == '}')
						{
							middle--;
						}
						if (after[i] == '[')
						{
							big++;
						}
						if (after[i] == ']')
						{
							big--;
						}
					}
					if (middle == 0 && shuangyinhao % 2 == 0 && big == 0) //˵������ȫ���������ˣ�������������
					{

						//��ȡ�����ַ��� {*****}
						std::string objStr = after.substr(firstCharIdx, i - firstCharIdx + 1);
						//�ضϣ�����������ı�
						str = str.substr(idx + 1 + firstCharIdx + i + 1);
						//д�� 
						retObject.setJsonObject(key, /*�ݹ�*/parseJsonObject(objStr/*�����ı�*/)); //ͬʱ���ö��󴫵�

						if (str.find_first_not_of(' ') == -1) //������β��
						{
							end = true;
						}
						break;
					}

				}
			}
			else if (firstChar == '[') //jsonObject
			{
				//Ѱ��after�Ľ�β]
				int middle = 0, big = 1, shuangyinhao = 0;
				for (int i = firstCharIdx + 1; i < after.size(); i++) {
					if (after[i] == '"' && after[i - 1] != '\\')
					{
						shuangyinhao++;
					}

					if (shuangyinhao % 2 != 1)  //˵�����ڲ����ַ�����
					{
						if (after[i] == '{')
						{
							middle++;
						}
						if (after[i] == '}')
						{
							middle--;
						}
						if (after[i] == '[')
						{
							big++;
						}
						if (after[i] == ']')
						{
							big--;
						}
					}
					if (middle == 0 && shuangyinhao % 2 == 0 && big == 0) //˵������ȫ���������ˣ�������������
					{

						//��ȡ�����ַ��� [*****]
						std::string arrStr = after.substr(firstCharIdx, i - firstCharIdx + 1);
						//�ضϣ�����������ı�
						str = str.substr(idx + 1 + firstCharIdx + i + 1);
						//д�� 
						retObject.setJsonArray(key, /*�ݹ�*/parseJsonArray(arrStr/*�����ı�*/)); //ͬʱ���ö��󴫵�

						if (str.find_first_not_of(' ') == -1) //������β��
						{
							end = true;
						}
						break;
					}

				}

			}
			else {  //˵�������ֻ���null����Bool
				std::string value;
				int endIdx = after.find(',');
				if (endIdx == -1) //˵���ǽ�β��
				{
					value = trim(after.substr(firstCharIdx));
					endIdx = after.size() - 1; //������������β
				}
				else {
					value = trim(after.substr(firstCharIdx, endIdx - firstCharIdx));
				}

				str = str.substr(idx + 1 + endIdx + 1);
				if (value.compare("NULL") == 0 || value.compare("null") == 0) //˵��ֵΪnull
				{
					//����������Ժ�����
				}
				else if (value.compare("true") == 0 || value.compare("TRUE") == 0 || value.compare("false") == 0 || value.compare("FALSE") == 0) {

					retObject.setBool(key, (value.compare("true") == 0 || value.compare("TRUE") == 0) ? true : false);
				}
				else {
					//�ж��Ƿ�Ϸ����֣�������ǣ�˵��json���Ϲ�
					if (isDigit(value))
					{
						retObject.setDouble(key, std::stod(value));
					}
					else {
						//json���Ϲ棬ֱ���׳��쳣
						throw std::exception("format exception");
					}
				}

				if (str.find_first_not_of(' ') == -1) //������β��
				{
					end = true;
				}
			}
		}
	}

	return retObject;
}

qJsonArray qJson::parseJsonArray(std::string str)
{
	qJsonArray retJsonArray;


	str = trim(str);
	int size = str.size();
	if (str[0] == '[' && str[size - 1] == ']') //�жϷ����ַ����淶
	{
		str = str.substr(1, size - 2); //ȥ������β������


		//��ֵ����
		bool end = false;
		while (true)
		{
			if (end) //������json�ַ�����β
			{
				break;
			}

			int idx = findFirstNotOf(str, ' ', ',', '\0');  //��һ���ֵ�λ��
			//�ж����ַ������ͻ���Null������������
			if (str.at(idx) == '"')
			{
				for (size_t i = idx + 1; i < str.size(); i++)
				{
					if (str[i] == '"' && str[i - 1] != '\\')
					{
						std::string value = str.substr(idx + 1, i - (idx + 1));
						retJsonArray.addString(value);
						str = str.substr(i + 1);

						if (str.find_first_not_of(' ') == -1)
						{
							end = true;
						}
						break;
					}
				}
			}
			else if (str.at(idx) == '{') {
				std::string value;


				int middle = 1, big = 0, shuangyinhao = 0;
				for (size_t i = idx + 1; i < str.size(); i++)
				{
					if (str[i] == '"' && str[i - 1] != '\\')
					{
						shuangyinhao++;
					}

					if (shuangyinhao % 2 != 1)  //˵�����ڲ����ַ�����
					{
						if (str[i] == '{')
						{
							middle++;
						}
						if (str[i] == '}')
						{
							middle--;
						}
						if (str[i] == '[')
						{
							big++;
						}
						if (str[i] == ']')
						{
							big--;
						}
					}
					if (middle == 0 && shuangyinhao % 2 == 0 && big == 0) //˵������ȫ���������ˣ�������������
					{
						std::string jsonObjectStr = str.substr(idx, i - idx + 1);
						retJsonArray.addJsonObject(parseJsonObject(jsonObjectStr));

						str = str.substr(i + 1);

						if (str.find_first_not_of(' ') == -1)
						{
							end = true;
						}

						break;
					}
				}
			}
			else if (str.at(idx) == '[') {
				//������������

			}
			else {
				//Ϊnull��������
				std::string value;
				int endIdx = str.find(',', idx);
				if (endIdx == -1) //�ҵ��ַ�����β��
				{
					endIdx = str.size() - 1;
					value = trim(str.substr(idx, endIdx - idx + 1));
				}
				else {
					value = trim(str.substr(idx, endIdx - idx));
				}

				str = str.substr(endIdx + 1);

				//��ȡֵ
				if (value.compare("NULL") == 0 || value.compare("null") == 0) //˵��ֵΪnull
				{
					//����������Ժ�����
				}
				else if (value.compare("true") == 0 || value.compare("TRUE") == 0 || value.compare("false") == 0 || value.compare("FALSE") == 0) {
					retJsonArray.addBool((value.compare("true") == 0 || value.compare("TRUE") == 0) ? true : false);
				}
				else {
					//�ж��Ƿ�Ϸ����֣�������ǣ�˵��json���Ϲ�
					if (isDigit(value))
					{
						retJsonArray.addDigital(std::stod(value));
					}
					else {
						//json���Ϲ棬ֱ���׳��쳣
						throw std::exception("format exception");
					}
				}

				if (str.find_first_not_of(' ') == -1) //������β��
				{
					end = true;
				}
			}
		}

	}


	return retJsonArray;
}

std::string qJsonObject::getString(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).string;
		}
	}
	return "";
}

void qJsonObject::setString(std::string key, std::string value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(0, value, -1, qJsonObject(), qJsonArray())));
}

qJsonObject qJsonObject::getJsonObject(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).jsonObject;
		}
	}
	return qJsonObject();
}

void qJsonObject::setJsonObject(std::string key, qJsonObject value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(2, "", -1, value, qJsonArray())));
}

int qJsonObject::getInt(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).digital;
		}
	}
	return -1;
}

void qJsonObject::setInt(std::string key, int value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(1, "", value, qJsonObject(), qJsonArray())));
}

double qJsonObject::getDouble(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).digital;
		}
	}
	return -1;
}

void qJsonObject::setDouble(std::string key, double value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(1, "", value, qJsonObject(), qJsonArray())));
}

float qJsonObject::getFloat(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).digital;
		}
	}
	return -1;
}

void qJsonObject::setFloat(std::string key, float value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(1, "", value, qJsonObject(), qJsonArray())));
}

void qJsonObject::setJsonArray(std::string key, qJsonArray value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(3, "", -1, qJsonObject(), value)));
}

qJsonArray qJsonObject::getJsonArray(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).jsonArray;
		}
	}
	return qJsonArray();
}

bool qJsonObject::getBool(std::string key)
{
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ++it) {
		if (key.compare(it->first) == 0)
		{
			return ((obj<qJsonObject, qJsonArray>)(it->second)).digital;
		}
	}
	return false;
}

void qJsonObject::setBool(std::string key, bool value)
{
	maps_obj.insert(std::make_pair(key, obj<qJsonObject, qJsonArray>(4, "", value, qJsonObject(), qJsonArray())));
}

std::string qJsonObject::toString()
{
	std::stringstream ssm;
	ssm << "{";
	for (std::map<std::string, obj<qJsonObject, qJsonArray>>::iterator it = maps_obj.begin(); it != maps_obj.end(); ) {
		ssm << "\"" << it->first << "\":";

		int type = ((obj<qJsonObject, qJsonArray>)(it->second)).type;
		if (type == 0)
		{
			ssm << "\"" << ((obj<qJsonObject, qJsonArray>)(it->second)).string << "\"";
		}
		if (type == 1)
		{
			double digital = ((obj<qJsonObject, qJsonArray>)(it->second)).digital;
			int integer = static_cast<int>(digital);
			if (digital == integer)
			{
				ssm << integer;
			}
			else {
				ssm << digital;
			}

		}
		if (type == 2)
		{
			ssm << ((obj<qJsonObject, qJsonArray>)(it->second)).jsonObject.toString();
		}
		if (type == 3)
		{
			ssm << ((obj<qJsonObject, qJsonArray>)(it->second)).jsonArray.toString();
		}
		if (type == 4)
		{
			ssm << (((obj<qJsonObject, qJsonArray>)(it->second)).digital ? "true" : "false");
		}


		if (++it != maps_obj.end())
		{
			ssm << ",";
		}
	}

	ssm << "}";

	return ssm.str();
}

void qJsonArray::addJsonObject(qJsonObject jsonObject)
{
	maps_obj.push_back(obj<qJsonObject, qJsonArray>(2, "", -1, jsonObject, qJsonArray()));
}

qJsonObject qJsonArray::getJsonObject(int idx)
{
	return maps_obj[idx].jsonObject;
}

std::string qJsonArray::getString(int idx)
{
	return maps_obj[idx].string;
}

int qJsonArray::getInt(int idx)
{
	return maps_obj[idx].digital;
}

double qJsonArray::getDouble(int idx)
{
	return maps_obj[idx].digital;
}

double qJsonArray::getFloat(int idx)
{
	return maps_obj[idx].digital;
}

bool qJsonArray::getBool(int idx)
{
	return maps_obj[idx].digital;
}

int qJsonArray::size()
{
	return maps_obj.size();
}
void qJsonArray::addString(std::string str)
{
	maps_obj.push_back(obj<qJsonObject, qJsonArray>(0, str, -1, qJsonObject(), qJsonArray())); //�����ַ���
}

void qJsonArray::addDigital(double value)
{
	maps_obj.push_back(obj<qJsonObject, qJsonArray>(1, "", value, qJsonObject(), qJsonArray()));
}

void qJsonArray::addBool(bool value)
{
	maps_obj.push_back(obj<qJsonObject, qJsonArray>(4, "", value, qJsonObject(), qJsonArray()));
}

std::string qJsonArray::toString()
{
	std::stringstream ssm;
	ssm << "[";
	for (int i = 0; i < maps_obj.size(); i++) {

		int type = maps_obj[i].type;
		if (type == 0)
		{
			ssm << "\"" << maps_obj[i].string << "\"";
		}
		if (type == 1)
		{
			double digital = maps_obj[i].digital;
			int integer = static_cast<int>(digital);
			if (digital == integer)
			{
				ssm << integer;
			}
			else {
				ssm << digital;
			}

		}
		if (type == 2)
		{
			ssm << maps_obj[i].jsonObject.toString();
		}
		if (type == 3)
		{
			ssm << maps_obj[i].jsonArray.toString();
		}
		if (type == 4)
		{
			ssm << (maps_obj[i].digital ? "true" : "false");
		}

		if (i != maps_obj.size() - 1)
		{
			ssm << ",";
		}
	}
	ssm << "]";


	return ssm.str();
}
