#pragma once

#include <windows.h>

#include <array>
#include <string>

namespace guoke
{
void show_windows_error(HWND owner, const wchar_t* operation, DWORD error);

class MainWindow final
{
public:
    MainWindow() = default;
    ~MainWindow();
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

    bool create(HINSTANCE instance);
    void show(int show_command) const;
    HWND handle() const { return window_; }
    DWORD creation_error() const { return creation_error_; }

private:
    enum Control : std::size_t
    {
        process_label, process_picker, refresh_button,
        dll_label, dll_path, browse_button, load_button, status_label,
        control_count
    };

    static LRESULT CALLBACK window_proc(HWND window, UINT message,
                                         WPARAM wparam, LPARAM lparam);
    LRESULT handle_message(UINT message, WPARAM wparam, LPARAM lparam);
    bool create_controls();
    void update_font();
    void layout() const;
    void browse_dll();
    int scale(int value) const;

    HWND window_ = nullptr;
    HINSTANCE instance_ = nullptr;
    HFONT font_ = nullptr;
    UINT dpi_ = USER_DEFAULT_SCREEN_DPI;
    DWORD creation_error_ = ERROR_SUCCESS;
    std::array<HWND, control_count> controls_{};
    std::wstring selected_dll_;
};
} // namespace guoke
