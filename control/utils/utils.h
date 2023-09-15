#pragma once


namespace utils
{
	uint64_t hexToInteger(std::string hex)
	{
		std::stringstream ss2;
		uint64_t d2;
		ss2 << std::hex << hex;
		ss2 >> d2;
		return d2;
	}

	std::string IntegerTohex(uint64_t dec)
	{
		std::stringstream ss2;
		ss2 << std::hex << dec;
		return ss2.str();
	}

	std::string bytesToHexString(const BYTE* bytes, const int length)
	{
		if (bytes == NULL) {
			return "";
		}
		std::string buff;
		const int len = length;
		for (int j = 0; j < len; j++) {
			int high = bytes[j] / 16, low = bytes[j] % 16;
			buff += (high < 10) ? ('0' + high) : ('a' + high - 10);
			buff += (low < 10) ? ('0' + low) : ('a' + low - 10);
			buff += " ";
		}
		return buff;
	}

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = (int)(::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL));

		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);

		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete[]pwBuf;
		delete[]pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}

	HICON GetProcessIcon(std::string path)
	{
		HICON hIcon{};
		ExtractIconExA(path.c_str(), 0, nullptr, &hIcon, 1);
		return hIcon;
	}

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT num = 0, size = 0;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

		Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
		bool found = false;
		for (UINT ix = 0; !found && ix < num; ++ix)
		{
			if (_wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[ix].Clsid;
				found = true;
				break;
			}
		}

		free(pImageCodecInfo);
		return found;
	}

	bool SaveIconToPng(HICON hIcon, std::string lpszPicFileName)
	{
		if (hIcon == NULL)
		{
			return false;
		}

		ICONINFO icInfo = { 0 };
		if (!::GetIconInfo(hIcon, &icInfo))
		{
			return false;
		}

		BITMAP bitmap;
		GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		Gdiplus::Bitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pWrapBitmap = NULL;

		do
		{
			if (bitmap.bmBitsPixel != 32)
			{
				pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
			}
			else
			{
				pWrapBitmap = Gdiplus::Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
				if (!pWrapBitmap)
					break;

				Gdiplus::BitmapData bitmapData;
				Gdiplus::Rect rcImage(0, 0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());

				pWrapBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData);
				pBitmap = new (Gdiplus::Bitmap)(bitmapData.Width, bitmapData.Height, bitmapData.Stride, PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
				pWrapBitmap->UnlockBits(&bitmapData);
			}

			CLSID encoderCLSID;
			GetEncoderClsid(_T("image/png"), &encoderCLSID);
			std::wstring aa{ lpszPicFileName.begin(),lpszPicFileName.end() };
			Gdiplus::Status st = pBitmap->Save(aa.c_str(), &encoderCLSID, NULL);
			if (st != Gdiplus::Ok)
				break;

		} while (false);

		delete pBitmap;
		if (pWrapBitmap)
			delete pWrapBitmap;
		DeleteObject(icInfo.hbmColor);
		DeleteObject(icInfo.hbmMask);

		return true;
	}

	HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// ���� PID ��ģ����(��Ҫд��׺���磺".dll")����ȡģ����ڵ�ַ��
		MODULEENTRY32 moduleEntry;
		HANDLE handle = NULL;
		handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  ��ȡ���̿����а�����th32ProcessID��ָ���Ľ��̵����е�ģ�顣
		if (!handle) {
			CloseHandle(handle);
			return NULL;
		}
		ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
		moduleEntry.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(handle, &moduleEntry)) {
			CloseHandle(handle);
			return NULL;
		}
		do {
			if (_tcscmp(moduleEntry.szModule, moduleName) == 0) { return moduleEntry.hModule; }
		} while (Module32Next(handle, &moduleEntry));
		CloseHandle(handle);
		return 0;
	}

	uint32_t GetMainThreadId(uint32_t pid)
	{
		THREADENTRY32 te32;
		HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap == INVALID_HANDLE_VALUE)
			return 0;

		te32.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(hThreadSnap, &te32)) 
		{
			CloseHandle(hThreadSnap);     // ������ʹ�ú�������ն���!
			return(FALSE);
		}

		do 
		{
			if (te32.th32OwnerProcessID == pid) 
			{
				CloseHandle(hThreadSnap);
				return te32.th32ThreadID;
			}
		} while (Thread32Next(hThreadSnap, &te32));

		CloseHandle(hThreadSnap);
		return 0;
	}

	void EnumCurtAllProcess(std::vector<ProcessItem>& items)
	{
		HANDLE hSnapshot_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot_proc)
		{
			PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
			bool bprocess = Process32First(hSnapshot_proc, &pe);
			if(!bprocess)
				return;

			std::filesystem::path icon_dir("./Data/icon/");
			if (std::filesystem::exists(icon_dir))
				std::filesystem::remove_all(icon_dir);
			std::filesystem::create_directory(icon_dir);

			while (bprocess)
			{
				std::wstring wtmp(pe.szExeFile);
				std::string fullPath(wtmp.begin(), wtmp.end());
				char imagepath[MAX_PATH]{}, startuptime[MAX_PATH]{};
				auto hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
				if (hProc)
				{
					//Get Module Path
					GetModuleFileNameExA(hProc, NULL, imagepath, MAX_PATH);
					//GetStartupTime
					FILETIME ftCreation, ftExit, ftKernel, ftUser;
					SYSTEMTIME stCreation, lstCreation;
					if (GetProcessTimes(hProc, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
						FileTimeToSystemTime(&ftCreation, &stCreation);
						SystemTimeToTzSpecificLocalTime(NULL, &stCreation, &lstCreation);
						sprintf(startuptime,"%04d-%02d-%02d %02d:%02d:%02d", lstCreation.wYear, lstCreation.wMonth, lstCreation.wDay, lstCreation.wHour, lstCreation.wMinute, lstCreation.wSecond);
					}
					CloseHandle(hProc);
				}
				std::string proName = fullPath.substr(fullPath.rfind("\\") + 1);

				//��������icon��Ŀ¼
				

				ID3D11ShaderResourceView* icon = nullptr;
				auto hIcon = GetProcessIcon(imagepath);
				if (hIcon != NULL && SaveIconToPng(hIcon, ("./Data/icon/" + proName + ".png").c_str()))
					icon = render::get_instasnce()->DX11LoadTextureImageFromFile(("./Data/icon/" + proName + ".png").c_str());
				items.push_back({ icon,pe.th32ProcessID ,proName,pe.th32ParentProcessID ,imagepath ,startuptime });
				bprocess = Process32Next(hSnapshot_proc, &pe);
			}
			CloseHandle(hSnapshot_proc);
		}
	}

	void EnumPidModules(uint32_t pid ,std::vector<data::process::ModuleItem>& items)
	{
		HANDLE hSnapshot_proc = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (hSnapshot_proc)
		{
			MODULEENTRY32  me32 = { sizeof(MODULEENTRY32) };
			bool bprocess = Module32First(hSnapshot_proc, &me32);
			while (bprocess)
			{
				std::wstring wtmp(me32.szExePath);
				std::string fullPath(wtmp.begin(), wtmp.end());
				items.push_back({ fullPath,(uint64_t)me32.modBaseAddr,me32.modBaseSize });
				bprocess = Module32Next(hSnapshot_proc, &me32);
			}
			CloseHandle(hSnapshot_proc);
		}
	}

	void EnumPidThread(uint32_t pid,std::vector<data::process::ThreadItem>& items)
	{
		THREADENTRY32 te32;
		HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap == INVALID_HANDLE_VALUE)
			return;

		te32.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(hThreadSnap, &te32))
		{
			CloseHandle(hThreadSnap);     // ������ʹ�ú�������ն���!
			return;
		}

		HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
		NTQUERYINFORMATIONTHREAD NtQueryInformationThread = NULL;
		NtQueryInformationThread = (NTQUERYINFORMATIONTHREAD)GetProcAddress(hNtdll, "NtQueryInformationThread");

		do
		{
			
			if (te32.th32OwnerProcessID == pid)
			{
				data::process::ThreadItem item{};
				uint64_t startaddr{};
				DWORD retlen{};
				auto hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
				if (hThread)
				{
					NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &startaddr, sizeof(startaddr), &retlen);
					CloseHandle(hThread);
				}
				item.DeltaPri = te32.tpDeltaPri;
				item.threadid = te32.th32ThreadID;
				item.startAddr = startaddr;
				items.push_back(item);
			}
			
		} while (Thread32Next(hThreadSnap, &te32));

		CloseHandle(hThreadSnap);
	}

	BOOL InjectDLL(DWORD PID, const char* Path) {
		HANDLE hProcess;
		DWORD dwSize = 0;
		LPVOID PDllAddr;
		FARPROC func;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
		if (hProcess == NULL) 
		{
			MessageBoxA(NULL,"OpenProcess Failed!","control",NULL);
			return false;
		}
		dwSize = strlen(Path) + 1;
		PDllAddr = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (PDllAddr == NULL) 
		{
			MessageBoxA(NULL, "VirtualAllocEx Error", "control", NULL);
			return false;
		}
		BOOL pWrite = WriteProcessMemory(hProcess, PDllAddr, Path, dwSize, NULL);
		if (!pWrite) 
		{
			MessageBoxA(NULL, "WriteProcessMemory Error", "control", NULL);
			return false;
		}
		HMODULE hModule = GetModuleHandleA("kernel32.dll");
		func = GetProcAddress(hModule, "LoadLibraryA");
		if (func == NULL) 
		{
			MessageBoxA(NULL, "GetProcAddress Error", "control", NULL);
			return false;

		}
		HANDLE pRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)func, PDllAddr, 0, NULL);
		if (pRemoteThread == NULL) 
		{
			MessageBoxA(NULL, "CreateRemoteThread Error", "control", NULL);
			return false;
		}
		WaitForSingleObject(pRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, PDllAddr, dwSize, MEM_COMMIT);
		CloseHandle(hProcess);
		return true;
	}

	bool CallFunction(ControlCmd cmd)
	{
		auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, data::global::target);
		if (hProcess == NULL)
		{
			MessageBoxA(NULL, "OpenProcess Failed!", "control", NULL);
			return false;
		}
		//��������ڴ�
		auto lpParam = VirtualAllocEx(hProcess, NULL, sizeof(ControlCmd), MEM_COMMIT, PAGE_READWRITE);
		if (lpParam == NULL)
		{
			MessageBoxA(NULL, "VirtualAllocEx Params Failed!", "control", NULL);
			return false;
		}

		if (!WriteProcessMemory(hProcess, lpParam, &cmd, sizeof(ControlCmd), NULL))
		{
			MessageBoxA(NULL, "Write Params Failed!", "control", NULL);
			return false;
		}
		HANDLE  pRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(data::global::fnDispatch), lpParam, NULL, NULL);
		if (pRemoteThread == NULL)
		{
			MessageBoxA(NULL, "CreateRemoteThread Error", "control", NULL);
			return false;
		}
		WaitForSingleObject(pRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, lpParam, sizeof(ControlCmd), MEM_COMMIT);
		CloseHandle(hProcess);
		return true;
	}

	namespace Mem 
	{

	}

	
}
