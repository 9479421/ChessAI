#pragma once
#include<list>
#include<time.h>
#include<fstream>
#include<atlstr.h>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")

#include<opencv2/opencv.hpp>


struct XSleep_Structure
{
	int duration;
	HANDLE eventHandle;
};

struct File
{
	CString fileName;
	CString filePath;
	
	File(CString fileName, CString filePath) {
		this->fileName = fileName;
		this->filePath = filePath;
	}
};
class Utils
{
public:
	static void XSleep(int nWaitInMsecs);
	static HBITMAP WindowCapture(HWND hwnd = NULL);
	static HBITMAP WindowCapture_Front(HWND hwnd, bool autoTopMost);
	static HBITMAP WindowCaptere_D3D(HWND hwnd, bool autoTopMost);
	static void saveBitMap(CString path, HBITMAP bitmap);
	static HBITMAP stretchBitMap(HBITMAP bitmap,int width,int height);
	static CString selectFolder();
	static CString readFile(CString path);
	static void writeFile(CString path,CString content);
	static std::list<File> enumFiles(CString path);
	static std::list<CString> splitStr(CString str,CString splitStr);
	static void copyFile(CString filePath,CString distPath);
	static CString getTimeStamp(bool isMilliSecond = FALSE);
	static bool HBitmap2Mat(HBITMAP& hBmp, cv::Mat& mat);
	static std::string ReplaceAllText(std::string str, std::string a, std::string b);
	static int getTextTimes(std::string str, std::string flag);
	static std::string trim(const std::string& str);
	static int findFirstNotOf(const std::string& str, char ch...);
	static bool isDigit(std::string str);

};





