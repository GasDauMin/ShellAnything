/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "InputBox.h"

#define DEFAULT_INPUTBOX_WIDTH  400
#define DEFAULT_INPUTBOX_HEIGHT 150
#define DEFAULT_BUTTON_WIDTH  90
#define DEFAULT_BUTTON_HEIGHT 25
#define DEFAULT_BUTTON_SPACING 5
#define DEFAULT_TEXTBOX_HEIGHT 20
#define DEFAULT_HORIZONTAL_PADDING 10
#define DEFAULT_VERTICAL_PADDING   10

static const char * INPUTBOX_CLASS_NAME = "InputBox";

HFONT createInputBoxFont()
{
  //https://stackoverflow.com/questions/221411/how-can-i-specify-a-font-for-a-window-created-through-createwindow

  //Manual font creation
  //HFONT hFont = CreateFont (13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
  //    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
  //    DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

  //Manual font creation from LOGFONT struct
  //LOGFONT lfont = {0};
  //lstrcpy(lfont.lfFaceName, "Arial");
  //lfont.lfHeight = 16;
  //lfont.lfWeight = FW_NORMAL;
  //lfont.lfItalic = FALSE;
  //lfont.lfCharSet = DEFAULT_CHARSET;
  //lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  //lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  //lfont.lfQuality = DEFAULT_QUALITY;
  //lfont.lfPitchAndFamily = DEFAULT_PITCH;
  //HFONT hFont = CreateFontIndirect(&lfont);

  //List of font to explorer for the prompt and/or the textbox:
  //  Georgia
  //  Microsoft Sans Serif
  //  Segoe UI
  //  Tahoma

  //Get default system font
  HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

  return hFont;
}

CInputBox::CInputBox(HWND hParent) :
  m_hInstance(NULL),
  m_hWindowFont(NULL),
  m_hIcon(NULL),
  m_hParent(NULL),
  m_hInputBox(NULL),
  m_hTextBoxAnswer(NULL),
  m_hButtonOK(NULL),
  m_hButtonCancel(NULL),
  m_hLabelPrompt(NULL),
  m_Width(DEFAULT_INPUTBOX_WIDTH),
  m_Height(DEFAULT_INPUTBOX_HEIGHT)
{
  m_hInstance = GetModuleHandle(NULL);
  m_hParent = hParent;

  WNDCLASSEX wcex = {0};
  wcex.cbSize = sizeof(WNDCLASSEX); //Be sure to set this member before calling the GetClassInfoEx function: https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagwndclassexa

  if (!GetClassInfoEx(m_hInstance, INPUTBOX_CLASS_NAME, &wcex))
  {
    //Load system question icon
    //Icon id: https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/ne-shellapi-shstockiconid
    SHSTOCKICONINFO sii;
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_INFO, SHGSI_ICON|SHGFI_SMALLICON, &sii)))
    {
      m_hIcon = sii.hIcon;
    }

    //http://www.cplusplus.com/forum/windows/103098/
    //https://www.codementor.io/malortie/build-win32-api-application-window-c-cpp-visual-studio-du107nrh6
    wcex.style          = 0;//CS_HREDRAW | CS_VREDRAW; // Redraw the window if the size changes
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.hInstance      = m_hInstance;
    wcex.hIcon          = m_hIcon; //LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1); //+1 is required according to https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagwndclassexa
    wcex.lpszClassName  = INPUTBOX_CLASS_NAME;
    wcex.hIconSm        = m_hIcon; //LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0)
      MessageBox(hParent, "Failed registering WNDCLASSEX for CInputBox!", "Error", MB_OK | MB_ICONERROR);
  }
}

CInputBox::~CInputBox()
{
  //Cleanup icon
  if (m_hIcon)
  {
    DestroyIcon(m_hIcon);
    m_hIcon = NULL;
  }

  // Unregister window class, freeing the memory that was previously allocated for this window.
  UnregisterClass(INPUTBOX_CLASS_NAME, m_hInstance);
}

void CInputBox::setWidth(const int & width)
{
  m_Width = width;
}

const int & CInputBox::getWidth() const
{
  return m_Width;
}

void CInputBox::setHeight(const int & height)
{
  m_Height = height;
}

const int & CInputBox::getHeight() const
{
  return m_Height;
}

void CInputBox::setWindowFont(HFONT hFont)
{
  m_hWindowFont = hFont;
}

HFONT CInputBox::getWindowFont() const
{
  return m_hWindowFont;
}

void CInputBox::setPromptFont(HFONT hFont)
{
  m_hPromptFont = hFont;
}

HFONT CInputBox::getPromptFont() const
{
  return m_hPromptFont;
}

void CInputBox::setIcon(HICON hIcon)
{
  m_hIcon = hIcon;
}

HICON CInputBox::getIcon() const
{
  return m_hIcon;
}

void CInputBox::setParentWindow(HWND hWnd)
{
  m_hParent = hWnd;
}

HWND CInputBox::getParentWindow() const
{
  return m_hParent;
}

//void CInputBox::setWindow(HWND hWnd)
//{
//  m_hInputBox = hWnd;
//}

HWND CInputBox::getWindow() const
{
  return m_hInputBox;
}

HINSTANCE CInputBox::getModuleHandle() const
{
  return m_hInstance;
}

void CInputBox::setCtrl(CONTROLS ctrl, HWND hWnd)
{
  switch(ctrl)
  {
    case TEXTBOX_ANSWER:
      m_hTextBoxAnswer = hWnd;
      break;
    case BUTTON_OK:
      m_hButtonOK = hWnd;
      break;
    case BUTTOK_CANCEL:
      m_hButtonCancel = hWnd;
      break;
    case LABEL_PROMPT:
      m_hLabelPrompt = hWnd;
      break;
  default:
    static const int BUFFER_SIZE = 1024;
    char message[BUFFER_SIZE];
    sprintf_s(message, BUFFER_SIZE, "Failed setting window control '%d' in function '%s'.", (int)ctrl, __FUNCTION__);
    MessageBox(m_hParent, message, "Error", MB_OK | MB_ICONERROR);
    break;
  };
}

HWND CInputBox::getCtrl(CONTROLS ctrl) const
{
  switch(ctrl)
  {
    case TEXTBOX_ANSWER:
      return m_hTextBoxAnswer;
      break;
    case BUTTON_OK:
      return m_hButtonOK;
      break;
    case BUTTOK_CANCEL:
      return m_hButtonCancel;
      break;
    case LABEL_PROMPT:
      return m_hLabelPrompt;
      break;
  default:
    static const int BUFFER_SIZE = 1024;
    char message[BUFFER_SIZE];
    sprintf_s(message, BUFFER_SIZE, "Failed getting window control '%d' in function '%s'.", (int)ctrl, __FUNCTION__);
    MessageBox(m_hParent, message, "Error", MB_OK | MB_ICONERROR);
    return NULL;
    break;
  };
}

void CInputBox::setText(const std::string & text)
{
  m_Text = text;
}

const std::string & CInputBox::getText() const
{
  return m_Text;
}

inline CInputBox* GetInputBoxInstance(HWND hWnd)
{
  //https://docs.microsoft.com/en-us/windows/desktop/learnwin32/managing-application-state-
  LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
  CInputBox *pInputBox = reinterpret_cast<CInputBox*>(ptr);
  return pInputBox;
}

LRESULT CALLBACK CInputBox::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  //Get the inputbox saved when WM_CREATE message was processed
  CInputBox *pInputBox = GetInputBoxInstance(hWnd);

  switch (uMsg)
  {
  case WM_CREATE:
    {
      //Get the associated CInputBox instance and save the pointer for later use
      //https://docs.microsoft.com/en-us/windows/desktop/learnwin32/managing-application-state-
      CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
      pInputBox = reinterpret_cast<CInputBox*>(pCreate->lpCreateParams);
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pInputBox);

      //create window font (default system font)
      HFONT hWindowFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
      pInputBox->setWindowFont(hWindowFont);

      //create font for prompt label
      //https://stackoverflow.com/questions/221411/how-can-i-specify-a-font-for-a-window-created-through-createwindow
      LOGFONT lfont = {0};
      lstrcpy(lfont.lfFaceName, "Georgia"); //Georgia, Microsoft Sans Serif, Segoe UI, Tahoma
      lfont.lfHeight = 18;
      lfont.lfWeight = FW_NORMAL;
      lfont.lfItalic = FALSE;
      lfont.lfCharSet = DEFAULT_CHARSET;
      lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
      lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      lfont.lfQuality = CLEARTYPE_QUALITY; //ANTIALIASED_QUALITY; //DEFAULT_QUALITY;
      lfont.lfPitchAndFamily = DEFAULT_PITCH;
      HFONT hPromptFont = CreateFontIndirect(&lfont);
      pInputBox->setPromptFont(hPromptFont);

      //get the module instance
      HINSTANCE hInstance = pInputBox->getModuleHandle();

      //get the window dimensions
      RECT window_rect = {0};
      RECT client_rect = {0};
      GetWindowRect(hWnd, &window_rect);
      GetClientRect(hWnd, &client_rect);
      LONG & client_width  = client_rect.right;
      LONG & client_height = client_rect.bottom;

      // textbox Answer
      HWND hTextBoxAnswer = CreateWindowEx(/*WS_EX_STATICEDGE*/ WS_EX_CLIENTEDGE,
        "EDIT","",
        WS_VISIBLE | WS_CHILD /*| WS_TABSTOP | ES_AUTOHSCROLL*/,
        DEFAULT_HORIZONTAL_PADDING, client_height - DEFAULT_VERTICAL_PADDING - DEFAULT_TEXTBOX_HEIGHT, client_width - 2*DEFAULT_HORIZONTAL_PADDING, DEFAULT_TEXTBOX_HEIGHT,
        hWnd,
        NULL,
        hInstance,
        NULL);
      pInputBox->setCtrl(CInputBox::TEXTBOX_ANSWER, hTextBoxAnswer);
      SendMessage(hTextBoxAnswer, WM_SETFONT, (WPARAM)hWindowFont, 0);

      // button OK
      HWND hButtonOK = CreateWindowEx(WS_EX_STATICEDGE,
        "BUTTON","OK",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        client_width - DEFAULT_HORIZONTAL_PADDING - DEFAULT_BUTTON_WIDTH, DEFAULT_VERTICAL_PADDING, DEFAULT_BUTTON_WIDTH, DEFAULT_BUTTON_HEIGHT,
        hWnd,
        NULL,
        hInstance,
        NULL);
      pInputBox->setCtrl(CInputBox::BUTTON_OK, hButtonOK);
      SendMessage(hButtonOK, WM_SETFONT, (WPARAM)hWindowFont, 0);

      // button Cancel
      HWND hButtonCancel = CreateWindowEx(WS_EX_STATICEDGE,
        "BUTTON","Cancel",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        client_width - DEFAULT_HORIZONTAL_PADDING - DEFAULT_BUTTON_WIDTH, DEFAULT_VERTICAL_PADDING + DEFAULT_BUTTON_HEIGHT + DEFAULT_BUTTON_SPACING, DEFAULT_BUTTON_WIDTH, DEFAULT_BUTTON_HEIGHT,
        hWnd,
        NULL,
        hInstance,
        NULL);
      pInputBox->setCtrl(CInputBox::BUTTOK_CANCEL, hButtonCancel);
      SendMessage(hButtonCancel, WM_SETFONT, (WPARAM)hWindowFont, 0);

      // label Prompt
      HWND hLabelPrompt = CreateWindowEx(SS_ETCHEDFRAME /*SS_LEFT*/,
        "STATIC","",
        WS_VISIBLE | WS_CHILD,
        DEFAULT_HORIZONTAL_PADDING, DEFAULT_VERTICAL_PADDING, client_width - 2*DEFAULT_HORIZONTAL_PADDING - DEFAULT_HORIZONTAL_PADDING - DEFAULT_BUTTON_WIDTH, client_height - 2*DEFAULT_VERTICAL_PADDING - DEFAULT_VERTICAL_PADDING - DEFAULT_TEXTBOX_HEIGHT,
        hWnd,
        NULL,
        hInstance,
        NULL);
      pInputBox->setCtrl(CInputBox::LABEL_PROMPT, hLabelPrompt);
      SendMessage(hLabelPrompt, WM_SETFONT, (WPARAM)hPromptFont, 0);

      //set default focus on textbox
      SetFocus(hTextBoxAnswer);
    }
    break;
  case WM_DESTROY:
    {
      //window font
      HFONT hFont = pInputBox->getWindowFont();
      DeleteObject(hFont);
      pInputBox->setWindowFont(NULL);

      //prompt font
      hFont = pInputBox->getPromptFont();
      DeleteObject(hFont);
      pInputBox->setPromptFont(NULL);

      //set parent back in focus
      HWND hParent = pInputBox->getParentWindow();
      EnableWindow(hParent, TRUE);
      SetForegroundWindow(hParent);

      DestroyWindow(hWnd);
      PostQuitMessage(0);
    }
    break;
  case WM_COMMAND:
    switch (HIWORD(wParam))
    {
    case BN_CLICKED:
      HWND hInputBox      = pInputBox->getWindow();
      HWND hButtonOK      = pInputBox->getCtrl(BUTTON_OK);
      HWND hButtonCancel  = pInputBox->getCtrl(BUTTOK_CANCEL);

      HWND hButtonClicked = (HWND)lParam;
      if (hButtonClicked == hButtonOK)
      {
        //simulate user pressing ENTER key which is catched in DoModal() and ease the destruction of the window
        PostMessage(hInputBox, WM_KEYDOWN, VK_RETURN, 0);
      }
      else if (hButtonClicked == hButtonCancel)
      {
        //simulate user pressing ESC key which is catched in DoModal() and ease the destruction of the window
        PostMessage(hInputBox, WM_KEYDOWN, VK_ESCAPE, 0);
      }
      break;
    }
    break;
  case WM_CTLCOLORSTATIC:
    {
      HDC hdc = (HDC) wParam;
      HWND hwndCtl = (HWND) lParam;

      //Different type of brushes for debugging
      //HBRUSH brush1 = CreateSolidBrush(RGB(255, 0, 0));
      //HBRUSH brush3 = GetSysColorBrush(COLOR_WINDOW);
      //HBRUSH brush4 = GetSysColorBrush(COLOR_WINDOW + 1);
      //HBRUSH brush5 = (HBRUSH)GetStockObject(NULL_BRUSH);
      //HBRUSH brush6 = GetSysColorBrush(CTLCOLOR_DLG);

      //Define a foreground color if required
      //Note: This only affects the area where text is paint.
      //SetTextColor(hdc, RGB(0,0,0)); //don't need to change the color of the text

      //Define the background color, if required.
      //Note: This only affects the area where text is paint.
      //Define background mode as transparent to match the background color of its parent window
      //(in this case, the color used in RegisterClassEx() from WNDCLASSEX::hbrBackground)
      SetBkMode(hdc, TRANSPARENT);

      //Use a NULL brush to force the label to use the same color as its parent window.
      //Note: This only affects the area of the label where text IS NOT painted.
      HBRUSH null_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
      return (LRESULT)null_brush;
    }
    break;
  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}

bool CInputBox::DoModal(const std::string & caption, const std::string & prompt)
{
  //Get the size of the desktop to be able to center the windows in the screen
  RECT rect;
  GetWindowRect(GetDesktopWindow(), &rect);

  // Setup window initialization attributes.
  CREATESTRUCT cs = {0};
  cs.lpCreateParams = NULL;
  cs.hInstance      = m_hInstance;
  cs.hMenu          = 0;
  cs.hwndParent     = m_hParent;
  cs.cy             = m_Height;
  cs.cx             = m_Width;
  cs.y              = (rect.bottom - m_Height) / 2;
  cs.x              = (rect.right - m_Width) / 2;
  cs.style          = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU; 
  cs.lpszName       = caption.c_str();
  cs.lpszClass      = INPUTBOX_CLASS_NAME;
  cs.dwExStyle      = 0; //WS_EX_TOPMOST;
  cs.lpCreateParams = this; //https://docs.microsoft.com/en-us/windows/desktop/LearnWin32/managing-application-state-

  m_hInputBox = ::CreateWindowEx(
    cs.dwExStyle,
    cs.lpszClass,
    cs.lpszName,
    cs.style,
    cs.x,
    cs.y,
    cs.cx,
    cs.cy,
    cs.hwndParent,
    cs.hMenu,
    cs.hInstance,
    cs.lpCreateParams
  ); 

  //Validate that WndProc captured WM_CREATE message and properly created all controls
  if(m_hInputBox == NULL)
    return FALSE;
  if(m_hTextBoxAnswer == NULL)
    return FALSE;
  if(m_hButtonOK == NULL)
    return FALSE;
  if(m_hButtonCancel == NULL)
    return FALSE;
  if(m_hLabelPrompt == NULL)
    return FALSE;
  
  SetWindowText(m_hLabelPrompt, prompt.c_str());

  EnableWindow(m_hParent, FALSE);

  SetForegroundWindow(m_hInputBox);
  ShowWindow(m_hInputBox, SW_SHOW);
  UpdateWindow(m_hInputBox);

  bool result = false;

  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) 
  {       
    if (msg.message == WM_KEYDOWN) 
    {
      switch(msg.wParam)
      {
      case VK_ESCAPE:
        {
          //Exit the window like if user have pressed CANCEL button
          //Note, pressing the CANCEL button also send this message.
          SendMessage(m_hInputBox, WM_DESTROY, 0, 0);
          result = false; //CANCEL button
        }
        break;
      case VK_RETURN:
        {
          //Make a copy of the textbox's value
          size_t length = (size_t)GetWindowTextLength(m_hTextBoxAnswer);
          m_Text.assign(length, 0);
          if (m_Text.size() == length && length > 0)
          {
            GetWindowText(m_hTextBoxAnswer, &m_Text[0], length+1); //GetWindowText() expect length to also include the NULL terminating character
          }

          //Exit the window like if user have pressed OK button
          //Note, pressing the OK button also send this message.
          SendMessage(m_hInputBox, WM_DESTROY, 0, 0);
          result = true; //OK button
        }
        break;
      case VK_TAB:
        {
          //Jump to the next focusable element
          HWND focused_ctrl = GetFocus();
          if (focused_ctrl == m_hTextBoxAnswer) SetFocus(m_hButtonOK     );
          if (focused_ctrl == m_hButtonOK     ) SetFocus(m_hButtonCancel );
          if (focused_ctrl == m_hButtonCancel ) SetFocus(m_hTextBoxAnswer);
        }
        break;
      };
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);      
  }

  return result;
}

BOOL CInputBox::UpdateWindowStyle(HWND hWnd, LONG new_styles)
{
  //https://stackoverflow.com/questions/2398746/removing-window-border
  //https://stackoverflow.com/questions/17896059/setwindowlong-function-doesnt-change-window-style-even-after-calling-setwi

  //Get current values for debugging
#ifndef NDEBUG //if DEBUG MODE
  LONG styles   = GetWindowLongPtr(hWnd, GWL_STYLE);
  LONG stylesex = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
#endif

  LONG result = SetWindowLongPtr(hWnd, GWL_EXSTYLE, new_styles);

  //Notify the window of the change
  BOOL success=SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  return success;
}
