// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMap.h"

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
	memoLocalGate->Clear();
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
