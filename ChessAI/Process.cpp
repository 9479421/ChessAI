#include "pch.h"
#include "Process.h"
#include<iostream>
using namespace std;
void Process::createProcess(std::string process)
{
	USES_CONVERSION;

	//���ùܵ���ȫ����
	SECURITY_ATTRIBUTES sa = SECURITY_ATTRIBUTES();
	sa.bInheritHandle = TRUE;
	//sa.nLength = sizeof(sa);
	//sa.lpSecurityDescriptor = NULL;
	//���������ܵ�
	CreatePipe(&hOutRead, &hOutWrite, &sa, 0);
	CreatePipe(&hInRead, &hInWrite, &sa, 0);


	SetHandleInformation(hOutRead, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(hInWrite, HANDLE_FLAG_INHERIT, 0);



	STARTUPINFOA si = STARTUPINFOA();
	si.cb = sizeof(si);
	//si.wShowWindow = SW_NORMAL;
	si.dwFlags = /*STARTF_USESHOWWINDOW |*/ STARTF_USESTDHANDLES;
	si.hStdInput = hInRead;
	si.hStdOutput = hOutWrite;
	//si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	std::string currentDir;
	int idx = process.find_last_of("\\");
	currentDir = process.substr(0, idx);



	//process += " \r\n isready \r\n go";
	CreateProcessA(NULL, const_cast<char*>(process.c_str()), NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, currentDir.c_str(), &si, &pi);

	// not needed
	CloseHandle(hOutWrite);
	CloseHandle(hInRead);


	::WaitForSingleObject(pi.hThread, 2000);
	::WaitForSingleObject(pi.hProcess, 2000);
}


void Process::addCmdLine(std::string cmdLine)
{
	cmdLines.push_back(cmdLine);
}

std::string Process::execute(std::string endFlag)
{
	std::string cmd;
	for (int i = 0; i < cmdLines.size(); i++)
	{
		cmd += cmdLines[i] + "\r\n";
	}
	WriteFile(hInWrite, cmd.c_str(), cmd.length(), NULL, NULL);

	std::string strRet;
	char resultbuffer[1024]{ 0 };

	while (true)
	{
		memset(resultbuffer, 0, 1024);
		if (!ReadFile(hOutRead, resultbuffer, sizeof(resultbuffer) - 1, NULL, NULL))
		{
			break;
		}
		strRet += resultbuffer;

		//printf("%s", resultbuffer);

		int idx;
		if ((idx = strRet.find(endFlag)) != std::string::npos)
		{
			for (int i = 0; i < strlen(resultbuffer); i++)
			{
				std::string tmp = strRet.substr(idx + endFlag.size() + i, 1);
				if (tmp == "\r" || tmp == "\n")
				{
					printf("all:\n%s\n", strRet.c_str());
					goto end;
				}
			}
		}
	}
	end:

	cmdLines.clear();

	return strRet;
}

void Process::exit()
{
	//�����ڴ�ͽ���
	TerminateProcess(pi.hProcess, 1);
	// �رս��̺��߳̾��
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hInWrite);
	CloseHandle(hOutRead);

}
