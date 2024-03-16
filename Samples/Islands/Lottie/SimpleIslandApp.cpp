﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// SimpleIslandApp.cpp : Defines the entry point for the application.

#include "pch.h"
#include "SimpleIslandApp.h"

#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage
#include <winrt/LottieIsland.h>
//#include <winrt/LottieIsland2.h>
#include <winrt/AnimatedVisuals.h>
//#include <winrt/LottieVisualWinRT.h>

namespace winrt
{
    using namespace winrt::Microsoft::UI;
    using namespace winrt::Microsoft::UI::Content;
    using namespace winrt::Microsoft::UI::Dispatching;
    using float2 = winrt::Windows::Foundation::Numerics::float2;
}

// Forward declarations of functions included in this code module:
void                MyRegisterClass(HINSTANCE hInstance, const wchar_t* szWindowClass);
HWND                InitInstance(HINSTANCE, int, const wchar_t* szTitle, const wchar_t* szWindowClass);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool                ProcessMessageForTabNavigation(const HWND topLevelWindow, MSG* msg);

// Extra state for our top-level window, we point to from GWLP_USERDATA.
struct WindowInfo
{
    // winrt::DesktopWindowXamlSource DesktopWindowXamlSource{ nullptr };
    winrt::Microsoft::UI::Composition::Compositor Compositor{};
    winrt::DesktopChildSiteBridge Bridge{ nullptr };
    winrt::event_token TakeFocusRequestedToken{};
    HWND LastFocusedWindow{ NULL };
    winrt::LottieIsland::LottieContentIsland LottieIsland{ nullptr };
    bool isPaused = false;
};

enum class ButtonType
{
    PlayButton = 1,
    PauseButton,
    StopButton
};

constexpr int k_padding = 10;
constexpr int k_buttonWidth = 150;
constexpr int k_buttonHeight = 40;

void LayoutButton(ButtonType type, int tlwWidth, int tlwHeight, HWND topLevelWindow);
void CreateWin32Button(ButtonType type, const std::wstring_view& text, HWND parentHwnd);
void OnButtonClicked(ButtonType type, WindowInfo* windowInfo, HWND topLevelWindow);
void SetButtonText(ButtonType type, const std::wstring_view& text, HWND topLevelWindow);
void SetPauseState(WindowInfo* windowInfo, bool isPaused, HWND topLevelWindow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try
    {
        // Island-support: Call init_apartment to initialize COM and WinRT for the thread.
        winrt::init_apartment(winrt::apartment_type::single_threaded);

        // Island-support: We must start a DispatcherQueueController before we can create an island or use Xaml.
        auto dispatcherQueueController{ winrt::DispatcherQueueController::CreateOnCurrentThread() };

        // Island-support: Create our custom Xaml App object. This is needed to properly use the controls and metadata
        // in Microsoft.ui.xaml.controls.dll.
        // auto simpleIslandApp{ winrt::make<winrt::SimpleIslandApp::implementation::App>() };

        // The title bar text
        WCHAR szTitle[100];
        winrt::check_bool(LoadStringW(hInstance, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle)) != 0);

        // The main window class name
        WCHAR szWindowClass[100];
        winrt::check_bool(LoadStringW(hInstance, IDC_SIMPLEISLANDAPP, szWindowClass, ARRAYSIZE(szWindowClass)) != 0);

        MyRegisterClass(hInstance, szWindowClass);

        // Perform application initialization:
        HWND topLevelWindow = InitInstance(hInstance, nCmdShow, szTitle, szWindowClass);

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEISLANDAPP));

        MSG msg{};

        // Main message loop:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            // Island-support: It's important to call ContentPreTranslateMessage in the event loop so that WinAppSDK can be aware of
            // the messages.  If you don't need to use an accelerator table, you could just call DispatcherQueue.RunEventLoop
            // to do the message pump for you (it will call ContentPreTranslateMessage automatically).
            if (::ContentPreTranslateMessage(&msg))
            {
                continue;
            }

            if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                continue;
            }

            // Island-support: This is needed so that the user can correctly tab and shift+tab into islands.
            if (ProcessMessageForTabNavigation(topLevelWindow, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Island-support: To properly shut down after using a DispatcherQueue, call ShutdownQueue[Aysnc]().
        dispatcherQueueController.ShutdownQueue();
    }
    catch (const winrt::hresult_error& exception)
    {
        // An exception was thrown, let's make the exit code the HR value of the exception.
        return exception.code().value;
    }

    return 0;
}

// Returns "true" if the function handled the message and it shouldn't be processed any further.
// Intended to be called from the main message loop.
bool ProcessMessageForTabNavigation(const HWND /*topLevelWindow*/, MSG* /*msg*/)
{
    //if (msg->message == WM_KEYDOWN && msg->wParam == VK_TAB)
    //{
    //    // The user is pressing the "tab" key.  We want to handle this ourselves so we can pass information into Xaml
    //    // about the tab navigation.  Specifically, we need to tell Xaml whether this is a forward tab, or a backward
    //    // shift+tab, so Xaml will know whether to put focus on the first Xaml element in the island or the last
    //    // Xaml element.  (This is done in the call to DesktopWindowXamlSource.NavigateFocus()).
    //    const HWND currentFocusedWindow = ::GetFocus();
    //    if (::GetAncestor(currentFocusedWindow, GA_ROOT) != topLevelWindow)
    //    {
    //        // This is a window outside of our top-level window, let the system process it.
    //        return false;
    //    }

    //    const bool isShiftKeyDown = ((HIWORD(::GetKeyState(VK_SHIFT)) & 0x8000) != 0);
    //    const HWND nextFocusedWindow = ::GetNextDlgTabItem(topLevelWindow, currentFocusedWindow, isShiftKeyDown /*bPrevious*/);

    //    WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(topLevelWindow, GWLP_USERDATA));
    //    const HWND dwxsWindow = winrt::GetWindowFromWindowId(windowInfo->DesktopWindowXamlSource.SiteBridge().WindowId());
    //    if (dwxsWindow == nextFocusedWindow)
    //    {
    //        // Focus is moving to our DesktopWindowXamlSource.  Instead of just calling SetFocus on it, we call NavigateFocus(),
    //        // which allows us to tell Xaml which direction the keyboard focus is moving.
    //        // If your app has multiple DesktopWindowXamlSources in the window, you'll want to loop over them and check to
    //        // see if focus is moving to each one.
    //        winrt::XamlSourceFocusNavigationRequest request{
    //            isShiftKeyDown ?
    //                winrt::XamlSourceFocusNavigationReason::Last :
    //                winrt::XamlSourceFocusNavigationReason::First };

    //        windowInfo->DesktopWindowXamlSource.NavigateFocus(request);
    //        return true;
    //    }

    //    // Focus isn't moving to our DesktopWindowXamlSource.  IsDialogMessage will automatically do the tab navigation
    //    // for us for this msg.
    //    const bool handled = (::IsDialogMessage(topLevelWindow, msg) == TRUE);
    //    return handled;
    //}
    return false;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
void MyRegisterClass(HINSTANCE hInstance, const wchar_t* szWindowClass)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEISLANDAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SIMPLEISLANDAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    winrt::check_bool(RegisterClassExW(&wcex) != 0);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE /*hInstance*/, int nCmdShow, const wchar_t* szTitle, const wchar_t* szWindowClass)
{
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, ::GetModuleHandle(NULL), nullptr);
   winrt::check_bool(hWnd != NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
        {
            windowInfo = new WindowInfo();
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowInfo));

            windowInfo->Bridge = winrt::DesktopChildSiteBridge::Create(
                windowInfo->Compositor,
                winrt::GetWindowIdFromWindow(hWnd));

            //windowInfo->LottieIsland = winrt::LottieIsland::LottieContentIsland{ windowInfo->Compositor };
            windowInfo->LottieIsland = winrt::LottieIsland::LottieContentIsland{ windowInfo->Compositor };

            windowInfo->Bridge.Connect(windowInfo->LottieIsland.Island());
            windowInfo->Bridge.Show();

            // C++/WinRT precompiled animation!
            windowInfo->LottieIsland.AnimatedVisualSource(winrt::AnimatedVisuals::LottieLogo1());

            // Live JSON loaded animation! (this fails beause lottie creates a dependency object)
            //winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource animatedVisualSource = winrt::LottieVisualWinRT::LottieAnimatedVisualWinRT::LoadLottie(L"ms-appx:///LottieLogo1.json");
            //windowInfo->LottieIsland.AnimatedVisualSource(animatedVisualSource);

            CreateWin32Button(ButtonType::PlayButton, L"Play", hWnd);
            CreateWin32Button(ButtonType::PauseButton, L"Pause", hWnd);
            CreateWin32Button(ButtonType::StopButton, L"Stop", hWnd);

            // Subscribe to the TakeFocusRequested event, which will be raised when Xaml wants to move keyboard focus back to our window.
            //windowInfo->TakeFocusRequestedToken = windowInfo->DesktopWindowXamlSource.TakeFocusRequested(
            //    [hWnd](winrt::DesktopWindowXamlSource const& /*sender*/, winrt::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args) {
            //        if (args.Request().Reason() == winrt::XamlSourceFocusNavigationReason::First)
            //        {
            //            // The reason "First" means the user is tabbing forward, so put the focus on the button in the tab order
            //            // after the DesktopWindowXamlSource.
            //            ::SetFocus(::GetDlgItem(hWnd, 502));
            //        }
            //        else if (args.Request().Reason() == winrt::XamlSourceFocusNavigationReason::Last)
            //        {
            //            // The reason "Last" means the user is tabbing backward (shift-tab, so put the focus on button prior to
            //            // the DesktopWindowXamlSource.
            //            ::SetFocus(::GetDlgItem(hWnd, 501));
            //        }
            //    });
        }
        break;
    case WM_SIZE:
        {
            const int width = LOWORD(lParam);
            const int height = HIWORD(lParam);

            if (windowInfo->Bridge)
            {
                windowInfo->Bridge.MoveAndResize({ k_padding, k_padding, width - (k_padding*2), height - (k_padding*3) - k_buttonHeight });
            }

            LayoutButton(ButtonType::PlayButton, width, height, hWnd);
            LayoutButton(ButtonType::PauseButton, width, height, hWnd);
            LayoutButton(ButtonType::StopButton, width, height, hWnd);
        }
        break;
    case WM_ACTIVATE:
        {
            // Make focus work nicely when the user presses alt+tab to activate a different window, and then alt+tab
            // again to come back to this window.  We want the focus to go back to the same child HWND that was focused
            // before.
            const bool isGettingDeactivated = (LOWORD(wParam) == WA_INACTIVE);
            if (isGettingDeactivated)
            {
                // Remember the HWND that had focus.
                windowInfo->LastFocusedWindow = ::GetFocus();
            }
            else if (windowInfo->LastFocusedWindow != NULL)
            {
                ::SetFocus(windowInfo->LastFocusedWindow);
            }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            int wmCode = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case 501: // Buttons
            case 502:
            case 503:
                if (wmCode == BN_CLICKED)
                {
                    ButtonType type = static_cast<ButtonType>(wmId - 500);
                    OnButtonClicked(type, windowInfo, hWnd);
                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            UNREFERENCED_PARAMETER(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NCDESTROY:
        /*if (windowInfo->DesktopWindowXamlSource && windowInfo->TakeFocusRequestedToken.value != 0)
        {
            windowInfo->DesktopWindowXamlSource.TakeFocusRequested(windowInfo->TakeFocusRequestedToken);
            windowInfo->TakeFocusRequestedToken = {};
        }*/
        delete windowInfo;
        ::SetWindowLong(hWnd, GWLP_USERDATA, NULL);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void LayoutButton(ButtonType type, int /*tlwWidth*/, int tlwHeight, HWND topLevelWindow)
{
    int buttonIndex = static_cast<int>(type);

    int xPos = ((buttonIndex - 1) * (k_buttonWidth + k_padding)) + k_padding;
    int yPos = tlwHeight - k_buttonHeight - k_padding;

    HWND buttonHwnd = ::GetDlgItem(topLevelWindow, 500 + buttonIndex);
    ::SetWindowPos(buttonHwnd, NULL, xPos, yPos, k_buttonWidth, k_buttonHeight, SWP_NOZORDER);
}

void CreateWin32Button(ButtonType type, const std::wstring_view& text, HWND parentHwnd)
{
    int buttonIndex = static_cast<int>(type);

    int xPos = ((buttonIndex - 1) * (k_buttonWidth + k_padding)) + k_padding;

    const HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);
    HMENU fakeHMenu = reinterpret_cast<HMENU>(static_cast<intptr_t>(500 + buttonIndex));
    ::CreateWindowW(
        L"BUTTON",
        text.data(),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        xPos, 250, k_buttonWidth, k_buttonHeight,
        parentHwnd,
        fakeHMenu,
        hInst,
        NULL);
}

void OnButtonClicked(ButtonType type, WindowInfo* windowInfo, HWND topLevelWindow)
{
    switch (type)
    {
    case ButtonType::PlayButton:
        windowInfo->LottieIsland.PlayAsync(0.0, 1.0, true);
        SetPauseState(windowInfo, false, topLevelWindow);
        break;
    case ButtonType::PauseButton:
        if (windowInfo->isPaused)
        {
            windowInfo->LottieIsland.Resume();
        }
        else
        {
            windowInfo->LottieIsland.Pause();
        }
        SetPauseState(windowInfo, !windowInfo->isPaused, topLevelWindow);
        break;
    case ButtonType::StopButton:
        windowInfo->LottieIsland.Stop();
        SetPauseState(windowInfo, false, topLevelWindow);
        break;
    default:
        throw winrt::hresult_invalid_argument{ L"Invalid button type." };
    }
}

void SetButtonText(ButtonType type, const std::wstring_view& text, HWND topLevelWindow)
{
    int buttonIndex = static_cast<int>(type);
    HWND buttonHwnd = ::GetDlgItem(topLevelWindow, 500 + buttonIndex);
    ::SendMessageW(buttonHwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text.data()));
}

void SetPauseState(WindowInfo* windowInfo, bool isPaused, HWND topLevelWindow)
{
    if (windowInfo->isPaused == isPaused)
    {
        return;
    }

    SetButtonText(ButtonType::PauseButton,
        isPaused ? L"Resume" : L"Pause",
        topLevelWindow);

    windowInfo->isPaused = isPaused;
}
