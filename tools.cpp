// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <sstream>
#include <iostream>
#include <iomanip>

#include "tools.h"

// ---------------------------------------------------------------------------
String tl_GetModuleName()
{
	wchar_t buf[MAX_PATH];
	GetModuleFileName(NULL, buf, MAX_PATH);
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
