#include "main_window.hpp"

#include <commdlg.h>

#include <algorithm>

namespace guoke
{
namespace
{
constexpr wchar_t window_class[] = L"GuokeMainWindow";
constexpr DWORD window_style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
constexpr DWORD window_ex_style = WS_EX_CONTROLPARENT;
constexpr int browse_id = 100;
}

void show_windows_error(HWND owner, const wchar_t* operation, DWORD error)
{
    std::array<wchar_t, 1024> description{};
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, error, 0, description.data(),
                   static_cast<DWORD>(description.size()), nullptr);
    const std::wstring text = std::wstring(operation) + L".\nWindows error "
                              + std::to_wstring(error) + L": " + description.data();
    MessageBoxW(owner, text.c_str(), L"guoke", MB_OK | MB_ICONERROR);
}

MainWindow::~MainWindow()
{
    if (window_ != nullptr)
    {
        DestroyWindow(window_);
    }
    if (font_ != nullptr)
    {
        DeleteObject(font_);
    }
}

bool MainWindow::create(HINSTANCE instance)
{
    instance_ = instance;
    WNDCLASSEXW definition{};
    definition.cbSize = sizeof(definition);
    definition.lpfnWndProc = window_proc;
    definition.hInstance = instance;
    definition.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    definition.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    definition.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    definition.lpszClassName = window_class;
    if (RegisterClassExW(&definition) == 0)
    {
        creation_error_ = GetLastError();
        return false;
    }

    dpi_ = GetDpiForSystem();
    RECT bounds{0, 0, scale(560), scale(280)};
    AdjustWindowRectExForDpi(&bounds, window_style, FALSE, window_ex_style, dpi_);
    const HWND created = CreateWindowExW(window_ex_style, window_class, L"guoke",
                                         window_style, CW_USEDEFAULT, CW_USEDEFAULT,
                                         bounds.right - bounds.left,
                                         bounds.bottom - bounds.top,
                                         nullptr, nullptr, instance, this);
    if (created == nullptr)
    {
        if (creation_error_ == ERROR_SUCCESS)
        {
            creation_error_ = GetLastError();
        }
        return false;
    }
    return true;
}

void MainWindow::show(int show_command) const
{
    ShowWindow(window_, show_command);
    UpdateWindow(window_);
}

int MainWindow::scale(int value) const
{
    return MulDiv(value, static_cast<int>(dpi_), USER_DEFAULT_SCREEN_DPI);
}

bool MainWindow::create_controls()
{
    const auto add = [this](Control index, const wchar_t* type, const wchar_t* text,
                           DWORD style, DWORD extended = 0, int id = 0)
    {
        controls_[index] = CreateWindowExW(extended, type, text, WS_CHILD | WS_VISIBLE | style,
                                           0, 0, 0, 0, window_,
                                           reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                                           instance_, nullptr);
        if (controls_[index] == nullptr && creation_error_ == ERROR_SUCCESS)
        {
            creation_error_ = GetLastError();
        }
    };

    add(process_label, L"STATIC", L"&Process", SS_LEFT);
    add(process_picker, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL);
    add(refresh_button, L"BUTTON", L"&Refresh", BS_PUSHBUTTON | WS_TABSTOP);
    add(dll_label, L"STATIC", L"&DLL", SS_LEFT);
    add(dll_path, L"EDIT", L"", ES_READONLY | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE);
    add(browse_button, L"BUTTON", L"&Browse...", BS_PUSHBUTTON | WS_TABSTOP, 0, browse_id);
    add(load_button, L"BUTTON", L"&Load", BS_PUSHBUTTON | WS_TABSTOP);
    add(status_label, L"STATIC", L"Status: No process selected.", SS_LEFT | SS_NOPREFIX);

    if (std::any_of(controls_.begin(), controls_.end(), [](HWND control) { return control == nullptr; }))
    {
        return false;
    }

    SendMessageW(controls_[process_picker], CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"No process selected"));
    SendMessageW(controls_[process_picker], CB_SETCURSEL, 0, 0);
    EnableWindow(controls_[process_picker], FALSE);
    EnableWindow(controls_[refresh_button], FALSE);
    EnableWindow(controls_[load_button], FALSE);
    update_font();
    layout();
    return true;
}

void MainWindow::update_font()
{
    NONCLIENTMETRICSW metrics{};
    metrics.cbSize = sizeof(metrics);
    HFONT replacement = nullptr;
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi_))
    {
        replacement = CreateFontIndirectW(&metrics.lfMessageFont);
    }
    const HFONT applied = replacement != nullptr
                              ? replacement : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    for (HWND control : controls_)
    {
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(applied), TRUE);
    }
    if (font_ != nullptr)
    {
        DeleteObject(font_);
    }
    font_ = replacement;
}

void MainWindow::layout() const
{
    if (controls_[status_label] == nullptr)
    {
        return;
    }
    RECT client{};
    GetClientRect(window_, &client);
    const int margin = scale(20);
    const int button_width = scale(96);
    const int right = client.right - margin - button_width;
    const int field_width = std::max(1, right - scale(10) - margin);
    const auto move = [this](Control control, int x, int y, int width, int height)
    {
        MoveWindow(controls_[control], x, y, width, height, TRUE);
    };

    move(process_label, margin, scale(18), field_width, scale(20));
    move(process_picker, margin, scale(42), field_width, scale(200));
    move(refresh_button, right, scale(42), button_width, scale(30));
    move(dll_label, margin, scale(94), field_width, scale(20));
    move(dll_path, margin, scale(118), field_width, scale(30));
    move(browse_button, right, scale(118), button_width, scale(30));
    move(load_button, right, client.bottom - margin - scale(32), button_width, scale(32));
    move(status_label, margin, client.bottom - margin - scale(90),
         std::max(1, static_cast<int>(client.right) - 2 * margin), scale(42));
}

void MainWindow::browse_dll()
{
    std::array<wchar_t, 32768> path{};
    std::copy(selected_dll_.begin(), selected_dll_.end(), path.begin());
    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = window_;
    dialog.lpstrFilter = L"Dynamic-link libraries (*.dll)\0*.dll\0\0";
    dialog.lpstrFile = path.data();
    dialog.nMaxFile = static_cast<DWORD>(path.size());
    dialog.lpstrTitle = L"Select a DLL";
    dialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
                   | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_DONTADDTORECENT;
    if (GetOpenFileNameW(&dialog))
    {
        selected_dll_ = path.data();
        SetWindowTextW(controls_[dll_path], selected_dll_.c_str());
        SetWindowTextW(controls_[status_label], L"Status: DLL selected. No process selected.");
    }
    else if (const DWORD error = CommDlgExtendedError(); error != 0)
    {
        const std::wstring status = L"Status: File selection failed (dialog error "
                                    + std::to_wstring(error) + L"). Try again.";
        SetWindowTextW(controls_[status_label], status.c_str());
    }
}

LRESULT CALLBACK MainWindow::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    auto* self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE)
    {
        self = static_cast<MainWindow*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
        self->window_ = window;
        self->dpi_ = GetDpiForWindow(window);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (self == nullptr)
    {
        return DefWindowProcW(window, message, wparam, lparam);
    }
    const LRESULT result = self->handle_message(message, wparam, lparam);
    if (message == WM_NCDESTROY)
    {
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        self->window_ = nullptr;
    }
    return result;
}

LRESULT MainWindow::handle_message(UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:
        return create_controls() ? 0 : -1;
    case WM_COMMAND:
        if (LOWORD(wparam) == browse_id && HIWORD(wparam) == BN_CLICKED)
        {
            browse_dll();
            return 0;
        }
        break;
    case WM_SETFOCUS:
        SetFocus(controls_[browse_button]);
        return 0;
    case WM_SIZE:
        layout();
        return 0;
    case WM_GETMINMAXINFO:
        {
            RECT bounds{0, 0, scale(480), scale(280)};
            AdjustWindowRectExForDpi(&bounds, window_style, FALSE, window_ex_style, dpi_);
            auto* limits = reinterpret_cast<MINMAXINFO*>(lparam);
            limits->ptMinTrackSize = {bounds.right - bounds.left, bounds.bottom - bounds.top};
        }
        return 0;
    case WM_DPICHANGED:
        {
            dpi_ = HIWORD(wparam);
            update_font();
            const auto* bounds = reinterpret_cast<const RECT*>(lparam);
            SetWindowPos(window_, nullptr, bounds->left, bounds->top,
                         bounds->right - bounds->left, bounds->bottom - bounds->top,
                         SWP_NOACTIVATE | SWP_NOZORDER);
            layout();
        }
        return 0;
    case WM_SETTINGCHANGE:
        update_font();
        layout();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window_, message, wparam, lparam);
}
} // namespace guoke
