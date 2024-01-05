#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sqlite3.h>
#include "crud.h"


extern sqlite3* db;


ProductList getAllProducts() {
    ProductList productList = {NULL, NULL};
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, "SELECT name, quantity, price, alert_threshold FROM stock;", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product product;
            strncpy(product.name, (const char*)sqlite3_column_text(stmt, 0), sizeof(product.name));
            product.quantity = sqlite3_column_int(stmt, 1);
            product.price = sqlite3_column_double(stmt, 2);
            product.alertThreshold = sqlite3_column_int(stmt, 3);


            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->product = product;
            newNode->next = NULL;


            if (productList.head == NULL) {
                productList.head = productList.tail = newNode;
            } else {
                productList.tail->next = newNode;
                productList.tail = newNode;
            }
        }
        sqlite3_finalize(stmt);
    }

    return productList;
}

void addProduct(char name[], int quantity, float price, int alertThreshold) {
    char query[100];
    sprintf(query, "INSERT INTO stock (name, quantity, price, alert_threshold) VALUES ('%s', %d, %.2f, %d);", name, quantity, price, alertThreshold);
    char* errMsg;
    sqlite3_exec(db, query, NULL, NULL, &errMsg);


    if (quantity < alertThreshold) {
        char alertMsg[100];
        sprintf(alertMsg, "Alert: Quantity for product %s has reached or exceeded the threshold of %d.", name, alertThreshold);
        MessageBox(NULL, alertMsg, "Quantity Alert", MB_OK | MB_ICONWARNING);
    }
}

void deleteProduct(char name[]) {
    char query[100];
    sprintf(query, "DELETE FROM stock WHERE name like'%s';", name);
    char* errMsg;
    int result = sqlite3_exec(db, query, NULL, NULL, &errMsg);
}

void updateProduct(char name[], int newQuantity, float newPrice, int newThreshold) {
    char sql[1000];


    snprintf(sql, sizeof(sql), "UPDATE stock SET ");


    if (newQuantity >= 0) {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), "quantity=%d, ", newQuantity);
    }

    if (newPrice >= 0) {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), "price=%.2f, ", newPrice);
    }

    if (newThreshold >= 0) {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), "alert_threshold=%d, ", newThreshold);
    }


    size_t len = strlen(sql);
    if (len >= 2) {
        sql[len - 2] = '\0';
    }
    snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " WHERE name='%s';", name);


    int result = sqlite3_exec(db, sql, 0, 0, 0);

    if (result != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
}



void checkAlert(HWND hwnd) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT name, quantity, alert_threshold FROM stock WHERE quantity < alert_threshold;", -1, &stmt, 0);

    char alertMsg[1000] = "Alerts:\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char product[100];
        sprintf(product, "Product: %s, Quantity: %d, Threshold: %d\n",
            sqlite3_column_text(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2));
        strcat(alertMsg, product);
    }

    if (strlen(alertMsg) > 8) {
        MessageBox(hwnd, alertMsg, "Quantity Alerts", MB_OK | MB_ICONWARNING);
    } else {
        MessageBox(hwnd, "No quantity alerts.", "Quantity Alerts", MB_OK);
    }

    sqlite3_finalize(stmt);
}
