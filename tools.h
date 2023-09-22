#ifndef ToolsH
#define ToolsH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <IdComponent.hpp>

// ---------------------------------------------------------------------------
String tl_GetModuleName(); // .exe full path
String tl_GetProgramPath(); // including trailing '\'

typedef void(__closure * FTCdef)(const String);
void tl_RunInMainThread(FTCdef FuncToCall, const String fP1);

String BytesToHexStr(unsigned char *bytes, int count);
String BytesToHexStr(const TIdBytes &bytes);

void t_RunProcess(String runcmd);
String t_GetGateways();
inline int t_Max(int n1, int n2);
void t_SetScrollWidth(TListBox *lb);

// ---------------------------------------------------------------------------
#endif
