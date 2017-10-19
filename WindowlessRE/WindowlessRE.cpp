/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/



/****************************************************************************

        PROGRAM: txtsrvcs.exe

        PURPOSE: demonstrates basic windowless richedit control implementation


****************************************************************************/

#include <windows.h>   
#include "resource.h"  
#include "tstxtsrv.h"   
#include "windowsx.h"
#include "richedit.h"
#include "memory.h"
#include "tchar.h"
#include "host.h"
#include "tom.h"


/********************************************************/

DEFINE_GUID(IID_ITextDocument,	0x8CC497C0,	0xA1DF,0x11CE,0x80,0x98,0x00,0xAA,
											0x00,0x47,0xBE,0x5D);

/**********************************************************/

const int dxCaret = 1;

struct SEditControlInfo
{
	RECT				rcLoc;			// Rectangle that control covers
	BOOL				fFocus;		// Whether control has the input focus
	CTxtWinHost *		ptwh;			// Host object 
};


//forward declaration so others can use this
LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message,	WPARAM uParam,	LPARAM lParam);



/***************************************************/
TCHAR *pszInitControl1Text = TEXT("Control 0") ;
TCHAR szAppName[] = TEXT("TestTextServices") ;
TCHAR szTitle[]   = TEXT("Test Text Services Application") ;
TCHAR szChildWindowClass[] = TEXT("ControlWindow") ;



/********************************************************/
HINSTANCE hInst;          
HWND hwndChild;
HANDLE hAccelTable = NULL ;
SEditControlInfo aseciChildControl ;// structure used  to redirect input to the control



int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   MSG msg;

   if (!InitApplication(hInstance)) 
   { 
      return (FALSE);     // Exits if unable to initialize
   }

   if(!InitInstance(hInstance, nCmdShow)) 
   {
      return (FALSE);
   }
   
   hAccelTable = LoadAccelerators (hInstance, NULL);


   while (GetMessage(&msg, NULL, 0, 0))
   {
      if (!TranslateAccelerator (msg.hwnd, (HACCEL) hAccelTable, &msg)) 
      {
         TranslateMessage(&msg);// Translates virtual key codes
         DispatchMessage(&msg); // Dispatches message to window
      }
   }
   return (msg.wParam); // Returns the value from PostQuitMessage
}



BOOL InitApplication(HINSTANCE hInstance)
{
        WNDCLASS  wc;
		ATOM ret = 0 ;
        // Fill in window class structure with parameters that describe the
        // main window.

        wc.style         = CS_HREDRAW | CS_VREDRAW;// Class style(s).
        wc.lpfnWndProc   = (WNDPROC)WndProc;       // Window Procedure
        wc.cbClsExtra    = 0;                      // No per-class extra data.
        wc.cbWndExtra    = 4;                      // Text Services Host needs 4 bytes
        wc.hInstance     = hInstance;              // Owner of this class
        wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP)); // Icon name from .RC
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);// Default color
		wc.lpszMenuName = NULL;	// MAKEINTRESOURCE(IDR_GENERIC); // Menu from .RC
        wc.lpszClassName = szAppName;              // Name to register as

        // Register the window class and return success/failure code.
		ret = RegisterClass(&wc) ;//whyis this failing on Millenium????? 8/14/00
		return (ret);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd; 

   hInst = hInstance;

   hWnd = CreateWindow(
                szAppName,           // See RegisterClass() call.
                szTitle,             // Text for window title bar.
                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,// Window style.
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, // Use default positioning
                NULL,                // Overlapped windows have no parent.
                NULL,                // Use the window class menu.
                hInstance,           // This instance owns this window.
                NULL);                 // We don't use any data in our WM_CREATE
        

   if (!hWnd) 
   {
      return (FALSE);
   }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return (TRUE);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)
{
   int wmId, wmEvent;
	HMENU hMenu;
   RECT rc ;
   switch (message) 
   {
	   case WM_SETFOCUS:
		   SetFocus(hwndChild);
		   break;
     // case WM_COMMAND: 
     //  	wmId = LOWORD(uParam);
     //    wmEvent = HIWORD(uParam);
   		//hMenu = GetMenu(hWnd);
     //    CheckMenuItem (hMenu, wmId, MF_CHECKED) ;
     //    switch (wmId) 
     //    {
     //       case IDM_EXIT:
     //          DestroyWindow (hWnd);
     //          break;

     // 		case IDM_PRINT:
     // 			SendMessage(hwndChild, WM_PRINT_CONTROL, 0, 0);
     // 		   break;

     //       default:
     //          return (DefWindowProc(hWnd, message, uParam, lParam));
     //      }
     //      break;
      case WM_DESTROY:  
    	   PostQuitMessage(0);
         break;

	   case WM_CREATE:
		   hwndChild = CreateControlWindow(hWnd, hInst);
			if (NULL == hwndChild)
			{
				return -1;
			}
			SetFocus(hwndChild);
		   break;
      case WM_SIZE:
         if(hwndChild)
         {
            GetClientRect(hWnd, &rc) ;
            MoveWindow(hwndChild, 0, 0, rc.right-rc.left, rc.bottom-rc.left, TRUE) ;
         }
      default:          
		   return DefWindowProc(hWnd, message, uParam, lParam);
   }
	return 0 ;
}



/****************************************************************************************
 this is a helper to create a control

*****************************************************************************************/

HRESULT CreateTextControl(
	HWND hwndControl,
	HWND hwndParent,
	DWORD dwStyle,
	TCHAR *pszText,
	LONG lLeft,
	LONG lTop,
	LONG lWidth,
	LONG lHeight,
	CTxtWinHost **pptec)
{
	CREATESTRUCT cs;

	// No text for text control yet
	cs.lpszName = pszText;

	// No parent for this window
	cs.hwndParent = hwndParent;

	// Style of control that we want
	cs.style = dwStyle;
	cs.dwExStyle = 0;

	// Rectangle for the control
	cs.x = lLeft;
	cs.y = lTop;
	cs.cy = lHeight;
	cs.cx = lWidth;

	// Create our handler for the text control
	return CreateHost(hwndControl, &cs, NULL, pptec);
}



/****************************************************************************************************
 This is the window proc for the control window. It's used to direct input to the windowless control 
*****************************************************************************************************/
LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message,	WPARAM uParam,	LPARAM lParam)
{
	// Get the text control if there is one
	LONG x = LOWORD (lParam);
	LONG y = HIWORD (lParam);
	SEditControlInfo *paseci = (SEditControlInfo *) GetWindowLong(hWnd, 0);

	// Used to save indirections
	SEditControlInfo *pseci = NULL;

	if (paseci)
	{
		// If this is a mouse click in someones window then record who
		// has the focus.

		switch(message)
		{
		   case WM_SETCURSOR:
			{
				POINT pt;
				GetCursorPos(&pt);
				::ScreenToClient(hWnd, &pt);
				// See if this is over a control
            if(!paseci->ptwh->DoSetCursor(NULL, &pt) )
            {
               SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
				break;
			}

		   case WM_DESTROY:
			{
				// Release the controls
            paseci->ptwh->Release();

				// Clear out our data
				SetWindowLong(hWnd, 0, 0);
   			break;
			}
/*
         case WM_KEYDOWN:
         {
            if(wParam == VK_UP ||wParam == VK_DOWN)
            {
               paseci->ptwh->OnKeyDown() ;
            }
            goto DEF_PROCESSING ;
         }
*/
		   case WM_LBUTTONDBLCLK:
			   pseci = paseci;
			   pseci->ptwh->TxWindowProc(hWnd, message, uParam, lParam);
			   break;
		   case WM_PAINT:
			{
				PAINTSTRUCT ps;

				HDC hdc;
				RECT rc;

				hdc = BeginPaint(hWnd, &ps);

				// Force clear out of window area being repainted
				GetClientRect(hWnd, &rc);
				ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

				// Because we are not really passing the message through Window's DefWinProc
				// we can doctor the the parameters.
				uParam = (WPARAM) hdc;
				lParam = (LPARAM) &ps.rcPaint;

				if ((0 == ps.rcPaint.bottom) && (0 == ps.rcPaint.right))
				{
					lParam = 0;
				}

				// First give the message to whoever has the focus.
            pseci = paseci ;
				pseci->ptwh->TxWindowProc(hWnd, message, uParam, lParam);
				EndPaint(hWnd, &ps);
				return 0;
            //break ;
			}

		case WM_TIMER:
			lParam = 0;
         break ;
		default:
//DEF_PROCESSING:
			// First give the message to whoever has the focus.
			pseci = paseci ;

			if (pseci->fFocus)
			{
				LRESULT lres = pseci->ptwh->TxWindowProc(hWnd, message, uParam, lParam);
				if (lres != S_MSG_KEY_IGNORED)
				{
					return lres;
				}
			}
			break;
      }
	}

	// Otherwise let DefWindowProc handle it.
	return DefWindowProc(hWnd, message, uParam, lParam);
}


/**********************************************************************************************
 This creates the control window and as a side effect fills the window
 with the windowless control. The control window is a child window. It is the parent's
 responsiblity to pass the input focus to the child window so that the
 correct things will happen with respect to input.
**********************************************************************************************/

HWND CreateControlWindow(HWND hwndParent, HINSTANCE hInst)
{
	// Register the class for the child window
	WNDCLASS wc;

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = ControlWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(void *);
	wc.hInstance = hInst;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szChildWindowClass;

	if(RegisterClass(&wc) != NULL)
	{
		// Get the client rectangle of the parent. We want the child window
		// to cover the entire parenets area.
		RECT rc;

		GetClientRect(hwndParent, &rc);

		// Total height and width of window
		LONG lHeight = rc.bottom - rc.top;
		LONG lWidth = rc.right - rc.left;

		// Create the window - note the window is set to the inital size
		// of the client area of the parent and is not resized.
        HWND hwnd = CreateWindow(
        	szChildWindowClass, 
        	TEXT(""),
            WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP,
            rc.left, 
            rc.top,
            lWidth,     
            lHeight,
            hwndParent,
			NULL,
            hInst, 
            NULL);
        GetClientRect(hwnd, &rc) ;

		if (hwnd != NULL)
		{
			// We have a control window. Now put a control in the 
			// window.

			// Set up the control's rectangle
			aseciChildControl.rcLoc.top = rc.top;
			aseciChildControl.rcLoc.bottom = rc.bottom ;
			aseciChildControl.rcLoc.left = rc.left;
			aseciChildControl.rcLoc.right = rc.right ;

			// Create the host for the window.
			HRESULT hr = CreateTextControl(
				hwnd,
				hwndParent,
				ES_MULTILINE, 
				pszInitControl1Text,
				rc.left,
				rc.top,
				rc.right-rc.top,
				rc.bottom-rc.top,
				&aseciChildControl.ptwh);


			if (FAILED(hr))
			{
            OutputDebugString(TEXT("failed to create windowless text control")) ;
				return NULL;
			}
			// Store the pointer to the control structure for the controls.
			SetWindowLong(hwnd, 0, (LONG) &aseciChildControl);

         //set focus to our windowless control
         aseciChildControl.ptwh->OnTxInPlaceActivate(NULL);

         aseciChildControl.fFocus = TRUE;
	      ((aseciChildControl.ptwh)->GetTextServices())->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
	   }
		return hwnd;
	}
	return NULL;
}

