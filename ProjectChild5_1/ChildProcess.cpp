#include <iostream>
#include <Windows.h>

int main() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Відкриваємо спільну пам'ять
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        L"SharedMemory");

    if (hMapFile == NULL) {
        std::cerr << "OpenFileMapping failed with error " << GetLastError() << std::endl;
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

    int lastValue = *((int*)pBuf);

    while (true) {
        int newValue = *((int*)pBuf);
        if (newValue != lastValue) {
            std::cout << "Value changed: " << newValue << std::endl;
            lastValue = newValue;
            if (newValue == 0) {
                break; // Вийти з циклу при значенні 0
            }
        }
        Sleep(1000);  // Затримка 1 секунда
    }

    // Закриваємо спільну пам'ять
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}