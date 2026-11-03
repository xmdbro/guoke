#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        constexpr char message[] = "Hello from guoke_demo_dll!\r\n";
        const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output != nullptr && output != INVALID_HANDLE_VALUE)
        {
            // manage size and avoid CRT or GUI initialisation here
            DWORD written = 0;
            WriteFile(output, message, static_cast<DWORD>(sizeof(message) - 1),
                      &written, nullptr);
        }
    }
    return TRUE;
}
