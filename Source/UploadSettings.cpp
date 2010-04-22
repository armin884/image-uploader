/*
    Image Uploader - program for uploading images/files to Internet
    Copyright (C) 2007-2010 ZendeN <zenden2k@gmail.com>
	 
    HomePage:    http://zenden.ws/imageuploader

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

#include "UploadSettings.h"
#include "ServerFolderSelect.h"
#include "NewFolderDlg.h"
#include "ServerParamsDlg.h"

CUploadSettings::CUploadSettings()
{
	nImageIndex = nFileIndex = -1;
}

CUploadSettings::~CUploadSettings()
{
}

LRESULT CUploadSettings::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	PageWnd = m_hWnd;
	
	
	TRC(IDC_FORMATLABEL,"������:");
	TRC(IDC_QUALITYLABEL,"��������:");
	TRC(IDC_RESIZEBYWIDTH,"��������� ������:");
	TRC(IDC_SAVEPROPORTIONS,"��������� ���������");
	TRC(IDC_YOURLOGO,"�������� ������� ����");
	TRC(IDC_XLABEL,"�/��� ������:");
	TRC(IDC_YOURTEXT,"�������� ����� �� ��������");
	TRC(IDC_IMAGEPARAMETERS,"��������� �����������");
	TRC(IDC_LOGOOPTIONS,"�������������...");
	
	TRC(IDC_KEEPASIS,"�������� ��� ���������");
	TRC(IDC_THUMBSETTINGS,"��������� �������");

	TRC(IDC_CREATETHUMBNAILS,"��������� ���������");
	TRC(IDC_IMAGESERVERGROUPBOX,"C����� ��� �������� �����������");
	TRC(IDC_USESERVERTHUMBNAILS,"������������ ��������� ������");
	TRC(IDC_WIDTHLABEL,"������ ������:");
	TRC(IDC_DRAWFRAME,"�������� � �����");
	TRC(IDC_ADDFILESIZE,"������� ����������� �� ��������");
	/*TRC(IDC_LOGINTOOLBUTTON,"�����������");
	TRC(IDC_LOGINTOOLBUTTON+1,"�����������");*/
	TRC(IDC_PRESSUPLOADBUTTON,"������� ������ \"���������\" ����� ������ ������� ��������");
	TRC(IDC_USETHUMBTEMPLATE,"������������ ������");
	TRC(IDC_TEXTOVERTHUMB2, "������� ������ ������");
	TRC(IDC_FILESERVERGROUPBOX, "������ ��� ��������� ����� ������");

	m_nImageServer = Settings.ServerID;
	m_nFileServer = Settings.FileServerID;

	HBITMAP hBitmap;

	// Get color depth (minimum requirement is 32-bits for alpha blended images).
	int iBitsPixel = GetDeviceCaps(::GetDC(HWND_DESKTOP),BITSPIXEL);
	HIMAGELIST m_hToolBarImageList;
	/*if (iBitsPixel >= 32)
	{
		hBitmap = LoadBitmap(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDB_BITMAP5));
		m_PlaceSelectorImageList.Create(16,16,ILC_COLOR32,0,6);
		m_PlaceSelectorImageList.Add(hBitmap, (HBITMAP) NULL);
	}
	else*/
	{
		hBitmap = LoadBitmap(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDB_SERVERTOOLBARBMP2));
		m_PlaceSelectorImageList.Create(16,16,ILC_COLOR32 | ILC_MASK,0,6);
		m_PlaceSelectorImageList.Add(hBitmap,RGB(255,0,255));
	}
	
	

	RECT rc = {10,250,400,290};
	GetClientRect(&rc);
	
	rc.top = 272;
	rc.bottom  = rc.top +25;
	rc.right -=40;
	rc.left+=38;

	for(int i = 0; i<2; i++)
	{
		CToolBarCtrl& CurrentToolbar = (i == 0) ? Toolbar: FileServerSelectBar;
		CurrentToolbar.Create(m_hWnd,rc,_T(""), WS_CHILD|WS_VISIBLE|WS_CHILD | TBSTYLE_LIST |TBSTYLE_FLAT| CCS_NORESIZE|/*CCS_BOTTOM |CCS_ADJUSTABLE|*/CCS_NODIVIDER|TBSTYLE_AUTOSIZE  );
		
		CurrentToolbar.SetButtonStructSize();
		CurrentToolbar.SetButtonSize(30,18);
		CurrentToolbar.SetImageList(m_PlaceSelectorImageList);
		
		CurrentToolbar.AddButton(IDC_SERVERBUTTON, TBSTYLE_DROPDOWN |BTNS_AUTOSIZE, TBSTATE_ENABLED, -1, TR("�������� ������..."), 0);
		CurrentToolbar.AddButton(IDC_TOOLBARSEPARATOR1, TBSTYLE_BUTTON |BTNS_AUTOSIZE, TBSTATE_ENABLED, 2, TR(""), 0);
		
		CurrentToolbar.AddButton(IDC_LOGINTOOLBUTTON + !i, /*TBSTYLE_BUTTON*/TBSTYLE_DROPDOWN |BTNS_AUTOSIZE, TBSTATE_ENABLED, 0, _T(""), 0);
		CurrentToolbar.AddButton(IDC_TOOLBARSEPARATOR2, TBSTYLE_BUTTON |BTNS_AUTOSIZE, TBSTATE_ENABLED, 2, TR(""), 0);
		
		CurrentToolbar.AddButton(IDC_SELECTFOLDER, TBSTYLE_BUTTON |BTNS_AUTOSIZE, TBSTATE_ENABLED, 1, TR("�������� �����..."), 0);
		CurrentToolbar.AutoSize();
		rc.top = 332;
		rc.bottom  = rc.top +25;
	}

	Toolbar.SetWindowLong(GWL_ID, IDC_IMAGETOOLBAR);
	FileServerSelectBar.SetWindowLong(GWL_ID, IDC_FILETOOLBAR);

	SendDlgItemMessage(IDC_THUMBWIDTHSPIN, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short)40, (short)1000) );
	

	SendDlgItemMessage(IDC_QUALITYSPIN,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)100, (short)1));
	
	SendDlgItemMessage(IDC_FORMATLIST,CB_ADDSTRING,0,(LPARAM)TR("����"));
	SendDlgItemMessage(IDC_FORMATLIST,CB_ADDSTRING,0,(LPARAM)_T("JPEG"));
	SendDlgItemMessage(IDC_FORMATLIST,CB_ADDSTRING,0,(LPARAM)_T("PNG"));
	SendDlgItemMessage(IDC_FORMATLIST,CB_ADDSTRING,0,(LPARAM)_T("GIF"));

	ShowParams();

	UpdateAllPlaceSelectors();


	//int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;

	//SquirrelVM::Init();

	
	//UploadEngine &ue = EnginesList[m_nImageServer];
	//CUploadScript *m_pluginLoader = iuPluginManager.getPlugin(ue.PluginName, Settings.ServersSettings[ue.Name]);
	return 1;  // Let the system set the focus
}

LRESULT CUploadSettings::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CUploadSettings::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}


LRESULT CUploadSettings::OnBnClickedKeepasis(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	int id;
	BOOL checked = SendDlgItemMessage(IDC_KEEPASIS, BM_GETCHECK, 0, 0);

	for(id=1004; id<1017; id++)
		::EnableWindow(GetDlgItem(id), !checked);

	::EnableWindow(GetDlgItem(IDC_SAVEPROPORTIONS), !checked);

	return 0;
}

void CUploadSettings::ShowParams(/*UPLOADPARAMS params*/)
{
	SendDlgItemMessage(IDC_KEEPASIS,BM_SETCHECK,Settings.ImageSettings.KeepAsIs);
	if(Settings.ImageSettings.NewWidth)
		SetDlgItemInt(IDC_IMAGEWIDTH,Settings.ImageSettings.NewWidth);
	else
		SetDlgItemText(IDC_IMAGEWIDTH,_T(""));

	if(Settings.ImageSettings.NewHeight)
		SetDlgItemInt(IDC_IMAGEHEIGHT,Settings.ImageSettings.NewHeight);
	else
		SetDlgItemText(IDC_IMAGEHEIGHT,_T(""));

	if(Settings.ImageSettings.Quality)
		SetDlgItemInt(IDC_QUALITYEDIT,Settings.ImageSettings.Quality);
	else
		SetDlgItemText(IDC_QUALITYEDIT,_T(""));


	SendDlgItemMessage(IDC_YOURLOGO,BM_SETCHECK, Settings.ImageSettings.AddLogo);
	SendDlgItemMessage(IDC_YOURTEXT,BM_SETCHECK, Settings.ImageSettings.AddText);
	SendDlgItemMessage(IDC_DRAWFRAME,BM_SETCHECK, Settings.ThumbSettings.DrawFrame);
	SendDlgItemMessage(IDC_FORMATLIST,CB_SETCURSEL, Settings.ImageSettings.Format);
	SendDlgItemMessage(IDC_CREATETHUMBNAILS,BM_SETCHECK,Settings.ThumbSettings.CreateThumbs);
	SendDlgItemMessage(IDC_SAVEPROPORTIONS,BM_SETCHECK,Settings.ImageSettings.SaveProportions);
	SendDlgItemMessage(IDC_ADDFILESIZE,BM_SETCHECK,Settings.ThumbSettings.ThumbAddImageSize);
	SendDlgItemMessage(IDC_USETHUMBTEMPLATE,BM_SETCHECK,Settings.ThumbSettings.UseThumbTemplate);
	if(!Settings.ThumbSettings.UseThumbTemplate)
		SendDlgItemMessage(IDC_USESERVERTHUMBNAILS,BM_SETCHECK,Settings.ThumbSettings.UseServerThumbs);

	SendDlgItemMessage(IDC_TEXTOVERTHUMB2, BM_SETCHECK, Settings.ThumbSettings.TextOverThumb);


	SendDlgItemMessage(IDC_CREATETHUMBNAILS,BM_SETCHECK,Settings.ThumbSettings.CreateThumbs);


	SetDlgItemInt(IDC_THUMBWIDTH,Settings.ThumbSettings.ThumbWidth);
}

LRESULT CUploadSettings::OnBnClickedCreatethumbnails(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	int id;
	BOOL checked = SendDlgItemMessage(IDC_CREATETHUMBNAILS, BM_GETCHECK, 0, 0);
	
	::EnableWindow(GetDlgItem(IDC_THUMBWIDTH), checked);
	::EnableWindow(GetDlgItem(IDC_ADDFILESIZE), checked);
	::EnableWindow(GetDlgItem(IDC_WIDTHLABEL), checked);
	::EnableWindow(GetDlgItem(IDC_USETHUMBTEMPLATE), checked);
	::EnableWindow(GetDlgItem(IDC_USESERVERTHUMBNAILS), checked);
	::EnableWindow(GetDlgItem(IDC_TEXTOVERTHUMB2), checked);
		
	if(!checked)
		::EnableWindow(GetDlgItem(IDC_DRAWFRAME), checked);
	else 
		OnBnClickedUseThumbTemplate(0, 0, 0, bHandled);
		
	::EnableWindow(GetDlgItem(IDC_PXLABEL), checked);
		
	return 0;
}

LRESULT CUploadSettings::OnBnClickedLogooptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSettingsDlg dlg(2); // Open settings dialog and logo options tab
	dlg.DoModal(m_hWnd);
	return 0;
}

bool CUploadSettings::OnNext()
{
	if(m_nImageServer == -1 || m_nFileServer == -1)
	{
		MessageBox(TR("�� �� ������� ������!"), APPNAME, MB_ICONWARNING);
		return false;
	}

	UploadEngine &ue = EnginesList[m_nImageServer];
	if(ue.NeedAuthorization ==2 && !Settings.ServersSettings[ue.Name].authData.DoAuth)
	{
		CString errorMsg;
		errorMsg.Format(TR("�������� ������ �� ������ '%s' ���������� ��� ������� ������� ������.\n����������, ����������������� �� ������ ������� � ������� ���� ��������������� ������ � ���������."), (LPCTSTR)ue.Name);
		MessageBox(errorMsg, APPNAME, MB_ICONERROR);
		return false;
	}

	UploadEngine &ue2 = EnginesList[m_nFileServer];
	if(ue2.NeedAuthorization ==2 && !Settings.ServersSettings[ue2.Name].authData.DoAuth)
	{
		CString errorMsg;
		errorMsg.Format(TR("�� �� ������ ��������� ����������� �� ������� '%s'!"), (LPCTSTR)ue2.Name);
		MessageBox(errorMsg, APPNAME, MB_ICONWARNING);
		//return false;
	}

	Settings.ThumbSettings.TextOverThumb = SendDlgItemMessage(IDC_TEXTOVERTHUMB2, BM_GETCHECK);
	
	Settings.ImageSettings.KeepAsIs = SendDlgItemMessage(IDC_KEEPASIS, BM_GETCHECK, 0) == BST_CHECKED;
	Settings.ImageSettings.NewWidth= GetDlgItemInt(IDC_IMAGEWIDTH);
	Settings.ImageSettings.NewHeight = GetDlgItemInt(IDC_IMAGEHEIGHT);
	Settings.ImageSettings.AddLogo = IsChecked(IDC_YOURLOGO);
	Settings.ImageSettings.AddText = IsChecked(IDC_YOURTEXT);
	Settings.ThumbSettings.CreateThumbs = IsChecked(IDC_CREATETHUMBNAILS);
	Settings.ThumbSettings.UseServerThumbs = IsChecked(IDC_USESERVERTHUMBNAILS);
	Settings.ThumbSettings.UseThumbTemplate = IsChecked(IDC_USETHUMBTEMPLATE);
	Settings.ThumbSettings.DrawFrame = IsChecked(IDC_DRAWFRAME);
	Settings.ThumbSettings.ThumbAddImageSize = IsChecked(IDC_ADDFILESIZE);
	Settings.ImageSettings.SaveProportions = IsChecked(IDC_SAVEPROPORTIONS);
	Settings.ImageSettings.Quality = GetDlgItemInt(IDC_QUALITYEDIT);
	Settings.ImageSettings.Format = SendDlgItemMessage(IDC_FORMATLIST, CB_GETCURSEL);
	

	Settings.ServerID = m_nImageServer;
		//SendDlgItemMessage(IDC_SERVERLIST, CB_GETITEMDATA, temp, 0);


	//emp= SendDlgItemMessage(IDC_SERVERLIST2, CB_GETCURSEL, 0, 0);
	Settings.FileServerID = m_nFileServer;
	//SendDlgItemMessage(IDC_SERVERLIST2, CB_GETITEMDATA, temp, 0);
	
	Settings.ThumbSettings.ThumbWidth = GetDlgItemInt(IDC_THUMBWIDTH);
	
	return true;
}

bool CUploadSettings::OnShow()
{
	BOOL temp;
	

	OnBnClickedCreatethumbnails(0, 0, 0, temp);
	OnBnClickedKeepasis(0, 0, 0, temp);
	EnableNext();
	//MessageBox(Settings.ServersSettings[EnginesList[ServerID].Name].params["FolderTitle"]);
	//if(ServerID >=0)
	{
		
//		m_FolderLink.SetWindowText(Settings.ServersSettings[EnginesList[ServerID].Name].params["FolderTitle"]);
	
	}
		SetNextCaption(TR("&���������"));
	return true;
}

LRESULT CUploadSettings::OnBnClickedLogin(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	bool ImageServer = wID % 2;// (hWndCtl == Toolbar.m_hWnd);	
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;
	CLoginDlg dlg(nServerIndex);

	ServerSettingsStruct & ss = Settings.ServersSettings[ EnginesList[nServerIndex ].Name];
	CString UserName = ss.authData.Login; 

	if( dlg.DoModal(m_hWnd) == IDOK)
	{
		if(UserName != ss.authData.Login)
		{
			ss.params[_T("FolderID")]=_T("");
			ss.params[_T("FolderTitle")]=_T("");
			ss.params[_T("FolderUrl")]=_T("");
			iuPluginManager.UnloadPlugins();
		}
			
		UpdateAllPlaceSelectors();
	}
	return 0;
}

LRESULT CUploadSettings::OnBnClickedUseThumbTemplate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL checked = SendDlgItemMessage(IDC_USETHUMBTEMPLATE, BM_GETCHECK, 0, 0);

	if(checked && !FileExists(IU_GetDataFolder() + _T("thumb.png")) && wID == IDC_USETHUMBTEMPLATE)
	{
		MessageBox(TR("���������� ������������ ������ ��� ���������. ���� ������� \"Data\\Thumb.png\" �� ������."), APPNAME, MB_ICONWARNING);
		SendDlgItemMessage(IDC_USETHUMBTEMPLATE, BM_SETCHECK, false);
		return 0;
	}
	
	if(checked) SendDlgItemMessage(IDC_USESERVERTHUMBNAILS, BM_SETCHECK, false);

	checked = checked || SendDlgItemMessage(IDC_USESERVERTHUMBNAILS, BM_GETCHECK, 0, 0);
	::EnableWindow(GetDlgItem(IDC_DRAWFRAME), !checked);
	::EnableWindow(GetDlgItem(IDC_TEXTOVERTHUMB2),!checked);
	
	return 0;
}
	
LRESULT CUploadSettings::OnBnClickedUseServerThumbnails(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL checked=SendDlgItemMessage(IDC_USESERVERTHUMBNAILS, BM_GETCHECK, 0, 0);
	
	if(checked) SendDlgItemMessage(IDC_USETHUMBTEMPLATE, BM_SETCHECK, false);
	
	checked = checked || SendDlgItemMessage(IDC_USETHUMBTEMPLATE, BM_GETCHECK, 0, 0);
	::EnableWindow(GetDlgItem(IDC_DRAWFRAME),!checked);
	::EnableWindow(GetDlgItem(IDC_TEXTOVERTHUMB2),!checked);
		
	return 0;
}
	

LRESULT CUploadSettings::OnBnClickedSelectFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
bool ImageServer = (hWndCtl == Toolbar.m_hWnd);	
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;

	UploadEngine *ue = &EnginesList[nServerIndex];

	if(ue->SupportsFolders){
		CServerFolderSelect as(ue);
		as.m_SelectedFolder.id= Settings.ServersSettings[ue->Name].params["FolderID"];
		
		if(as.DoModal() == IDOK){
			if(!as.m_SelectedFolder.id.empty()){
				Settings.ServersSettings[ue->Name].params["FolderID"] = CString(as.m_SelectedFolder.id.c_str());
				Settings.ServersSettings[ue->Name].params["FolderTitle"]=CString(as.m_SelectedFolder.title.c_str());
				Settings.ServersSettings[ue->Name].params["FolderUrl"]= CString(as.m_SelectedFolder.viewUrl.c_str());
			}
			else {
				Settings.ServersSettings[ue->Name].params["FolderID"]="";
				Settings.ServersSettings[ue->Name].params["FolderTitle"]="";
				Settings.ServersSettings[ue->Name].params["FolderUrl"]="";

			}
		UpdateAllPlaceSelectors();
		}
	}
	return 0;
}
	
void CUploadSettings::UpdateToolbarIcons()
{
	HICON hImageIcon = NULL, hFileIcon = NULL;

	if(m_nImageServer != -1)
		hImageIcon = (HICON)LoadImage(0,IU_GetDataFolder()+_T("Favicons\\")+EnginesList[m_nImageServer].Name+_T(".ico"),IMAGE_ICON	,16,16,LR_LOADFROMFILE);
		
	if(m_nFileServer != -1)
		hFileIcon = (HICON)LoadImage(0,IU_GetDataFolder()+_T("Favicons\\")+EnginesList[m_nFileServer].Name+_T(".ico"),IMAGE_ICON	,16,16,LR_LOADFROMFILE);
	
	if(hImageIcon)
	{
		if(nImageIndex == -1)
		{
			nImageIndex = m_PlaceSelectorImageList.AddIcon( hImageIcon);
		}
		else nImageIndex= m_PlaceSelectorImageList.ReplaceIcon(nImageIndex, hImageIcon);
	} else nImageIndex =-1;

	if(hFileIcon)
	{
		if(nFileIndex == -1)
		{
			nFileIndex = m_PlaceSelectorImageList.AddIcon( hFileIcon);
		}
		else 
			nFileIndex = m_PlaceSelectorImageList.ReplaceIcon(nFileIndex, hFileIcon);
	} else nFileIndex = -1;

	Toolbar.ChangeBitmap(IDC_SERVERBUTTON, nImageIndex);
	FileServerSelectBar.ChangeBitmap(IDC_SERVERBUTTON, nFileIndex);
}

void CUploadSettings::UpdatePlaceSelector(bool ImageServer)
{
	TBBUTTONINFO bi;
	CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;

	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;

	CString serverTitle = (nServerIndex != -1) ? EnginesList[nServerIndex].Name: TR("�������� ������");

	ZeroMemory(&bi, sizeof(bi));
	bi.cbSize = sizeof(bi);
	bi.dwMask = TBIF_TEXT;
	bi.pszText = (LPWSTR)(LPCTSTR) serverTitle ;
	

	if(nServerIndex == -1)
	{
		CurrentToolbar.HideButton(IDC_LOGINTOOLBUTTON + ImageServer ,true);
		CurrentToolbar.HideButton(IDC_TOOLBARSEPARATOR1, true);
		CurrentToolbar.HideButton(IDC_SELECTFOLDER, true);
		CurrentToolbar.HideButton(IDC_TOOLBARSEPARATOR2, true);	
		return;
	}
	CurrentToolbar.SetButtonInfo(IDC_SERVERBUTTON, &bi);

	LoginInfo& li = Settings.ServersSettings[ EnginesList[nServerIndex ].Name].authData;
	CString login = TrimString(li.Login,23);
	
	CurrentToolbar.SetImageList(m_PlaceSelectorImageList);

	CurrentToolbar.HideButton(IDC_LOGINTOOLBUTTON + ImageServer,(bool)!EnginesList[nServerIndex].NeedAuthorization);
	CurrentToolbar.HideButton(IDC_TOOLBARSEPARATOR1,(bool)!EnginesList[nServerIndex].NeedAuthorization);
	
	bool ShowLoginButton = !login.IsEmpty() && li.DoAuth;
	if(!ShowLoginButton)
	{
		if(EnginesList[nServerIndex].NeedAuthorization == 2)
			login = TR("������ ������������...");
		else 
			login = TR("������� �� ������");

	}
	bi.pszText = (LPWSTR)(LPCTSTR)login;
	CurrentToolbar.SetButtonInfo(IDC_LOGINTOOLBUTTON+ImageServer, &bi);

	bool ShowFolderButton = EnginesList[nServerIndex].SupportsFolders && ShowLoginButton;

	CurrentToolbar.HideButton(IDC_SELECTFOLDER,!ShowFolderButton);
	CurrentToolbar.HideButton(IDC_TOOLBARSEPARATOR2,!ShowFolderButton);
		
	CString title = TrimString(Settings.ServersSettings[EnginesList[nServerIndex].Name].params["FolderTitle"], 27);
	if(title.IsEmpty()) title = TR("����� �� �������");
	bi.pszText = (LPWSTR)(LPCTSTR)title;
	CurrentToolbar.SetButtonInfo(IDC_SELECTFOLDER, &bi);
	
}
void CUploadSettings::UpdateAllPlaceSelectors()
{
	UpdatePlaceSelector(false);
	UpdatePlaceSelector(true);
	UpdateToolbarIcons();	
}


LRESULT CUploadSettings::OnImageServerSelect(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nServerIndex = wID - IDC_IMAGESERVER_FIRST_ID;
	m_nImageServer = nServerIndex;
	UpdateAllPlaceSelectors();
	return 0;
}

LRESULT CUploadSettings::OnFileServerSelect(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nServerIndex = wID - IDC_FILESERVER_FIRST_ID;
	
	m_nFileServer = nServerIndex;
	UpdateAllPlaceSelectors();
	return 0;
}
	

LRESULT CUploadSettings::OnServerDropDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMTOOLBAR* pnmtb = (NMTOOLBAR *) pnmh;

	bool ImageServer = (idCtrl == IDC_IMAGETOOLBAR);
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;


	CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;
	
	CMenu sub;	
	MENUITEMINFO mi;
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_TYPE|MIIM_ID;
	mi.fType = MFT_STRING;
	sub.CreatePopupMenu();
		
	if(pnmtb->iItem == IDC_SERVERBUTTON)
	{
		int menuItemCount=0;
		int FirstFileServerIndex = -1;
		
		if(ImageServer)
		{
			for(int i=0; i<EnginesList.size(); i++)
			{
				if(!EnginesList[i].ImageHost) continue;
				mi.wID = (ImageServer?IDC_IMAGESERVER_FIRST_ID: IDC_FILESERVER_FIRST_ID  ) +i;
				mi.dwTypeData  = (LPWSTR)(LPCTSTR) EnginesList[i].Name;//TR("��������� �����������");
				sub.InsertMenuItem(menuItemCount++, true, &mi);
			}
			
			mi.wID = IDC_FILESERVER_LAST_ID + 1;
			mi.fType = MFT_SEPARATOR;
			sub.InsertMenuItem(menuItemCount++, true, &mi);
		}

		mi.fType = MFT_STRING;
		for(int i=0; i<EnginesList.size(); i++)
		{
			if(EnginesList[i].ImageHost) continue;
			mi.wID = (ImageServer?IDC_IMAGESERVER_FIRST_ID: IDC_FILESERVER_FIRST_ID  ) +i;
			mi.dwTypeData  =(LPWSTR)(LPCTSTR) EnginesList[i].Name;//TR("��������� �����������");
			sub.InsertMenuItem(menuItemCount++, true, &mi);	
		}

		sub.SetMenuDefaultItem(ImageServer?(IDC_IMAGESERVER_FIRST_ID+m_nImageServer): (IDC_FILESERVER_FIRST_ID+m_nFileServer),FALSE);
	}
	else
	{
		if(EnginesList[nServerIndex].UsingPlugin )
		{
			CUploadScript *plug = iuPluginManager.getPlugin(EnginesList[nServerIndex].PluginName, Settings.ServersSettings[EnginesList[nServerIndex].Name]);
			if(!plug) return TBDDRET_TREATPRESSED;

			if(!plug->supportsSettings()) return TBDDRET_TREATPRESSED;
			mi.wID = IDC_LOGINTOOLBUTTON + (int)ImageServer;
 			mi.dwTypeData  = TR("��������� �����������");
			sub.InsertMenuItem(0, true, &mi);
   
			mi.wID = IDC_SERVERPARAMS + (int)ImageServer;
 			mi.dwTypeData  = TR("��������� �������");
			sub.InsertMenuItem(1, true, &mi);

			sub.SetMenuDefaultItem(0,TRUE);
		}
		else
		{
			return TBDDRET_TREATPRESSED;
		}
	}
		
	RECT rc;
   ::SendMessage(CurrentToolbar.m_hWnd,TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
   CurrentToolbar.ClientToScreen(&rc);
	TPMPARAMS excludeArea;
	ZeroMemory(&excludeArea, sizeof(excludeArea));
	excludeArea.cbSize = sizeof(excludeArea);
	excludeArea.rcExclude = rc;
	sub.TrackPopupMenuEx(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, m_hWnd, &excludeArea);
	bHandled = true;
	return TBDDRET_DEFAULT;
}

LRESULT CUploadSettings::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwnd = (HWND) wParam;
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam); 

	RECT rc;
	POINT pt = {xPos, yPos};

	if(hwnd == Toolbar.m_hWnd || hwnd == FileServerSelectBar.m_hWnd)
	{
		bool ImageServer = (hwnd == Toolbar.m_hWnd);
		CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;
	
		::SendMessage(CurrentToolbar.m_hWnd,TB_GETRECT, IDC_SELECTFOLDER, (LPARAM)&rc);
		CurrentToolbar.ClientToScreen(&rc);
		if(PtInRect(&rc, pt))
		{
			OnFolderButtonContextMenu(pt, ImageServer);
		}

		::SendMessage(CurrentToolbar.m_hWnd,TB_GETRECT, IDC_SERVERBUTTON, (LPARAM)&rc);
		CurrentToolbar.ClientToScreen(&rc);
		if(PtInRect(&rc, pt))
		{
			OnServerButtonContextMenu(pt, ImageServer);
		}
		
	}
	return 0;
}

void CUploadSettings::OnFolderButtonContextMenu(POINT pt, bool isImageServerToolbar)
{
		CMenu sub;	
		MENUITEMINFO mi;
		mi.cbSize = sizeof(mi);	
		mi.fMask = MIIM_TYPE | MIIM_ID;
		mi.fType = MFT_STRING;

		sub.CreatePopupMenu();
		RECT rc;

		mi.wID = IDC_NEWFOLDER + (int)isImageServerToolbar;
 		mi.dwTypeData  = TR("����� �����");
		sub.InsertMenuItem(0, true, &mi);

		mi.wID = IDC_OPENINBROWSER + (int)isImageServerToolbar;
		mi.dwTypeData  = TR("������� � ��������");
		sub.InsertMenuItem(1, true, &mi);
			
		sub.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, m_hWnd);
}

LRESULT CUploadSettings::OnNewFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool ImageServer = wID % 2;
	CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;

	
	UploadEngine &ue = EnginesList[nServerIndex];

	CUploadScript *m_pluginLoader = iuPluginManager.getPlugin(ue.PluginName, Settings.ServersSettings[ue.Name], true);
	if(!m_pluginLoader) return 0;

	CString title;
	std::vector<std::wstring> accessTypeList;

	m_pluginLoader->getAccessTypeList(accessTypeList);
	CFolderItem newFolder;
	if(Settings.ServersSettings[ue.Name].params[_T("FolderID")]	== IU_NEWFOLDERMARK)
		newFolder = Settings.ServersSettings[ue.Name].newFolder;

	 CNewFolderDlg dlg(newFolder, true, accessTypeList);
	 if(dlg.DoModal(m_hWnd) == IDOK)
	 {
		 Settings.ServersSettings[ue.Name].params[_T("FolderTitle")] = newFolder.title.c_str();
		Settings.ServersSettings[ue.Name].params[_T("FolderID")]	= IU_NEWFOLDERMARK;
		Settings.ServersSettings[ue.Name].params[_T("FolderUrl")]	= _T("");
		
		Settings.ServersSettings[ue.Name].newFolder = newFolder;
		UpdateAllPlaceSelectors();
	 }
	 return 0;
}
	
LRESULT CUploadSettings::OnOpenInBrowser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool ImageServer = wID % 2;
	CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;
	UploadEngine &ue = EnginesList[nServerIndex];


	CString str = Settings.ServersSettings[ue.Name].params[_T("FolderUrl")];
	if(!str.IsEmpty())
	{
		ShellExecute(0,_T("open"),str,_T(""),0,SW_SHOWNORMAL);
	}
	return 0;
}
	
void CUploadSettings::OnServerButtonContextMenu(POINT pt, bool isImageServerToolbar)
{
	CMenu sub;	
	MENUITEMINFO mi;
	mi.cbSize = sizeof(mi);	
	mi.fMask = MIIM_TYPE | MIIM_ID;
	mi.fType = MFT_STRING;
	sub.CreatePopupMenu();
	RECT rc;
	mi.wID = IDC_SERVERPARAMS + (int)isImageServerToolbar;
	mi.dwTypeData  = TR("��������� �������");
	sub.InsertMenuItem(0, true, &mi);
	mi.wID = IDC_OPENREGISTERURL + (int)isImageServerToolbar;
	mi.dwTypeData  = TR("������� �������� �����������");
	sub.InsertMenuItem(1, true, &mi);
	sub.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, m_hWnd);
}

LRESULT CUploadSettings::OnServerParamsClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool ImageServer = wID % 2;
	CToolBarCtrl& CurrentToolbar = (ImageServer) ? Toolbar: FileServerSelectBar;
	int nServerIndex = ImageServer? m_nImageServer: m_nFileServer;
	UploadEngine &ue = EnginesList[nServerIndex];
	if(!ue.UsingPlugin) return false;

	CServerParamsDlg dlg(ue);
	dlg.DoModal();
	return 0;
}
	