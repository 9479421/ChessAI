#include "pch.h"
#include "Utils.h"


//
// Function  : XSleepThread()
// Purpose   : The thread which will sleep for the given duration
// Returns   : DWORD WINAPI
// Parameters:       
//  1. pWaitTime -
//
DWORD WINAPI XSleepThread(LPVOID pWaitTime)
{
	XSleep_Structure* sleep = (XSleep_Structure*)pWaitTime;

	Sleep(sleep->duration);
	SetEvent(sleep->eventHandle);

	return 0;
}

void Utils::XSleep(int nWaitInMsecs)
{

	XSleep_Structure sleep;
	sleep.duration = nWaitInMsecs;
	sleep.eventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*DWORD dwThreadId;
	CreateThread(NULL, 0, &XSleepThread, &sleep, 0, &dwThreadId);*/

	HANDLE getHandle;
	getHandle = (HANDLE)_beginthreadex(NULL, 0,
		(unsigned(__stdcall*)(void*))XSleepThread, &sleep, 0, NULL);

	MSG msg;
	while (::WaitForSingleObject(sleep.eventHandle, 0) == WAIT_TIMEOUT)
	{
		// get and dispatch message
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	CloseHandle(sleep.eventHandle);
	CloseHandle(getHandle);

}



HBITMAP Utils::WindowCapture(HWND hwnd)
{
	if (hwnd == NULL) //��ͼ��Ļ
	{
		hwnd = ::GetDesktopWindow();
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);

	HDC hdc = GetWindowDC(hwnd);
	HDC memDC;
	memDC = ::CreateCompatibleDC(hdc);
	HBITMAP memBitmap;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	memBitmap = ::CreateCompatibleBitmap(hdc, width, height);
	SelectObject(memDC, memBitmap);
	BitBlt(memDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

	//ɾ�������ļ����ڴ�DC
	DeleteDC(memDC);
	//�ͷŴ���DC
	ReleaseDC(hwnd, hdc);

	return memBitmap;
}

HBITMAP Utils::WindowCapture_Front(HWND hwnd,bool autoTopMost)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);

	//�������ö� Ϊ��ͼ��׼��
	if (autoTopMost)
	{
		::SetWindowPos((hwnd), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(::GetParent(hwnd), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}


	HDC hdc = GetWindowDC(::GetDesktopWindow());
	HDC memDC;
	memDC = ::CreateCompatibleDC(hdc);
	HBITMAP memBitmap;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	memBitmap = ::CreateCompatibleBitmap(hdc, width, height);
	SelectObject(memDC, memBitmap);
	BitBlt(memDC, 0, 0, width, height, hdc, rect.left, rect.top, SRCCOPY);

	////ɾ�������ļ����ڴ�DC
	DeleteDC(memDC);
	////�ͷŴ���DC
	ReleaseDC(hwnd, hdc);

	return memBitmap;
}

HBITMAP Utils::WindowCaptere_D3D(HWND hwnd, bool autoTopMost)
{
	//�������ö� Ϊ��ͼ��׼��
	if (autoTopMost)
	{
		::SetWindowPos((hwnd), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(::GetParent(hwnd), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	}

	LPDIRECT3D9    g_pD3D = NULL;
	LPDIRECT3DDEVICE9   g_pd3dDevice = NULL;
	//����Direct3D����
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);
	// Get adapter display mode
	D3DDISPLAYMODE mode;
	g_pd3dDevice->GetDisplayMode(0, &mode);
	// Create the surface to hold the screen image data
	LPDIRECT3DSURFACE9 surf;
	g_pd3dDevice->CreateOffscreenPlainSurface(mode.Width,
		mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf, NULL); //ע����ĸ�����������D3DPOOL_DEFAULT
	// Get the screen data
	g_pd3dDevice->GetFrontBufferData(0, surf);
	RECT* rect = NULL;
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(WINDOWINFO);
	if (hwnd) // capture window
	{
		::GetWindowInfo(hwnd, &windowInfo);
		rect = &windowInfo.rcWindow;
	}
	LPD3DXBUFFER buffer;
	D3DXSaveSurfaceToFileInMemory(&buffer, D3DXIFF_BMP, surf, NULL, rect);
	DWORD imSize = buffer->GetBufferSize();
	void* imgBuffer = buffer->GetBufferPointer();


	WCHAR tmpPath[MAX_PATH];
	GetTempPath(MAX_PATH, tmpPath);
	wcscat(tmpPath, L"yigebi.bmp");

	std::fstream out;
	out.open(tmpPath, std::ios_base::binary | std::ios_base::out);
	out.write((char*)imgBuffer, imSize);
	out.clear();
	out.close();

	surf->Release();

	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, tmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	return hBitmap;
}

void Utils::saveBitMap(CString path, HBITMAP bitmap)
{
	CImage image;
	image.Attach(bitmap);
	image.Save(path);
	image.Detach();

	DeleteObject(bitmap);
}

HBITMAP Utils::stretchBitMap(HBITMAP bitmap, int width, int height)
{
	CImage image;
	image.Attach(bitmap);

	CImage imageNew;
	imageNew.Create(width, height, 32);
	HDC dcNew = imageNew.GetDC();
	SetStretchBltMode(dcNew, HALFTONE);
	image.StretchBlt(dcNew, 0, 0, width, height);//��image�����imageNew��DC��
	image.Detach();
	imageNew.ReleaseDC();


	//��ȡimageNewλͼ
	HBITMAP hBmp = imageNew.Detach();

	//DeleteDC(dcNew);
	return hBmp;
}

CString Utils::selectFolder()
{
	LPITEMIDLIST lpitemidlist;
	SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, NULL, &lpitemidlist);
	if (lpitemidlist == NULL)
		return CString("");

	//���öԻ���
	BROWSEINFO browseinfo;
	ZeroMemory(&browseinfo, sizeof(browseinfo));
	browseinfo.pidlRoot = lpitemidlist;
	browseinfo.lpszTitle = _T("ѡ��Ŀ¼��·��");
	browseinfo.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	//�򿪶Ի���
	LPITEMIDLIST obj = SHBrowseForFolder(&browseinfo);
	if (obj != NULL)
	{
		TCHAR szPath[MAX_PATH];
		SHGetPathFromIDList(obj, szPath);
		return CString(szPath);
	}

	return CString("");
}

CString Utils::readFile(CString path)
{
	FILE* filePtr = fopen(CW2A(path), "rb");
	CString sb;
	char buffer[1024];
	while (!feof(filePtr))
	{
		memset(buffer, 0, sizeof(buffer));
		fread(buffer, sizeof(char), sizeof(buffer) - 1, filePtr);
		/*printf("%s", buffer);*/
		sb += CA2W(buffer);
	}
	fclose(filePtr);  // �ر��ļ�
	return sb;
}

void Utils::writeFile(CString path, CString content)
{
	FILE* filePtr = fopen(CW2A(path), "wb");
	if (filePtr == NULL)
	{
		MessageBoxA(NULL, "д���ļ�ʧ��", "��Ϣ", 0);
	}
	fwrite(CW2A(content), sizeof(char), strlen(CW2A(content)), filePtr);
	fclose(filePtr);  // �ر��ļ�
}

std::list<File> Utils::enumFiles(CString path)
{
	std::list<File> fileList;

	WIN32_FIND_DATA ListFile;
	HANDLE hListFile = FindFirstFile(path + "\\*", &ListFile);
	if (hListFile != INVALID_HANDLE_VALUE) {
		do
		{
			if (lstrcmp(ListFile.cFileName, L"..") == 0 || lstrcmp(ListFile.cFileName, L".") == 0)
				continue;

			if (!(ListFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				//�ļ�
				//printf("%ls\n", ListFile.cFileName);
				fileList.push_back(File(ListFile.cFileName, path));
			}
			else
			{
				//Ŀ¼
			}
		} while (FindNextFile(hListFile, &ListFile));
	}
	return fileList;
}

std::list<CString> Utils::splitStr(CString str, CString splitStr)
{
	std::list<CString> strList;
	int idx = 0;
	int position = 0;
	do
	{
		position = str.Find(splitStr, idx);
		if (position == -1)
		{
			CString tmp = str.Mid(idx);
			if (tmp.GetLength() > 0) //��β��\r\n���������ı�
			{
				strList.push_back(tmp);
			}
			break;
		}
		else {
			CString tmp = str.Mid(idx, position - idx);
			strList.push_back(tmp);
			idx = position + splitStr.GetLength();
		}
	} while (true);
	return strList;
}

void Utils::copyFile(CString filePath, CString distPath)
{
	system(CW2A(L"copy " + filePath + L" " + distPath));
}

CString Utils::getTimeStamp(bool isMilliSecond)
{
	CString timestampStr;
	time_t timestamp = time(NULL);
	if (isMilliSecond)
	{
		timestampStr.Format(L"%ld000", timestamp); //13λ��ʱ����ȷ���õ���ʱ���ٸĽ�
	}
	else {
		timestampStr.Format(L"%ld", timestamp); //10λʱ���
	}
	return timestampStr;
}

bool Utils::HBitmap2Mat(HBITMAP& hBmp, cv::Mat& mat)
{
	BITMAP Bmp;

	GetObject(hBmp, sizeof(BITMAP), &Bmp);
	int nChannels = Bmp.bmBitsPixel == 1 ? 1 : Bmp.bmBitsPixel / 8;
	//int depth = Bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;


	cv::Mat des_mat;

	des_mat.create(cv::Size(Bmp.bmWidth, Bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	GetBitmapBits(hBmp, Bmp.bmHeight * Bmp.bmWidth * nChannels, des_mat.data);

	mat = des_mat;

	return true;

}


std::string Utils::ReplaceAllText(std::string str, std::string a, std::string b)
{
	int oldPos = 0;
	while (str.find(a, oldPos) != -1)//��δ���滻���ı���Ѱ��Ŀ���ı�
	{
		int start = str.find(a, oldPos);//�ҵ�Ŀ���ı�����ʼ�±�

		str.replace(start, a.size(), b);
		//��str[start]��ʼ��str[a.size()]�滻Ϊb
		//str[start]��str[a.size()]Ҳ����a���ڵ�Ƭ��

		oldPos = start + b.size();//��¼δ�滻�ı�����ʼ�±�
	}
	return str;
}

int Utils::getTextTimes(std::string str, std::string flag)
{
	int times = 0;

	int idx = 0;
	while (true)
	{
		idx = str.find(flag, idx);
		if (idx == std::string::npos)
		{
			return times;
		}
		else {
			times++;
			idx += flag.size();
		}
	}
}

std::string Utils::trim(const std::string& str) {
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

int Utils::findFirstNotOf(const std::string& str, char ch...) {
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


bool Utils::isDigit(std::string str)
{
	if (str[0] == '.' || str[str.size() - 1] == '.')
	{
		return false;
	}
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!((str[i] >= '0' && str[i] < '9') || str[i] == '.')) {
			return false;
		}
	}
	return true;
}
