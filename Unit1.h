// ---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPServer.hpp>
#include <Vcl.ComCtrls.hpp>
#include <IdContext.hpp>
#include <System.SysUtils.hpp>
#include <IdGlobal.hpp>
#include <IdSocketHandle.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>

// ---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published: // IDE-managed Components
	TIdUDPServer *UDPServer;
	TEdit *edtPort;
	TUpDown *udPort;
	TButton *btnStart;
	TIdTCPServer *TCPServer;
	TRadioButton *rdbTCP;
	TRadioButton *rdbUDP;
	TMemo *txtLog;
	TTimer *Timer1;
	TButton *btnStop;
	TButton *btnGetPublicIP;
	TEdit *edtPublicIP;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TButton *btnOpenScanner;
	TComboBox *cbPublicIPService;
	TComboBox *cbScannerService;
	TLabel *Label7;
	TButton *btnCopyPort;
	TButton *btnCopyPublicIP;
	TCheckBox *chkAutoMode;
	TTimer *tmrGetPublicIP;

	void __fastcall TCPServerAfterBind(TObject *Sender);
	void __fastcall TCPServerConnect(TIdContext *AContext);
	void __fastcall TCPServerDisconnect(TIdContext *AContext);
	void __fastcall TCPServerStatus(TObject *ASender, const TIdStatus AStatus,
		const UnicodeString AStatusText);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall TCPServerExecute(TIdContext *AContext);
	void __fastcall UDPServerAfterBind(TObject *Sender);
	void __fastcall UDPServerStatus(TObject *ASender, const TIdStatus AStatus,
		const UnicodeString AStatusText);
	void __fastcall UDPServerUDPRead(TIdUDPListenerThread *AThread, const TIdBytes AData,
		TIdSocketHandle *ABinding);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall btnGetPublicIPClick(TObject *Sender);
	void __fastcall btnOpenScannerClick(TObject *Sender);
	void __fastcall btnCopyPortClick(TObject *Sender);
	void __fastcall btnCopyPublicIPClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall rdbTCPClick(TObject *Sender);
	void __fastcall edtPortChange(TObject *Sender);
	void __fastcall edtPortExit(TObject *Sender);
	void __fastcall cbPublicIPServiceEnter(TObject *Sender);
	void __fastcall tmrGetPublicIPTimer(TObject *Sender);
	void __fastcall chkAutoModeClick(TObject *Sender);

private: // User declarations
	void Log(String msg);
	void AddToMemo(String msg);
	void Stop();
	String FormatScannerServiceURL(String url, String ip, String port);
	void Load();
	void Save();
	void GetPublicIP();

public: // User declarations
	__fastcall TForm1(TComponent* Owner);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
