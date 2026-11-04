#include "ui/main_window.hpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show_command)
{
    guoke::MainWindow window;
    if (!window.create(instance))
    {
        guoke::show_windows_error(nullptr, L"Could not create the guoke window",
                                  window.creation_error());
        return 1;
    }

    window.show(show_command);

    MSG message{};
    int result = 0;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0)
    {
        if (!IsDialogMessageW(window.handle(), &message))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    if (result == -1)
    {
        guoke::show_windows_error(window.handle(), L"Could not read window messages",
                                  GetLastError());
        return 1;
    }
    return static_cast<int>(message.wParam);
}
