// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
// #include <ws2ipdef.h>
// #include <ws2tcpip.h>
#include <iphlpapi.h>

#include <sstream>
#include <iostream>
#include <iomanip>

#include "tools.h"

// ---------------------------------------------------------------------------
String tl_GetModuleName()
{
	wchar_t buf[1024];
	GetModuleFileName(NULL, buf, 1024);
	return (String)buf;
}

// ---------------------------------------------------------------------------
String tl_GetProgramPath() // path with trailing '\'
{
	static String Path = tl_GetModuleName();
	return Path.SubString(1, Path.LastDelimiter(L"\\"));
}

// ---------------------------------------------------------------------------
void tl_RunInMainThread(FTCdef FuncToCall, const String fP1)
{
	struct Args
	{
		String P1;
		FTCdef FTC;

		void __fastcall ExecFuncQueue()
		{
			try
			{
				FTC(P1);
			}
			__finally
			{
				delete this;
			}
		}
	};

	Args *args = new Args;
	args->P1 = fP1;
	args->FTC = FuncToCall;
	TThread::Queue(NULL, &args->ExecFuncQueue);
}

// ---------------------------------------------------------------------------
String BytesToHexStr(unsigned char *bytes, int count)
{
	if (count <= 0)
		return L"";

	std::wstringstream ss;
	ss << std::hex << std::uppercase;
	for (int i = 0; i < count; i++)
	{
		ss << std::setw(2) << std::setfill(L'0') << (int)bytes[i] << L" ";
	}
	String hexStr(ss.str().c_str());
	return hexStr.TrimRight();
}

// ---------------------------------------------------------------------------
String BytesToHexStr(const TIdBytes &bytes)
{
	if (bytes.Length <= 0)
		return L"";

	std::wstringstream ss;
	ss << std::hex << std::uppercase;
	for (int i = 0; i < bytes.Length; i++)
	{
		ss << std::setw(2) << std::setfill(L'0') << (int)bytes[i] << L" ";
	}
	String hexStr(ss.str().c_str());
	return hexStr.TrimRight();
}

// ---------------------------------------------------------------------------
void t_RunProcess(String runcmd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcess(NULL, runcmd.w_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		String msg;
		msg.printf(L"CreateProcess failed. Error: %lx\n%s", GetLastError(),
			runcmd.w_str());
		throw Exception(msg);
	}
}

// ---------------------------------------------------------------------------
String GetGateways()
{
    String rv;
	DWORD dwRetVal = 0;
	/* Variables used to print error messages */
	LPVOID lpMsgBuf;

	/* Variables used to store information returned by
	 GetIpAddrTable */
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (IP_ADAPTER_INFO*) malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		throw new Exception(L"Error allocating memory needed to call GetAdaptersinfo");
	}

	if (GetAdaptersInfo(pAdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*) malloc(dwBufLen);
		if (pAdapterInfo == NULL)
		{
			throw new Exception
				(L"Error allocating memory needed to call GetAdaptersinfo");
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &dwBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			//print_gateway(pAdapter);
			String str;
			str.printf(L"%s\r\n%s\r\n", String(pAdapter->Description).w_str(),
				String(pAdapter->GatewayList.IpAddress.String).w_str());
			rv += str;

			pAdapter = pAdapter->Next;
		}
	}
	else
	{
		String str;
		str.printf(L"GetAdaptersInfo failed : %d", dwRetVal);
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL,
			SUBLANG_DEFAULT), // Default language
			(LPTSTR) & lpMsgBuf, 0, NULL))
		{
			String str1;
			str1.printf(L"\nError: %s", lpMsgBuf);
			str += str1;
		}
		LocalFree(lpMsgBuf);
		free(pAdapterInfo);
		throw new Exception(str);
	}

	if (pAdapterInfo)
		free(pAdapterInfo);

	return rv;
}

// ---------------------------------------------------------------------------
