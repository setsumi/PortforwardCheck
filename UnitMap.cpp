// ---------------------------------------------------------------------------

#include <vcl.h>
#include <System.RegularExpressionsCore.hpp>
#include <IdStackWindows.hpp>
#pragma hdrstop

#include "UnitMap.h"

#include "Unit1.h"
#include "tools.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMap *FormMap;

// ---------------------------------------------------------------------------
__fastcall TFormMap::TFormMap(TComponent* Owner) : TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::CreateParams(TCreateParams &Params)
{
	TForm::CreateParams(Params);
	Params.ExStyle = Params.ExStyle | WS_EX_APPWINDOW;
	Params.WndParent = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caHide;
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::ActionHideExecute(TObject *Sender)
{
	Hide();
}

// ---------------------------------------------------------------------------
void TFormMap::Clear()
{
	memoLocalHost->Clear();
	lbLocalGate->Clear();
	memoPublicGate->Clear();
	edtLocalPort->Clear();
	edtPublicPort->Clear();
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::btnSysInfoClick(TObject *Sender)
{
	String cmd;
	cmd.printf(L"CMD.EXE /K \"ECHO %s && %s\"", btnSysInfo->Hint.w_str(),
		btnSysInfo->Hint.w_str());
	t_RunProcess(cmd);
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::btnPortForwardClick(TObject *Sender)
{
	ShellExecute(NULL, L"open", btnPortForward->Hint.w_str(), NULL, NULL, SW_NORMAL);
}

// ---------------------------------------------------------------------------
void __fastcall TFormMap::lbLocalGateClick(TObject *Sender)
{
	SelectGateway();
}

// ---------------------------------------------------------------------------
bool TFormMap::SelectGateway()
{
	String ip;
	int index = lbLocalGate->ItemIndex;
	bool valid = false;
	if (index > -1)
	{
		ip = lbLocalGate->Items->Strings[index];
		if (!ip.IsEmpty())
		{
			TPerlRegEx *regex = new TPerlRegEx();
			// https://stackoverflow.com/questions/5284147/validating-ipv4-addresses-with-regexp
			regex->RegEx = L"^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$";
			regex->Subject = ip;
			if (regex->Match() && regex->MatchedText == ip)
				valid = true;
			delete regex;
		}
	}

	btnPortForward->Enabled = valid;
	if (valid)
	{
		String url;
		url.printf(L"http://%s", ip.w_str());
		btnPortForward->Hint = url;
	}

	return valid;
}

// ---------------------------------------------------------------------------
void TFormMap::AutoSelectGateway()
{
	bool gateFound = false;
	for (int i = 0; i < lbLocalGate->Items->Count; i++)
	{
		lbLocalGate->ItemIndex = i;
		if ((gateFound = SelectGateway()))
			break;
	}
	if (!gateFound)
	{
		lbLocalGate->ItemIndex = -1;
	}
}

// ---------------------------------------------------------------------------
#define SETUIVALUE(var, value) if ((var) != (value)) (var) = (value);
#define SETUIVALUE2(var, value, cmd1, cmd2) if ((var) != (value)) { (var) = (value); (cmd1); (cmd2); }

void TFormMap::UpdateMap()
{
	// localhost info
	String str;
	TStringList *list = new TStringList();
	TIdStackWindows *stack = new TIdStackWindows();
	str = stack->HostName + L"\r\n";
	stack->AddLocalAddressesToList(list);
	str += list->Text;
	SETUIVALUE(memoLocalHost->Text, str);
	SETUIVALUE(edtLocalPort->Text, Form1->edtPortLocal->Text);

	// gateway info
	try
	{
		String gate(t_GetGateways());
		SETUIVALUE2(lbLocalGate->Items->Text, gate, t_SetScrollWidth(lbLocalGate),
			AutoSelectGateway());
	}
	catch (Exception &ex)
	{
		SETUIVALUE(lbLocalGate->Items->Text, ex.Message);
	}
	if (Form1->edtPublicIP->Text.IsEmpty())
	{
		SETUIVALUE(memoPublicGate->Text,
			L"Press button (2) to determine the public IP address.");
	}
	else
	{
		SETUIVALUE(memoPublicGate->Text, Form1->edtPublicIP->Text);
	}
	SETUIVALUE(edtPublicPort->Text, Form1->edtPort->Text);

	delete stack;
	delete list;
}

// ---------------------------------------------------------------------------
