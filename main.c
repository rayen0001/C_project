#include <windows.h>
#include <sqlite3.h>

#include "database.h"
#include "window.h"

sqlite3* db;

int main() {
    initializeDatabase();
    HINSTANCE hInstance = GetModuleHandle(NULL);
    createMainWindow(hInstance);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    sqlite3_close(db);

    return 0;
}
