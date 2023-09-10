// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

// #include <sstream>
// #include <iostream>
// #include <iomanip>
#include <list>
#include <System.Net.HttpClient.hpp>
#include <System.SysUtils.hpp>
#include <XMLDoc.hpp>

#include "Unit1.h"
#include "tools.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

namespace config
{
	const String fileName(tl_GetProgramPath() + Application->Title + L".xml");

	int serverTimeout = 120; // how long server runs (seconds)
	int tcpReadTimeout = 500; // TCP I/O reading timeout (ms)
	std::list<String>tcpServLst;
	std::list<String>udpServLst;
	std::list<String>ipServLst;
	String tcpService;
	String udpService;
	String ipService;
	int portNumber = 1;
	bool isProtocolTCP = true;
	bool isAutoMode = false;
	bool usePortCheck = false;
	String PortCheckTcpUrl =
		L"http://portchecker.portforward.com/portcheck3.cgi?port={{PORT}}&protocol=tcp&version=0";
	String PortCheckUdpUrl =
		L"http://portchecker.portforward.com/portcheck3.cgi?port={{PORT}}&protocol=udp&version=0";
	String PortCheckUserAgent = L"PFPortChecker/1.0";
}

// ---------------------------------------------------------------------------
class PortCheckThread : public TThread
{
protected:
	virtual void __fastcall Execute();

public:
	__fastcall PortCheckThread(String url);

private:
	String _url;
};

__fastcall PortCheckThread::PortCheckThread(String url) : TThread(true)
{
	FreeOnTerminate = true;
	// setup other thread parameters as needed...
	_url = url;
}

void __fastcall PortCheckThread::Execute()
{
	// do work here ...
	// if you need to access the UI controls,
	// use the TThread::Synchronize() method for that

	TIdHTTP *http = new TIdHTTP(NULL);
	http->ProtocolVersion = pv1_0; // HTTP 1.0
	http->Request->UserAgent = L"PFPortChecker/1.0";
	http->Request->Accept = L"";
	http->Request->Connection = L"close";
	try
	{
		try
		{
			Form1->Log(L"http →Request service...");
			String result = http->Get(_url);
			Form1->Log((String)L"http ←Responce: " + result);
		}
		catch (Exception &ex)
		{
			Form1->Log((String)L"http ←Error: " + ex.Message);
		}
	}
	__finally
	{
		delete http;
	}
}
// How to use:
// TMyThread *thrd = new TMyThread();
// thrd->OnTerminate = &ThreadTerminated;
// thrd->Resume();

// ---------------------------------------------------------------------------
void __fastcall TForm1::PortCheckThreadTerminated(TObject *Sender)
{
	// thread is finished with its work ...
	btnOpenScanner->Enabled = true;
}

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	chkAutoMode->Hint =
		L"◆ One click mode ◆\n" L"Auto-press buttons ② and ③ after server is started with ①\n"
		L"Saves extra manual clicks.";

	reBottomMsg->Clear();
	reBottomMsg->SelText = L"↑ If a ";
	reBottomMsg->SelAttributes->Style = TFontStyles() << fsUnderline;
	reBottomMsg->SelText = L"TCP Connect";
	reBottomMsg->SelAttributes->Style = TFontStyles();
	reBottomMsg->SelText = L" or ";
	reBottomMsg->SelAttributes->Style = TFontStyles() << fsUnderline;
	reBottomMsg->SelText = L"UDP Get";
	reBottomMsg->SelAttributes->Style = TFontStyles();
	reBottomMsg->SelText =
		L" response is received, then the Port is accessible for this protocol, otherwise the Port is not accessible.";

	Load();
	UpdateGUI();
}

// ---------------------------------------------------------------------------
void TForm1::Log(String msg)
{
	tl_RunInMainThread(AddToMemo, msg);
}

// ---------------------------------------------------------------------------
void TForm1::AddToMemo(String msg)
{
	String line;
	line.printf(L"%s | %s", FormatDateTime(L"hh:nn:ss", Now()).w_str(), msg.w_str());
	txtLog->Lines->Add(line);
}

// ---------------------------------------------------------------------------
void TForm1::Stop()
{
	Timer1->Enabled = false;
	btnStart->Caption = L"① Start";
	this->FormStyle = fsNormal;

	if (!TCPServer->Active && !UDPServer->Active)
		return;
	TCPServer->Active = false;
	UDPServer->Active = false;
	Log(L"SERVER STOPPED");
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnStartClick(TObject *Sender)
{
	Stop();
	txtLog->Clear();

	std::list<String> *list;
	String *service;

	if (config::isProtocolTCP)
	{
		TCPServer->Bindings->Clear();
		TCPServer->DefaultPort = udPort->Position;
		TCPServer->Active = true;
		list = &config::tcpServLst;
		service = &config::tcpService;
	}
	else
	{
		UDPServer->Bindings->Clear();
		UDPServer->DefaultPort = udPort->Position;
		UDPServer->Active = true;
		list = &config::udpServLst;
		service = &config::udpService;
	}

	cbScannerService->Items->Clear();
	for (String i : *list)
		cbScannerService->Items->Add(i);
	if (service->IsEmpty())
		cbScannerService->ItemIndex = 0;
	else
		cbScannerService->Text = *service;

	btnStart->Caption = config::serverTimeout;
	Timer1->Tag = config::serverTimeout;
	Timer1->Enabled = true;
	this->FormStyle = fsStayOnTop;

	if (config::isAutoMode)
	{
		btnGetPublicIPClick(NULL);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnStopClick(TObject *Sender)
{
	Stop();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	Timer1->Tag = Timer1->Tag - 1;
	if (Timer1->Tag <= 0)
	{
		Stop();
	}
	else
	{
		btnStart->Caption = Timer1->Tag;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TCPServerAfterBind(TObject *Sender)
{
	Log((String)L"TCP LISTEN on port " + TCPServer->Bindings->DefaultPort);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TCPServerConnect(TIdContext *AContext)
{
	String ip = AContext->Connection->Socket->Binding->PeerIP;
	String msg;
	msg.printf(L"TCP ←Connect [%s]", ip.w_str());
	Log(msg);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TCPServerDisconnect(TIdContext *AContext)
{
	String ip = AContext->Connection->Socket->Binding->PeerIP;
	String msg;
	msg.printf(L"TCP ×Disconnect [%s]", ip.w_str());
	Log(msg);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TCPServerStatus(TObject *ASender, const TIdStatus AStatus,
	const UnicodeString AStatusText)
{
	Log((String)L"TCPServerStatus " + AStatusText);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TCPServerExecute(TIdContext *AContext)
{
	String ip = AContext->Connection->Socket->Binding->PeerIP;
	AContext->Connection->IOHandler->ReadTimeout = config::tcpReadTimeout;
	TIdBytes bytes;
	try
	{
		while (true)
			AContext->Connection->IOHandler->ReadBytes(bytes, 1, true);
	}
	catch (EIdReadTimeout&)
	{
		if (bytes.Length)
		{
			String msg;
			msg.printf(L"TCP ←Get [%s] (%d): %s", ip.w_str(), bytes.Length,
				BytesToHexStr(bytes).w_str());
			Log(msg);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UDPServerAfterBind(TObject *Sender)
{
	Log((String)L"UDP LISTEN on port " + UDPServer->Bindings->DefaultPort);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UDPServerStatus(TObject *ASender, const TIdStatus AStatus,
	const UnicodeString AStatusText)
{
	Log((String)L"UDPServerStatus " + AStatusText);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UDPServerUDPRead(TIdUDPListenerThread *AThread,
	const TIdBytes AData, TIdSocketHandle *ABinding)
{
	String ip = ABinding->PeerIP;
	int len = AData.Length;
	String data = BytesToHexStr(AData);
	if ((len > 15 && len < 100) && data.Pos
		(L"50 6F 72 74 46 6F 72 77 61 72 64 2E 63 6F 6D") != 0)
	{
		len = 2;
		data = L"4F 4B";
	}
	String msg;
	msg.printf(L"UDP ←Get [%s] (%d): %s", ip.w_str(), len, data.w_str());
	Log(msg);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnGetPublicIPClick(TObject *Sender)
{
	edtPublicIP->Text = L"";
	btnGetPublicIP->Enabled = false;
	btnOpenScanner->Enabled = false;

	tmrGetPublicIP->Enabled = true; // Call function
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::tmrGetPublicIPTimer(TObject *Sender)
{
	tmrGetPublicIP->Enabled = false;
	GetPublicIP();
}

// ---------------------------------------------------------------------------
void TForm1::GetPublicIP()
{
	THTTPClient *client = THTTPClient::Create();
	client->UserAgent = L"curl/7.55.1";
	try
	{
		edtPublicIP->Text = client->Get(cbPublicIPService->Text)->ContentAsString()
			.Trim();
	}
	__finally
	{
		delete client;
		btnGetPublicIP->Enabled = true;
		btnOpenScanner->Enabled = true;
	}
	// update valid Public IP service
	config::ipService = cbPublicIPService->Text;

	if (config::isAutoMode)
	{
		btnOpenScannerClick(NULL);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnOpenScannerClick(TObject *Sender)
{
	if (!TCPServer->Active && !UDPServer->Active)
	{
		MessageBox(Handle, L"Server is not started.", Application->Title.w_str(),
			MB_OK | MB_ICONWARNING);
		return;
	}
	edtPublicIP->Text = edtPublicIP->Text.Trim();
	if (edtPublicIP->Text.IsEmpty())
	{
		MessageBox(Handle, L"No public IP address.", Application->Title.w_str(),
			MB_OK | MB_ICONWARNING);
		return;
	}

	// use PortCheck service
	if (config::usePortCheck)
	{
		btnOpenScanner->Enabled = false;

		String url;
		int port;
		if (TCPServer->Active)
		{
			url = config::PortCheckTcpUrl;
			port = TCPServer->Bindings->DefaultPort;
		}
		else
		{
			url = config::PortCheckUdpUrl;
			port = UDPServer->Bindings->DefaultPort;
		}
		PortCheckThread *thrd =
			new PortCheckThread(FormatScannerServiceURL(url, L"", port));
		thrd->OnTerminate = &PortCheckThreadTerminated;
		thrd->Resume();
		return;
	}

	// use web service
	cbScannerService->Text = cbScannerService->Text.Trim();
	const String &url = cbScannerService->Text;
	if (url.Pos(L"http") == 1 || url.Pos(L"HTTP") == 1); //
	else
	{
		String msg;
		msg.printf(L"Invalid URL: \"%s\"", url.w_str());
		MessageBox(Handle, msg.w_str(), Application->Title.w_str(), MB_OK | MB_ICONERROR);
		return;
	}

	String *service;
	String port;
	if (TCPServer->Active)
	{
		port = TCPServer->DefaultPort;
		service = &config::tcpService;
	}
	else
	{
		port = UDPServer->DefaultPort;
		service = &config::udpService;
	}
	// update valid Scanner service
	*service = cbScannerService->Text;
	ShellExecute(NULL, L"open", FormatScannerServiceURL(url, edtPublicIP->Text,
		port).w_str(), NULL, NULL, SW_NORMAL);
}

// ---------------------------------------------------------------------------
String TForm1::FormatScannerServiceURL(String url, String ip, String port)
{
	url = StringReplace(url, "{{IP}}", ip, TReplaceFlags() << rfReplaceAll);
	url = StringReplace(url, "{{PORT}}", port, TReplaceFlags() << rfReplaceAll);
	return url;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnCopyPortClick(TObject *Sender)
{
	edtPort->SelectAll();
	edtPort->CopyToClipboard();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnCopyPublicIPClick(TObject *Sender)
{
	edtPublicIP->SelectAll();
	edtPublicIP->CopyToClipboard();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Stop();
	Save();
}

// ---------------------------------------------------------------------------
void TForm1::Load()
{
	if (!FileExists(config::fileName))
	{
		config::tcpServLst.clear();
		config::tcpServLst.push_back
			(L"https://check-host.net/check-tcp?host={{IP}}:{{PORT}}");
		config::tcpServLst.push_back
			(L"http://tools.eti.pw/port-scanner.php?addr={{IP}}&port={{PORT}}");
		config::tcpServLst.push_back(L"https://2ip.io/check-port/?port={{PORT}}");
		config::tcpServLst.push_back(L"https://portscaner.com/");
		config::tcpServLst.push_back(L"https://portchecker.co/");
		config::tcpServLst.push_back(L"https://www.portcheckers.com/");
		config::tcpServLst.push_back(L"https://router-network.com/tools/port-checker");
		config::tcpServLst.push_back(L"https://dnschecker.org/port-scanner.php");
		config::tcpServLst.push_back(L"https://myopenports.com/");
		config::tcpServLst.push_back(L"https://www.portchecktool.com/");
		config::tcpServLst.push_back(L"https://port.tools/port-checker-ipv4/");
		config::tcpServLst.push_back(L"https://www.yougetsignal.com/tools/open-ports/");
		config::tcpServLst.push_back
			(L"https://videos.cctvcamerapros.com/check-open-port-forwarding-tool");
		config::tcpServLst.push_back(L"https://www.htmlstrip.com/open-port-checker");
		config::tcpServLst.push_back(L"https://www.dynu.com/NetworkTools/PortCheck");
		config::tcpServLst.push_back(L"https://www.dnsissue.com/port-checker/index.php");

		config::udpServLst.clear();
		config::udpServLst.push_back
			(L"https://check-host.net/check-udp?host={{IP}}:{{PORT}}");

		config::ipServLst.clear();
		config::ipServLst.push_back(L"http://icanhazip.com");
		config::ipServLst.push_back(L"http://whatismyip.akamai.com");
		config::ipServLst.push_back(L"http://api.ipify.org");
		config::ipServLst.push_back(L"http://ifconfig.net");

		Save();
	}

	CoInitialize(0);
	String str;
	const _di_IXMLDocument document = interface_cast<Xmlintf::IXMLDocument>
		(new TXMLDocument(NULL));
	document->LoadFromFile(config::fileName);

	// find root node
	const _di_IXMLNode root = document->ChildNodes->FindNode(L"Config");
	// get attributes
	if (root->HasAttribute(L"serverTimeout"))
		config::serverTimeout = (int)root->Attributes[L"serverTimeout"];

	if (root->HasAttribute(L"tcpReadTimeout"))
		config::tcpReadTimeout = (int)root->Attributes[L"tcpReadTimeout"];

	if (root->HasAttribute(L"tcpService"))
		config::tcpService = root->Attributes[L"tcpService"];

	if (root->HasAttribute(L"udpService"))
		config::udpService = root->Attributes[L"udpService"];

	if (root->HasAttribute(L"ipService"))
		config::ipService = root->Attributes[L"ipService"];

	if (root->HasAttribute(L"portNumber"))
	{
		config::portNumber = (int)root->Attributes[L"portNumber"];
		udPort->Position = config::portNumber; // GUI
	}

	if (root->HasAttribute(L"isProtocolTCP"))
	{
		str = root->Attributes[L"isProtocolTCP"];
		config::isProtocolTCP = str == L"true";
		if (config::isProtocolTCP)
			rdbTCP->Checked = true; // GUI
		else
			rdbUDP->Checked = true;
	}

	if (root->HasAttribute(L"isAutoMode"))
	{
		str = root->Attributes[L"isAutoMode"];
		config::isAutoMode = str == L"true";
		chkAutoMode->Checked = config::isAutoMode; // GUI
	}

	if (root->HasAttribute(L"usePortCheck"))
	{
		str = root->Attributes[L"usePortCheck"];
		config::usePortCheck = str == L"true";
		chkPortCheck->Checked = config::usePortCheck; // GUI
	}

	// find TCP services node
	config::tcpServLst.clear();
	_di_IXMLNode node0 = root->ChildNodes->FindNode(L"TcpServices");
	if (node0 != NULL)
	{
		for (int i = 0; i < node0->ChildNodes->Count; i++)
		{
			const _di_IXMLNode node = node0->ChildNodes->Get(i);
			if (node->HasAttribute(L"Url"))
			{
				str = node->Attributes[L"Url"];
				config::tcpServLst.push_back(str);
			}
		}
	}

	// find UDP services node
	config::udpServLst.clear();
	node0 = root->ChildNodes->FindNode(L"UdpServices");
	if (node0 != NULL)
	{
		for (int i = 0; i < node0->ChildNodes->Count; i++)
		{
			const _di_IXMLNode node = node0->ChildNodes->Get(i);
			if (node->HasAttribute(L"Url"))
			{
				str = node->Attributes[L"Url"];
				config::udpServLst.push_back(str);
			}
		}
	}

	// find IP services node
	config::ipServLst.clear();
	node0 = root->ChildNodes->FindNode(L"IpServices");
	if (node0 != NULL)
	{
		for (int i = 0; i < node0->ChildNodes->Count; i++)
		{
			const _di_IXMLNode node = node0->ChildNodes->Get(i);
			if (node->HasAttribute(L"Url"))
			{
				str = node->Attributes[L"Url"];
				config::ipServLst.push_back(str);
			}
		}
	}
	cbPublicIPService->Items->Clear();
	for (String i : config::ipServLst)
		cbPublicIPService->Items->Add(i);
	if (config::ipService.IsEmpty())
		cbPublicIPService->ItemIndex = 0;
	else
		cbPublicIPService->Text = config::ipService;

	// find PortCheck service node
	node0 = root->ChildNodes->FindNode(L"PortCheck");
	if (node0 != NULL)
	{
		config::PortCheckTcpUrl = node0->Attributes[L"TcpUrl"];
		config::PortCheckUdpUrl = node0->Attributes[L"UdpUrl"];
		config::PortCheckUserAgent = node0->Attributes[L"UserAgent"];
	}

	document->Release();
	CoUninitialize();
}

// ---------------------------------------------------------------------------
void TForm1::Save()
{
	CoInitialize(0);
	String str1;
	_di_IXMLDocument document = interface_cast<Xmlintf::IXMLDocument>
		(new TXMLDocument(NULL));
	document->Active = true;
	document->SetEncoding(L"UTF-8");
	document->Options = document->Options << doNodeAutoIndent;
	_di_IXMLNode root = document->CreateNode(L"Config", ntElement, L"");
	document->DocumentElement = root;

	root->Attributes[L"serverTimeout"] = config::serverTimeout;
	root->Attributes[L"tcpReadTimeout"] = config::tcpReadTimeout;
	root->Attributes[L"tcpService"] = config::tcpService;
	root->Attributes[L"udpService"] = config::udpService;
	root->Attributes[L"ipService"] = config::ipService;
	root->Attributes[L"portNumber"] = config::portNumber;
	root->Attributes[L"isProtocolTCP"] = config::isProtocolTCP;
	root->Attributes[L"isAutoMode"] = config::isAutoMode;
	root->Attributes[L"usePortCheck"] = config::usePortCheck;

	_di_IXMLNode node0 = document->CreateNode(L"TcpServices", ntElement, L"");
	root->ChildNodes->Add(node0);
	for (String i : config::tcpServLst)
	{
		_di_IXMLNode node1 = document->CreateNode(L"Item", ntElement, L"");
		node1->Attributes[L"Url"] = i;
		node0->ChildNodes->Add(node1);
	}

	node0 = document->CreateNode(L"UdpServices", ntElement, L"");
	root->ChildNodes->Add(node0);
	for (String i : config::udpServLst)
	{
		_di_IXMLNode node1 = document->CreateNode(L"Item", ntElement, L"");
		node1->Attributes[L"Url"] = i;
		node0->ChildNodes->Add(node1);
	}

	node0 = document->CreateNode(L"IpServices", ntElement, L"");
	root->ChildNodes->Add(node0);
	for (String i : config::ipServLst)
	{
		_di_IXMLNode node1 = document->CreateNode(L"Item", ntElement, L"");
		node1->Attributes[L"Url"] = i;
		node0->ChildNodes->Add(node1);
	}

	node0 = document->CreateNode(L"PortCheck", ntElement, L"");
	root->ChildNodes->Add(node0);
	node0->Attributes[L"TcpUrl"] = config::PortCheckTcpUrl;
	node0->Attributes[L"UdpUrl"] = config::PortCheckUdpUrl;
	node0->Attributes[L"UserAgent"] = config::PortCheckUserAgent;

	document->SaveToFile(config::fileName);
	document->Release();
	CoUninitialize();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::rdbTCPClick(TObject *Sender)
{
	config::isProtocolTCP = rdbTCP->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::edtPortChange(TObject *Sender)
{
	config::portNumber = udPort->Position;
}

void __fastcall TForm1::edtPortExit(TObject *Sender)
{
	edtPort->Text = udPort->Position;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::chkAutoModeClick(TObject *Sender)
{
	config::isAutoMode = chkAutoMode->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::chkPortCheckClick(TObject *Sender)
{
	config::usePortCheck = chkPortCheck->Checked;
	UpdateGUI();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::cbPublicIPServiceEnter(TObject *Sender)
{
	// suppress autoselect text in comboboxes
	PostMessage(cbPublicIPService->Handle, CB_SETEDITSEL, (WPARAM) - 1, 0);
	PostMessage(cbScannerService->Handle, CB_SETEDITSEL, (WPARAM) - 1, 0);
}

// ---------------------------------------------------------------------------
void TForm1::UpdateGUI()
{
	if (config::usePortCheck)
	{
		btnOpenScanner->Caption = L"③ Call";
		btnOpenScanner->Hint = L"Query PortCheck™ service";
		cbScannerService->Color = clBtnFace;
		cbScannerService->Enabled = false;
		lblScannerService->Enabled = false;
	}
	else
	{
		btnOpenScanner->Caption = L"③ Open";
		btnOpenScanner->Hint = L"Open port check website in Browser";
		cbScannerService->Color = clWindow;
		cbScannerService->Enabled = true;
		lblScannerService->Enabled = true;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Help1Click(TObject *Sender)
{
	String str =
		L"If your internet router is configured for automatic UPnP port forwarding, you can use the included UPnP Wizard tool to manually forward a port for testing.\nAfter the testing, do not forget to delete the created port mappings or create them with a Lease Duration of say 120 seconds so that they will be removed automatically after the timeout expires.";
	MessageBox(Handle, str.w_str(), L"UPnP Information", MB_OK | MB_ICONINFORMATION);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::OpenUPnPWizard1Click(TObject *Sender)
{
	String runcmd;
	runcmd.printf(L"%supnp\\UPNPWizard.exe", tl_GetProgramPath().w_str());
	t_RunProcess(runcmd);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnUpnpClick(TObject *Sender)
{
	TPoint pos = ClientToScreen(TPoint(btnUpnp->Left, btnUpnp->Top + btnUpnp->Height));
	menuUpnp->Popup(pos.X, pos.Y);
}

// ---------------------------------------------------------------------------
