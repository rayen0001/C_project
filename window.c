#include <windows.h>
#include <commctrl.h>
#include "window.h"
#include "database.h"
#include "crud.h"
#include <stdio.h>

HWND g_hListView;

void createControls(HWND hwnd);
LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void displayAllProductsInListView(HWND hwndListView);

float GetDlgItemFloat(HWND hDlg, int nIDDlgItem) {
    char buffer[50];
    GetDlgItemText(hDlg, nIDDlgItem, buffer, sizeof(buffer));
    return atof(buffer);
}

void addListViewColumn(HWND hwndListView, int iCol, char *szText, int iWidth) {
    LV_COLUMN lvCol;
    lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvCol.fmt = LVCFMT_LEFT;
    lvCol.pszText = szText;
    lvCol.cx = iWidth;
    lvCol.iSubItem = iCol;
    ListView_InsertColumn(hwndListView, iCol, &lvCol);
}

void addListViewItem(HWND hwndListView, int iItem, int iSubItem, char *szText) {
    LV_ITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = iItem;
    lvItem.iSubItem = iSubItem;
    lvItem.pszText = szText;
    ListView_InsertItem(hwndListView, &lvItem);
}

void createMainWindow(HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "StockManagementClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        "StockManagementClass",
        "Stock Management",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 400,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);
}

void createControls(HWND hwnd) {
    CreateWindow("STATIC", "Product Name:", WS_VISIBLE | WS_CHILD, 10, 10, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 10, 150, 20, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow("STATIC", "Quantity:", WS_VISIBLE | WS_CHILD, 10, 40, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 40, 150, 20, hwnd, (HMENU)2, NULL, NULL);

    CreateWindow("STATIC", "Price:", WS_VISIBLE | WS_CHILD, 10, 70, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 70, 150, 20, hwnd, (HMENU)3, NULL, NULL);

    CreateWindow("STATIC", "Threshold:", WS_VISIBLE | WS_CHILD, 10, 100, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 100, 150, 20, hwnd, (HMENU)4, NULL, NULL);

    CreateWindow("BUTTON", "Add Product", WS_VISIBLE | WS_CHILD, 10, 130, 120, 30, hwnd, (HMENU)5, NULL, NULL);
    CreateWindow("BUTTON", "Delete Product", WS_VISIBLE | WS_CHILD, 140, 130, 120, 30, hwnd, (HMENU)6, NULL, NULL);
    CreateWindow("BUTTON", "Update Product", WS_VISIBLE | WS_CHILD, 270, 130, 120, 30, hwnd, (HMENU)7, NULL, NULL);
    CreateWindow("BUTTON", "Check Alert", WS_VISIBLE | WS_CHILD, 400, 130, 120, 30, hwnd, (HMENU)9, NULL, NULL);

    g_hListView = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,
        "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        10, 180, 660, 150,
        hwnd,
        (HMENU)10,
        GetModuleHandle(NULL),
        NULL);

    addListViewColumn(g_hListView, 0, "Product Name", 200);
    addListViewColumn(g_hListView, 1, "Quantity", 80);
    addListViewColumn(g_hListView, 2, "Price", 80);
    addListViewColumn(g_hListView, 3, "Threshold", 80);

    displayAllProductsInListView(g_hListView);
}

void displayAllProductsInListView(HWND hwnd) {
    ListView_DeleteAllItems(g_hListView);
    ProductList productList = getAllProducts();
    Node* current = productList.head;

    int itemIndex = 0;

    while (current != NULL) {

        char quantityStr[20], priceStr[20], alertThresholdStr[20];

        snprintf(quantityStr, sizeof(quantityStr), "%d", current->product.quantity);
        snprintf(priceStr, sizeof(priceStr), "%.2f", current->product.price);
        snprintf(alertThresholdStr, sizeof(alertThresholdStr), "%d", current->product.alertThreshold);

        LV_ITEM lvItem = {0};
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = itemIndex;
        lvItem.iSubItem = 0;
        lvItem.pszText = current->product.name;
        ListView_InsertItem(g_hListView, &lvItem);

        ListView_SetItemText(g_hListView, itemIndex, 1, quantityStr);
        ListView_SetItemText(g_hListView, itemIndex, 2, priceStr);
        ListView_SetItemText(g_hListView, itemIndex, 3, alertThresholdStr);

        current = current->next;
        itemIndex++;
    }
}



LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            createControls(hwnd);
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 5: // Add Product
                    {
                        char name[50];
                        int quantity;
                        float price;
                        int threshold;

                        GetDlgItemText(hwnd, 1, name, 50);
                        quantity = GetDlgItemInt(hwnd, 2, NULL, FALSE);
                        price = GetDlgItemFloat(hwnd, 3);
                        threshold = GetDlgItemInt(hwnd, 4, NULL, FALSE);

                        addProduct(name, quantity, price, threshold);
                        MessageBox(hwnd, "Product added to stock.", "Add Product", MB_OK);

                        displayAllProductsInListView(g_hListView);
                        break;
                    }
                case 6:
                    {
                        char name[50];
                        GetDlgItemText(hwnd, 1, name, 50);
                        deleteProduct(name);
                        MessageBox(hwnd, "Product deleted from stock.", "Delete Product", MB_OK);


                        displayAllProductsInListView(g_hListView);
                        break;
                    }
                case 7:
                    {
                        char name[50];
                        int quantity;
                        float price;
                        int threshold;

                        GetDlgItemText(hwnd, 1, name, 50);
                        quantity = GetDlgItemInt(hwnd, 2, NULL, FALSE);
                        price = GetDlgItemFloat(hwnd, 3);
                        threshold = GetDlgItemInt(hwnd, 4, NULL, FALSE);

                        updateProduct(name, quantity, price, threshold);
                        MessageBox(hwnd, "Product updated in stock.", "Update Product", MB_OK);


                        displayAllProductsInListView(g_hListView);
                        break;
                    }
                case 9:
                    checkAlert(hwnd);
                    break;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
