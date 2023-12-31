// ---------------------------------------------------------------------------

#ifndef UnitMapH
#define UnitMapH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ActnMan.hpp>
#include <Vcl.PlatformDefaultStyleActnCtrls.hpp>
#include <Vcl.ComCtrls.hpp>

// ---------------------------------------------------------------------------
class TFormMap : public TForm
{
__published: // IDE-managed Components
	TMemo *memoLocalHost;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *edtLocalPort;
	TLabel *Label3;
	TShape *shapeLAN;
	TLabel *Label4;
	TShape *shapeWAN;
	TLabel *Label5;
	TPanel *Panel1;
	TLabel *Label6;
	TMemo *memoPublicGate;
	TEdit *edtPublicPort;
	TLabel *Label7;
	TLabel *Label8;
	TShape *Shape2;
	TLabel *Label10;
	TLabel *Label11;
	TShape *Shape3;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TActionManager *ActionManager1;
	TAction *ActionHide;
	TButton *btnSysInfo;
	TListBox *lbLocalGate;
	TLabel *Label17;
	TButton *btnPortForward;

	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ActionHideExecute(TObject *Sender);
	void __fastcall btnSysInfoClick(TObject *Sender);
	void __fastcall btnPortForwardClick(TObject *Sender);
	void __fastcall lbLocalGateClick(TObject *Sender);

private: // User declarations
protected:
	virtual void __fastcall CreateParams(TCreateParams &Params);

public: // User declarations
	__fastcall TFormMap(TComponent* Owner);

	void Clear();
	bool SelectGateway();
    void AutoSelectGateway();
	void UpdateMap();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFormMap *FormMap;
// ---------------------------------------------------------------------------
#endif
