#ifndef CRUD_H
#define CRUD_H


typedef struct {
    char name[50];
    int quantity;
    float price;
    int alertThreshold;
} Product;


typedef struct Node {
    Product product;
    struct Node* next;
} Node;


typedef struct {
    Node* head;
    Node* tail;
    int count;
} ProductList;


void addProduct(char name[], int quantity, float price, int alertThreshold);
void deleteProduct(char name[]);
void updateProduct(char name[], int newQuantity, float newPrice, int newThreshold);
ProductList getAllProducts();
void checkAlert(HWND hwnd);

#endif
