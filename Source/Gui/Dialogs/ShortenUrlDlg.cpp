/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2015 Sergey Svistunov (zenden2k@gmail.com)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/
#include "ShortenUrlDlg.h"
#include "atlheaders.h"
#include "Func/Common.h"
#include "Core/3rdpart/pcreplusplus.h"
#include "LogWindow.h"
#include "Func/Settings.h"
#include "Gui/GuiTools.h"
#include <Core/Upload/FileQueueUploader.h>
#include <Core/Upload/UrlShorteningTask.h>
#include <Func/WinUtils.h>
#include <Wininet.h>
#include <Func/WebUtils.h>

// CShortenUrlDlg
CShortenUrlDlg::CShortenUrlDlg(CWizardDlg *wizardDlg,CMyEngineList * engineList, const CString &initialBuffer)
{
	m_WizardDlg = wizardDlg;
	m_InitialBuffer = initialBuffer;
	queueUploader_ = new CFileQueueUploader();
	queueUploader_->setCallback( this );
	engineList_ = engineList;
	serverId_ = engineList_->GetUploadEngineIndex(_T("Local Shorten server"));
	backgroundBrush_.CreateSysColorBrush(COLOR_BTNFACE);
	//serverId_ = engineList_->GetUploadEngineIndex(_T("clck.ru"));
}


CShortenUrlDlg::~CShortenUrlDlg()
{
	delete queueUploader_;
}

LRESULT CShortenUrlDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow(GetParent());
	PrevClipboardViewer = SetClipboardViewer();
	DlgResize_Init(false, true, 0); // resizable dialog without "griper"
 
	::SetFocus(GetDlgItem(IDOK));
	SetWindowText(TR("���������� ������"));
	TRC(IDOK, "���������");
	TRC(IDCANCEL, "�������");
	TRC(IDC_SHORTENURLTIP, "������� URL:");
	GuiTools::MakeLabelBold(GetDlgItem(IDC_RESULTSLABEL));

	::ShowWindow(GetDlgItem(IDC_DOWNLOADFILESPROGRESS), SW_HIDE);
	HWND hWnd = GetDlgItem(IDC_ANIMATIONSTATIC);
	if (hWnd) {
		wndAnimation_.SubclassWindow(hWnd);
		if (wndAnimation_.Load(MAKEINTRESOURCE(IDR_PROGRESSGIF), _T("GIF")))
			wndAnimation_.Draw();
		wndAnimation_.ShowWindow(SW_HIDE);
	}
	outputEditControl_.AttachToDlgItem(m_hWnd, IDC_RESULTSEDIT);

	CUploadEngineData *uploadEngine = _EngineList->byIndex( serverId_ );
	std::string selectedServerName = Settings.urlShorteningServer.uploadEngineData()->Name;
	int selectedIndex = 0;

	for( int i = 0; i < engineList_->count(); i++) {	
		CUploadEngineData * ue = _EngineList->byIndex( i ); 
		
		char *serverName = new char[ue->Name.length() + 1];
		lstrcpyA( serverName, ue->Name.c_str() );
		if ( ue->Type ==  CUploadEngineData::TypeUrlShorteningServer ) {
			int itemIndex = SendDlgItemMessage(IDC_SERVERCOMBOBOX, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Utf8ToWCstring( ue->Name ));
			if ( ue->Name == selectedServerName ){
				selectedIndex = itemIndex;
			}
			servers_.push_back(ue);
		}
	}

	SendDlgItemMessage(IDC_SERVERCOMBOBOX, CB_SETCURSEL, selectedIndex, 0);

	if(!m_InitialBuffer.IsEmpty())
	{
		ParseBuffer(m_InitialBuffer);
		//BeginDownloading(); 
	}

	CString clipboardText;
	WinUtils::GetClipboardText(clipboardText);
	if ( !clipboardText.IsEmpty() ) {
		ParseBuffer(clipboardText);
	}
	::SetFocus(GetDlgItem(IDC_INPUTEDIT));
	return 0; 
}


LRESULT CShortenUrlDlg::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{	
	CString url = GuiTools::GetDlgItemText(m_hWnd, IDC_INPUTEDIT);
	if ( url.IsEmpty() ) {
		return 0;
	}
	StartProcess();
	//BeginDownloading();
	return 0;
}

LRESULT CShortenUrlDlg::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if ( queueUploader_->IsRunning() ) {
		queueUploader_->stop();
		return 0;
	}
	/*if(m_FileDownloader.IsRunning()) 
		m_FileDownloader.stop();
	else
	{
		Settings.WatchClipboard = SendDlgItemMessage(IDC_WATCHCLIPBOARD, BM_GETCHECK) != 0;
		EndDialog(wID);
	}*/
	OnClose();
	EndDialog(wID);
	return 0;
}

LRESULT CShortenUrlDlg::OnChangeCbChain(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwndRemove = (HWND) wParam;  // handle of window being removed 
	HWND hwndNext = (HWND) lParam;

	if(hwndRemove == PrevClipboardViewer) PrevClipboardViewer = hwndNext;
	else ::SendMessage(PrevClipboardViewer, WM_CHANGECBCHAIN, wParam, lParam);
	return 0;
}

void CShortenUrlDlg::OnDrawClipboard()
{
	bool IsClipboard = IsClipboardFormatAvailable(CF_TEXT)!=0;

	if(IsClipboard && SendDlgItemMessage(IDC_WATCHCLIPBOARD,BM_GETCHECK)==BST_CHECKED && !m_FileDownloader.IsRunning()	)
	{
		CString str;  
		IU_GetClipboardText(str);
		//ParseBuffer(str, true);
		
	}
	//Sending WM_DRAWCLIPBOARD msg to the next window in the chain
	if(PrevClipboardViewer) ::SendMessage(PrevClipboardViewer, WM_DRAWCLIPBOARD, 0, 0); 
}

LRESULT CShortenUrlDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ChangeClipboardChain(PrevClipboardViewer);
	return 0;
}

bool CShortenUrlDlg::StartProcess() {
	int selectedIndex = SendDlgItemMessage(IDC_SERVERCOMBOBOX, CB_GETCURSEL, 0, 0);
	if ( selectedIndex < 0 ) {
		return false;
	}
	::ShowWindow(GetDlgItem(IDC_RESULTSLABEL), SW_SHOW);
	GuiTools::EnableDialogItem(m_hWnd, IDOK, false);
	wndAnimation_.ShowWindow(SW_SHOW);
	CString url = GuiTools::GetDlgItemText(m_hWnd, IDC_INPUTEDIT);

	std_tr::shared_ptr<UrlShorteningTask> task(new UrlShorteningTask(WCstringToUtf8(url)));

	CUploadEngineData *ue = servers_[selectedIndex];
	//CUploadEngineData* newData = new CUploadEngineData();
	//uploadEngineDataVector.push_back(std_tr::shared_ptr<CUploadEngineData>(newData));
	//*newData = *ue;
	CAbstractUploadEngine * e = engineList_->getUploadEngine(ue,Settings.urlShorteningServer.serverSettings());
	if ( !e ) {
		ProcessFinished();
		return false;
	}
	e->setUploadData(ue);
	
	ServerSettingsStruct& settings = Settings.urlShorteningServer.serverSettings();
	e->setServerSettings(settings);
	queueUploader_->AddUploadTask(task, 0, e);
	queueUploader_->start();

	return true;
}

bool CShortenUrlDlg::OnFileFinished(bool ok, CFileQueueUploader::FileListItem& result) {
	if ( ok ) {
		CString shortUrl = Utf8ToWCstring(result.imageUrl);
		SetDlgItemText(IDC_RESULTSEDIT, shortUrl);
		WinUtils::CopyTextToClipboard(shortUrl);
		SetDlgItemText(IDC_RESULTSLABEL, TR("�������� ������ ����������� � ����� ������!"));
		::ShowWindow(GetDlgItem(IDC_RESULTSLABEL), SW_SHOW);
		::SetFocus(GetDlgItem(IDC_RESULTSEDIT));
	}
	return true;
}

bool CShortenUrlDlg::OnQueueFinished(CFileQueueUploader* queueUploader) {
	ProcessFinished();
	return true;
}

void CShortenUrlDlg::ProcessFinished() {
	GuiTools::EnableDialogItem(m_hWnd, IDOK, true);
	wndAnimation_.ShowWindow(SW_HIDE);
}

void CShortenUrlDlg::OnClose() {
	int selectedIndex = SendDlgItemMessage(IDC_SERVERCOMBOBOX, CB_GETCURSEL, 0, 0);
	if ( selectedIndex >= 0 ) {
		CUploadEngineData *ue = servers_[selectedIndex];
		Settings.urlShorteningServer.setServerName( Utf8ToWCstring(ue->Name));
	}
}

bool CShortenUrlDlg::ParseBuffer(const CString& text) {
	CString textCopy = text;
	if (  WebUtils::DoesTextLookLikeUrl(textCopy) ) {
		SetDlgItemText(IDC_INPUTEDIT, textCopy);
	}
	return false;
}

LRESULT CShortenUrlDlg::OnCtlColorMsgDlg(HDC hdc, HWND hwnd) {
	if ( hwnd == GetDlgItem(IDC_RESULTSLABEL ) ) {
		SetTextColor(hdc, RGB(0,180,0));
		SetBkMode(hdc, TRANSPARENT);
		return (LRESULT)(HBRUSH) backgroundBrush_; 
	}
	return 0;
}

bool  CShortenUrlDlg::OnConfigureNetworkClient(CFileQueueUploader* ,NetworkClient* nm) {
	IU_ConfigureProxy(*nm);
	return true;
}


