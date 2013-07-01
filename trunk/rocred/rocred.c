// -----------------------------------------------------------------
// RO Credentials (ROCred)
// (c) 2012-2013 Ai4rei/AN
//
// -----------------------------------------------------------------

#include <windows.h>
#include <commctrl.h>

#include <btypes.h>
#include <dlgabout.h>
#include <dlgtempl.h>
#include <md5.h>
#include <xf_binhex.h>

#include "rocred.h"

static const DLGTEMPLATEITEMINFO l_DlgItems[] =
{
    { DLGTEMPLATEITEM_CLASS_STATIC, 0,                                                  0, IDS_USERNAME,    7,      10, 60,     8,  },
    { DLGTEMPLATEITEM_CLASS_EDIT,   ES_AUTOHSCROLL|WS_BORDER|WS_TABSTOP,                0, IDC_USERNAME,    73,     7,  110,    14, },
    { DLGTEMPLATEITEM_CLASS_STATIC, 0,                                                  0, IDS_PASSWORD,    7,      28, 60,     8,  },
    { DLGTEMPLATEITEM_CLASS_EDIT,   ES_AUTOHSCROLL|ES_PASSWORD|WS_BORDER|WS_TABSTOP,    0, IDC_PASSWORD,    73,     25, 110,    14, },
    { DLGTEMPLATEITEM_CLASS_BUTTON, BS_AUTOCHECKBOX|WS_TABSTOP,                         0, IDC_CHECKSAVE,   73,     43, 110,    10, },
    { DLGTEMPLATEITEM_CLASS_BUTTON, BS_DEFPUSHBUTTON|WS_TABSTOP,                        0, IDOK,            79,     61, 50,     14, },
    { DLGTEMPLATEITEM_CLASS_BUTTON, BS_PUSHBUTTON|WS_TABSTOP,                           0, IDCANCEL,        133,    61, 50,     14, },
    { DLGTEMPLATEITEM_CLASS_BUTTON, BS_PUSHBUTTON|WS_TABSTOP,                           0, IDB_REPLAY,      7,      61, 50,     14, },
};
static const DLGTEMPLATEINFO l_DlgTempl =
{
    L"Tahoma",
    DS_MODALFRAME|DS_CENTER|DS_SETFONT|WS_POPUPWINDOW|WS_CAPTION,
    0,
    __ARRAYSIZE(l_DlgItems),
    9,
    0,
    0,
    190,
    82,
    l_DlgItems,
};
static char l_szIniFile[MAX_PATH];

static BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            char szBuffer[4096];
            BOOL bCheckSave;
            HINSTANCE hInstance = GetModuleHandleA(NULL);

            SendMessage(hWnd, WM_SETICON, ICON_BIG,
                (LPARAM)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_SHARED));
            SendMessage(hWnd, WM_SETICON, ICON_SMALL,
                (LPARAM)LoadImage(hInstance, MAKEINTRESOURCE(2), IMAGE_ICON, 16, 16, LR_SHARED));

            LoadStringA(hInstance, IDS_TITLE, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(hWnd, szBuffer);

            LoadStringA(hInstance, IDS_USERNAME, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDS_USERNAME), szBuffer);

            LoadStringA(hInstance, IDS_PASSWORD, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDS_PASSWORD), szBuffer);

            LoadStringA(hInstance, IDS_CHECKSAVE, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDC_CHECKSAVE), szBuffer);

            LoadStringA(hInstance, IDS_OK, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDOK), szBuffer);

            LoadStringA(hInstance, IDS_CANCEL, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDCANCEL), szBuffer);

            LoadStringA(hInstance, IDS_REPLAY, szBuffer, __ARRAYSIZE(szBuffer));
            SetWindowTextA(GetDlgItem(hWnd, IDB_REPLAY), szBuffer);

            bCheckSave = GetPrivateProfileIntA("ROCred", "CheckSave", FALSE, l_szIniFile);
            SendMessage(GetDlgItem(hWnd, IDC_CHECKSAVE), BM_SETCHECK, (WPARAM)bCheckSave, 0);

            if(bCheckSave)
            {
                GetPrivateProfileStringA("ROCred", "UserName", "", szBuffer, __ARRAYSIZE(szBuffer), l_szIniFile);

                if(szBuffer[0])
                {
                    SetWindowTextA(GetDlgItem(hWnd, IDC_USERNAME), szBuffer);
                    SetFocus(GetDlgItem(hWnd, IDC_PASSWORD));  // move focus to password
                    return FALSE;
                }
            }
        }
        break;
        case WM_COMMAND:
            if(HIWORD(wParam)!=1  && HIWORD(wParam)!=0)
            {
                ;
            }
            else switch(LOWORD(wParam))
            {
                case IDB_REPLAY:
                    {
                        char szExePath[MAX_PATH];
                        char szExeName[MAX_PATH];
                        char szExeType[16];
                        char szBuffer[4096];
                        HINSTANCE hInstance = GetModuleHandleA(NULL);
                        STARTUPINFO Si = { sizeof(Si) };
                        PROCESS_INFORMATION Pi;

                        GetPrivateProfileStringA("ROCred", "ExeName", "", szExeName, __ARRAYSIZE(szExeName), l_szIniFile);
                        GetPrivateProfileStringA("ROCred", "ExeType", "1rag1", szExeType, __ARRAYSIZE(szExeType), l_szIniFile);
                        {
                            char* lpszSlash = szExePath+GetModuleFileNameA(NULL, szExePath, __ARRAYSIZE(szExePath));

                            for(; lpszSlash[-1]!='\\'; lpszSlash--);
                            lpszSlash[0] = 0;
                            lstrcat(szExePath, szExeName);
                        }

                        // Replay mode
                        wsprintfA(szBuffer, "\"%s\" -t:Replay %s", szExePath, szExeType);

                        if(CreateProcessA(szExePath, szBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi))
                        {
                            CloseHandle(Pi.hThread);
                            CloseHandle(Pi.hProcess);
                        }
                        else
                        {
                            DWORD dwLastError = GetLastError();
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szBuffer, __ARRAYSIZE(szBuffer), NULL);
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                            LoadStringA(hInstance, IDS_EXE_ERROR, szBuffer, __ARRAYSIZE(szBuffer));
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                        }
                    }
                    EndDialog(hWnd, 1);
                    break;
                case IDOK:
                    {
                        char szUserName[24];
                        char szPassWord[24];
                        char szExePath[MAX_PATH];
                        char szExeName[MAX_PATH];
                        char szExeType[16];
                        char szBuffer[4096];
                        BOOL bCheckSave;
                        HINSTANCE hInstance = GetModuleHandleA(NULL);
                        STARTUPINFO Si = { sizeof(Si) };
                        PROCESS_INFORMATION Pi;

                        GetWindowTextA(GetDlgItem(hWnd, IDC_USERNAME), szUserName, __ARRAYSIZE(szUserName));

                        if(lstrlenA(szUserName)<4)
                        {
                            LoadStringA(hInstance, szUserName[0] ? IDS_USER_SHRT : IDS_USER_NONE, szBuffer, __ARRAYSIZE(szBuffer));
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONINFORMATION);
                            break;
                        }

                        GetWindowTextA(GetDlgItem(hWnd, IDC_PASSWORD), szPassWord, __ARRAYSIZE(szPassWord));

                        if(lstrlenA(szPassWord)<4)
                        {
                            LoadStringA(hInstance, szPassWord[0] ? IDS_PASS_SHRT : IDS_PASS_NONE, szBuffer, __ARRAYSIZE(szBuffer));
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONINFORMATION);
                            break;
                        }

                        bCheckSave = (BOOL)(SendMessage(GetDlgItem(hWnd, 103), BM_GETCHECK, 0, 0)==BST_CHECKED);
                        WritePrivateProfileStringA("ROCred", "CheckSave", bCheckSave ? "1" : "0", l_szIniFile);

                        if(bCheckSave)
                        {// save
                            WritePrivateProfileStringA("ROCred", "UserName", szUserName, l_szIniFile);
                        }

                        GetPrivateProfileStringA("ROCred", "ExeName", "", szExeName, __ARRAYSIZE(szExeName), l_szIniFile);
                        GetPrivateProfileStringA("ROCred", "ExeType", "1rag1", szExeType, __ARRAYSIZE(szExeType), l_szIniFile);
                        {
                            char* lpszSlash = szExePath+GetModuleFileNameA(NULL, szExePath, __ARRAYSIZE(szExePath));

                            for(; lpszSlash[-1]!='\\'; lpszSlash--);
                            lpszSlash[0] = 0;
                            lstrcat(szExePath, szExeName);
                        }

                        if(GetPrivateProfileIntA("ROCred", "HashMD5", FALSE, l_szIniFile))
                        {// MD5
                            uint8 ucHash[4*4];
                            char szHexHash[4*4*2+1];

                            MD5_String(szPassWord, ucHash);
                            XF_BinHex(szHexHash, __ARRAYSIZE(szHexHash), ucHash, __ARRAYSIZE(ucHash));

                            wsprintfA(szBuffer, "\"%s\" -t:%s %s %s", szExePath, szHexHash, szUserName, szExeType);
                        }
                        else
                        {// Plaintext
                            wsprintfA(szBuffer, "\"%s\" -t:%s %s %s", szExePath, szPassWord, szUserName, szExeType);
                        }

                        if(CreateProcessA(szExePath, szBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi))
                        {
                            CloseHandle(Pi.hThread);
                            CloseHandle(Pi.hProcess);
                        }
                        else
                        {
                            DWORD dwLastError = GetLastError();
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szBuffer, __ARRAYSIZE(szBuffer), NULL);
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                            LoadStringA(hInstance, IDS_EXE_ERROR, szBuffer, __ARRAYSIZE(szBuffer));
                            MessageBox(hWnd, szBuffer, "", MB_OK|MB_ICONSTOP);
                        }
                    }
                    EndDialog(hWnd, 1);
                    break;
                case IDCANCEL:
                    EndDialog(hWnd, 0);
                    break;
            }
            break;
        case WM_HELP:
        {
            DLGABOUTINFO Dai =
            {
                hWnd,
                MAKELONG(2012,2013),
                "About ROCred...",
                "ROCred",
                APP_VERSION,
                "Ai4rei/AN",
                "\r\nThis software is FREEWARE and is provided AS IS, without warranty of ANY KIND, either expressed or implied, including but not limited to the implied warranties of merchantability and/or fitness for a particular purpose. If your country's law does not allow complete exclusion of liability, you may not use this software. The author SHALL NOT be held liable for ANY damage to you, your hardware, your software, your pets, your dear other, or to anyone or anything else, that may or may not result from the use or misuse of this software. Basically, you use it at YOUR OWN RISK.",
            };

            DlgAbout(&Dai);
        }
        break;
        default:
            return FALSE;
    }

    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nShowCmd)
{
    unsigned char ucDlgBuf[264];
    unsigned long luLen, luDlgBufSize = sizeof(ucDlgBuf);

    GetModuleFileNameA(NULL, l_szIniFile, __ARRAYSIZE(l_szIniFile));
    luLen = lstrlenA(l_szIniFile);
    lstrcpyA((luLen>4 && l_szIniFile[luLen-4]=='.') ? &l_szIniFile[luLen-4] : &l_szIniFile[luLen], ".ini");

    AssertHere(DlgTemplate(&l_DlgTempl, ucDlgBuf, &luDlgBufSize));
    InitCommonControls();
    DialogBoxIndirectParam(GetModuleHandle(NULL), (LPCDLGTEMPLATE)ucDlgBuf, NULL, &DlgProc, 0);

    return 0;
}