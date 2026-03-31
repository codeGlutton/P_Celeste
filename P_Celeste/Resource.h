#include <tchar.h>

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++에서 생성한 포함 파일입니다.
// SizeInput.rc에서 사용되고 있습니다.
//
#define IDI_ICON1						101
#define IDD_TILE_SIZE_DIALOG			102

#define IDC_WIDTH_EDIT					1001
#define IDC_HEIGHT_EDIT					1002

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        103
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1003
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif

struct FSizeDiaglogData
{
    std::wstring mTitle;
    int mSizeX;
    int mSizeY;
};

inline INT_PTR CALLBACK TileSizeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static FSizeDiaglogData* pData;

    switch (message)
    {
    case WM_INITDIALOG:
        pData = (FSizeDiaglogData*)lParam;
        if (pData != nullptr)
        {
            // 제목 변경
            SetWindowText(hDlg, pData->mTitle.c_str());
        }
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            TCHAR buf[32];

            GetDlgItemText(hDlg, IDC_WIDTH_EDIT, buf, 32);
            pData->mSizeX = _ttoi(buf);

            GetDlgItemText(hDlg, IDC_HEIGHT_EDIT, buf, 32);
            pData->mSizeY = _ttoi(buf);

            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}