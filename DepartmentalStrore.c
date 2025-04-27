#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define MAX_COUPONS 10

struct item {
    int id;
    char name[50];
    float price;
    int qty;
    int sold_qty;
    struct item* next;
};

struct cart_node {
    int id;
    char name[50];
    int qty;
    float price;
    struct cart_node* next;
};

struct coupon {
    char code[20];
    float discount;
};

struct item* store[SIZE];
struct cart_node* cart[SIZE];
float cart_total = 0.0;
struct coupon coupons[MAX_COUPONS];
int coupon_count = 0;
char last_report_filename[100]; 

int hash(int id) {
    return id % SIZE;
}

void copy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0' && i < 49) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void add_store_item() {
    int id, qty;
    float price;
    char name[50];
    printf("\n--- Add Product to Store ---\n");
    printf("Enter product ID: ");
    scanf("%d", &id);
    getchar();
    printf("Enter product name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("Enter product price: ");
    scanf("%f", &price);
    printf("Enter product quantity: ");
    scanf("%d", &qty);
    int index = hash(id);
    struct item* newnode = malloc(sizeof(struct item));
    newnode->id = id;
    copy(newnode->name, name);
    newnode->price = price;
    newnode->qty = qty;
    newnode->sold_qty = 0;
    newnode->next = store[index];
    store[index] = newnode;
    printf("Product added to store.\n");
}

void update_store_item() {
    int id;
    printf("\n--- Update Product ---\n");
    printf("Enter product ID to update: ");
    scanf("%d", &id);
    struct item* product = store[hash(id)];
    while (product) {
        if (product->id == id) {
            printf("Current Name: %s\n", product->name);
            printf("Enter new name (or . to keep same): ");
            char new_name[50];
            getchar();
            fgets(new_name, sizeof(new_name), stdin);
            new_name[strcspn(new_name, "\n")] = '\0';
            if (strcmp(new_name, ".") != 0)
                copy(product->name, new_name);
            printf("Current Price: %.2f\n", product->price);
            printf("Enter new price (or -1 to keep same): ");
            float new_price;
            scanf("%f", &new_price);
            if (new_price >= 0)
                product->price = new_price;
            printf("Current Quantity: %d\n", product->qty);
            printf("Enter new quantity (or -1 to keep same): ");
            int new_qty;
            scanf("%d", &new_qty);
            if (new_qty >= 0)
                product->qty = new_qty;
            printf("Product updated successfully.\n");
            return;
        }
        product = product->next;
    }
    printf("Product not found.\n");
}

struct item* find_store_item(int id) {
    int index = hash(id);
    struct item* ptr = store[index];
    while (ptr) {
        if (ptr->id == id)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

void view_store() {
    printf("\n--- Store Inventory ---\n");
    printf("ID   | Name                | Price   | Qty\n");
    printf("------------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        struct item* ptr = store[i];
        while (ptr) {
            printf("%-5d | %-18s | %-7.2f | %-3d\n", ptr->id, ptr->name, ptr->price, ptr->qty);
            ptr = ptr->next;
        }
    }
}

void insert_cart(int id, char* name, int qty, float price) {
    int index = hash(id);
    struct cart_node* ptr = cart[index];
    while (ptr) {
        if (ptr->id == id) {
            ptr->qty += qty;
            cart_total += price * qty;
            return;
        }
        ptr = ptr->next;
    }
    struct cart_node* newnode = malloc(sizeof(struct cart_node));
    newnode->id = id;
    copy(newnode->name, name);
    newnode->qty = qty;
    newnode->price = price;
    newnode->next = cart[index];
    cart[index] = newnode;
    cart_total += price * qty;
}

void view_cart() {
    printf("\n--- Cart Contents ---\n");
    printf("ID   | Name                | Qty | Price   | Subtotal\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        struct cart_node* ptr = cart[i];
        while (ptr) {
            printf("%-5d | %-18s | %-3d | %-7.2f | %-7.2f\n", ptr->id, ptr->name, ptr->qty, ptr->price, ptr->qty * ptr->price);
            ptr = ptr->next;
        }
    }
    printf("--------------------------------------------------------\n");
    printf("Total: %.2f\n", cart_total);
}

void add_to_cart() {
    int id, qty;
    printf("\n--- Add Product to Cart ---\n");
    printf("Enter product ID to add to cart: ");
    scanf("%d", &id);
    struct item* product = find_store_item(id);
    if (!product) {
        printf("Product not found in store.\n");
        return;
    }
    printf("Enter quantity: ");
    scanf("%d", &qty);
    if (qty > product->qty) {
        printf("Not enough quantity in store.\n");
        return;
    }
    insert_cart(id, product->name, qty, product->price);
    product->qty -= qty;
    product->sold_qty += qty;
    printf("Item added to cart.\n");
}

void add_coupon() {
    if (coupon_count >= MAX_COUPONS) {
        printf("Coupon limit reached.\n");
        return;
    }
    char code[20];
    float discount;
    printf("\n--- Add Coupon ---\n");
    printf("Enter coupon code: ");
    scanf("%s", code);
    printf("Enter discount amount: ");
    scanf("%f", &discount);
    copy(coupons[coupon_count].code, code);
    coupons[coupon_count].discount = discount;
    coupon_count++;
    printf("Coupon added successfully.\n");
}

float apply_coupon() {
    char input_code[20];
    printf("\nDo you have a coupon? (y/n): ");
    char ans;
    getchar();
    scanf("%c", &ans);
    if (ans == 'y' || ans == 'Y') {
        printf("Enter coupon code: ");
        scanf("%s", input_code);
        for (int i = 0; i < coupon_count; i++) {
            if (strcmp(coupons[i].code, input_code) == 0) {
                printf("Coupon applied. Discount: %.2f\n", coupons[i].discount);
                return coupons[i].discount;
            }
        }
        printf("Invalid coupon code.\n");
    }
    return 0.0;
}

void daily_report() {
    char date[20];
    printf("\n--- Daily Report ---\n");
    printf("Enter date for report (e.g., 2025-04-24): ");
    scanf("%s", date);
    sprintf(last_report_filename, "report_%s.txt", date);
    FILE* fp = fopen(last_report_filename, "w");
    if (!fp) {
        printf("Failed to create report file.\n");
        return;
    }
    fprintf(fp, "--- Daily Report for %s ---\n", date);
    fprintf(fp, "ID   | Name                | Sold Qty | Price   | Revenue\n");
    fprintf(fp, "--------------------------------------------------------\n");
    printf("ID   | Name                | Sold Qty | Price   | Revenue\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        struct item* ptr = store[i];
        while (ptr) {
            if (ptr->sold_qty > 0) {
                float revenue = ptr->sold_qty * ptr->price;
                fprintf(fp, "%-5d | %-18s | %-8d | %-7.2f | %.2f\n", ptr->id, ptr->name, ptr->sold_qty, ptr->price, revenue);
                printf("%-5d | %-18s | %-8d | %-7.2f | %.2f\n", ptr->id, ptr->name, ptr->sold_qty, ptr->price, revenue);
            }
            ptr = ptr->next;
        }
    }
    fprintf(fp, "--------------------------------------------------------\n");
    printf("--------------------------------------------------------\n");
    printf("Report saved to file: %s\n", last_report_filename);
    fclose(fp);
}

void checkout() {
    char phone[20], email[50], payment[20];
    printf("\n--- Final Bill ---\n");
    printf("ID   | Name                | Qty | Price   | Subtotal\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        struct cart_node* ptr = cart[i];
        while (ptr) {
            printf("%-5d | %-18s | %-3d | %-7.2f | %-7.2f\n", ptr->id, ptr->name, ptr->qty, ptr->price, ptr->qty * ptr->price);
            ptr = ptr->next;
        }
    }
    printf("--------------------------------------------------------\n");
    float discount = apply_coupon();
    float final_total = cart_total - discount;
    printf("Original Total: %.2f\n", cart_total);
    printf("Discount: %.2f\n", discount);
    printf("Amount Payable: %.2f\n", final_total);
    printf("Enter your phone number: ");
    scanf("%s", phone);
    printf("Enter your email ID: ");
    scanf("%s", email);
    printf("Select payment method (Cash/Card/Online): ");
    scanf("%s", payment);
    printf("Payment successful using %s\n", payment);
    printf("Thank you for shopping! A receipt has been sent to %s\n", email);
    for (int i = 0; i < SIZE; i++) {
        struct cart_node* ptr = cart[i];
        while (ptr) {
            struct cart_node* temp = ptr;
            ptr = ptr->next;
            free(temp);
        }
        cart[i] = NULL;
    }
    cart_total = 0.0;
    printf("Checkout complete. Cart has been cleared.\n");
    daily_report();
}

int main() {
    int choice;
    for (int i = 0; i < SIZE; i++) {
        store[i] = NULL;
        cart[i] = NULL;
    }
    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Add Product to Store\n2. Update Product\n3. View Store\n4. Add to Cart\n5. View Cart\n6. Checkout\n7. Add Coupon\n8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: add_store_item(); break;
            case 2: update_store_item(); break;
            case 3: view_store(); break;
            case 4: add_to_cart(); break;
            case 5: view_cart(); break;
            case 6: checkout(); break;
            case 7: add_coupon(); break;
            case 8:
                printf("Exiting program...\n");
                if (strlen(last_report_filename) > 0) {
                    char command[150];
                    sprintf(command, "start %s", last_report_filename);
                    system(command);
                }
                return 0;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
