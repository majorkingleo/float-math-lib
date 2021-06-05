/*
  Interface für float Programm von L.E.O
*/
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
//#include "confunc.h"
#include "floatfunc.h"

#define IDC_EDIT1 102
#define IDC_EDIT2 104
#define IDC_EDIT3 105
#define IDCONVERT 1
#define IDC_DIV	108
#define IDC_FAC	113
#define IDC_LN		112
#define IDC_LOG	111
#define IDC_MINUS	106
#define IDC_MULT	109
#define IDC_PI		114
#define IDC_PLUS	107
#define IDC_QUAD	115
#define IDC_WURZ	110


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
HINSTANCE hInst;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
     {
     static char  szAppName[] = "Interface" ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASSEX   wndclass ;

     wndclass.cbSize        = sizeof (wndclass) ;
     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = DLGWINDOWEXTRA ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     wndclass.hIconSm       = LoadIcon (hInstance, szAppName) ;

     RegisterClassEx (&wndclass) ;

     hwnd = CreateDialog (hInstance, szAppName, 0, NULL) ;
     hInst= hInstance;

     ShowWindow (hwnd, iCmdShow) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
     {
     switch (iMsg)
          {
          case WM_CREATE :
               ZeroMemory(&buffer,sizeof(buffer));
               ZeroMemory(&outbuffer,sizeof(outbuffer));
               ZeroMemory(&errorbuffer,sizeof(buffer));
               ZeroMemory(&ergbuffer,sizeof(ergbuffer));
          		break;
//          case WM_PAINT :
//          		SetDlgItemText(hwnd,IDC_EDIT1,buffer);
//               SetDlgItemText(hwnd,IDC_EDIT3,outbuffer);
//               SetDlgItemText(hwnd,IDC_EDIT2,ergbuffer);
               break;
/*			 case WM_KEYDOWN :
               if ((wParam = toupper (wParam)) == VK_RETURN)
               {
                  iMsg=WM_COMMAND;
                  wParam = IDCONVERT;
               } else break;
*/          case WM_COMMAND :
               switch ( LOWORD(wParam) )
               {
               case	IDCONVERT :
                     GetDlgItemText(hwnd,IDC_EDIT1,buffer,sizeof(buffer));
                     if( convert()==-1 )
                     	MessageBox(hwnd,errorbuffer,"Fehlerhafte Eingabe",MB_OK | MB_ICONERROR);
                     else
                     	SetDlgItemText(hwnd,IDC_EDIT3,outbuffer);
               		break;
               case	IDC_WURZ :
               	   SetCursor (LoadCursor (NULL, IDC_WAIT));
               		if(floatfunc(WURZEL)==-1)
                     	MessageBox(hwnd,errorbuffer,"Ungültige Eingabe",MB_OK | MB_ICONERROR);
                     SetDlgItemText(hwnd,IDC_EDIT2,ergbuffer);
                     SetCursor (LoadCursor (NULL, IDC_ARROW));
                     MessageBeep(1);
                     break;
               }
               break;
          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
     }
