#include <iostream>
#include <Windows.h>

int main() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Створення спільної пам'яті
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(int),
        L"SharedMemory");

    if (hMapFile == NULL) {
        std::cerr << "CreateFileMapping failed with error " << GetLastError() << std::endl;
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(int));

    if (pBuf == NULL) {
        std::cerr << "MapViewOfFile failed with error " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Створення дочірнього процесу
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
        L"ChildProcess.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)) {

        std::cerr << "CreateProcess failed with error " << GetLastError() << std::endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 1;
    }

    int inputValue;
    do {
        std::cout << "Enter an integer value (0 to exit): " << std::endl;
        std::cin >> inputValue;

        // Запис значення у спільну пам'ять
        memcpy((LPVOID)pBuf, &inputValue, sizeof(int));


    } while (inputValue != 0);

    // Закриваємо дескриптори
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Закриваємо спільну пам'ять
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}