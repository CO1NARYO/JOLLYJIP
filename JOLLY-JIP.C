#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#define filename "sales_data.txt"
#define max_customers 100
#define max_items 50


typedef struct{
    char code[10];
    char description[50];
    float unit_price;
} MenuItem;

typedef struct{
    MenuItem item;
    int quantity; 
    float amount;
} Order;

typedef struct {
    char date[20];
    int table_no;
    Order orders[10];
    // int numOfOrders;
    float total_bill;
    float discount;
    float net_bill;
    float tendered;
    float change;
    int num_orders;
} Billing;

MenuItem menu[max_items];
int menu_size = 0; //Tracks the number of items stored in the menu[].

void addItem(){
    printf("Enter item code: ");
    scanf("%s", menu[menu_size].code);
    printf("Enter item description: ");
    scanf(" %[^\n]", menu[menu_size].description);
    printf("Enter unit price: ");
    scanf("%f", &menu[menu_size].unit_price);
    menu_size++;
    printf("Item added successfully.\n");
}

void displayMenu(){
    printf("\n%-10s %-30s %-10s\n", "CODE", "DESCRIPTION", "PRICE");
    
    for(int i = 0; i < menu_size; i++){
        printf("\n%-10s %-30s %.2f\n", menu[i].code, menu[i].description, menu[i].unit_price);
    }
}

float compute_discount(float total, int if_senior, int if_pwd) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    float discount = 0;

    if (if_senior) return total * 0.20;
    if (if_pwd) return total * 0.05;

    if ((tm.tm_mon + 1 == 5 || tm.tm_mon + 1 == 6) && total >= 500 && total <= 2000)
        discount = total * 0.02;

    return discount;
}


void billing_module(){
    Billing bill;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(bill.date, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("Enter table number: ");
    scanf("%d", &bill.table_no);

    bill.num_orders = 0;
    char choice;

    do{
        displayMenu();
        printf("Enter item code to order: ");
        char code[10];
        scanf("%s", code);

        int found = -1;
        for(int i = 0; i < menu_size; i++){
            if(strcmp(menu[i].code, code) == 0){
                found = i; 
                break;
            }
        }
        if(found == -1){
            printf("Item not found.\n");
            continue;
        }

        bill.orders[bill.num_orders].item = menu[found];
        printf("Enter quantity: ");
        scanf("%d", &bill.orders[bill.num_orders].quantity);
       
        bill.orders[bill.num_orders].amount = bill.orders[bill.num_orders].item.unit_price * bill.orders[bill.num_orders].quantity;

        bill.num_orders++;

        printf("Add another item? (y/n): ");
        scanf(" %c", &choice);

    } while (choice == 'y' || choice == 'Y');

    bill.total_bill = 0;
    for(int i = 0; i < bill.num_orders; i++){
        bill.total_bill += bill.orders[i].amount;
    }

    int if_senior = 0, if_pwd = 0;

    printf("Is customer a senior citizen (1-yes/0-no)? ");
    scanf("%d", &if_senior);
    if(!if_senior){
        printf("Is customer a PWD (1-yes/0-no)? "); 
        scanf("%d", &if_pwd);
    }

    bill.discount = compute_discount(bill.total_bill, if_senior, if_pwd);
    bill.net_bill = bill.total_bill - bill.discount;

    do {
        printf("Enter amount tendered: ");
        scanf("%f", &bill.tendered);
        if(bill.tendered < bill.net_bill){
            printf("Insufficient amount. Please enter the correct amount.\n");
        }
    }while(bill.tendered < bill.net_bill);

    bill.change = bill.tendered - bill.net_bill;

    //Display and print's the receipt 
    printf("\n---RECEIPT---\n");
    printf("DATE: %s\tTABLE NO.: %d\n", bill.date, bill.table_no);
    printf("%-10s %-20s %-10s %-10s %-10s\n", "CODE", "DESCRIPTION", "UNIT PRICE", "QUANTITY", "AMOUNT");
    
    for(int i = 0; i < bill.num_orders; i++){
        printf("%-10s %-20s %-10d %-10.2f %10.2f\n", bill.orders[i].item.code, bill.orders[i].item.description, bill.orders[i].quantity, bill.orders[i].item.unit_price, bill.orders[i].amount);
    }

    printf("TOTAL BILL: %.2f\n", bill.total_bill);
    printf("DISCOUNT: %.2f\n", bill.discount);
    printf("NET BILL: %.2f\n", bill.net_bill);
    printf("AMOUNT TENDERED: %.2f\n", bill.tendered);
    printf("CHANGE: %.2f\n", bill.change);

    //Append to file 
    FILE *fp = fopen(filename, "a");
    if(fp){
        fprintf(fp, "%s %.2f %.2f %.2f\n", bill.date, bill.total_bill, bill.discount, bill.net_bill);
        fclose(fp);
    }    
}

void generate_daily_sales() {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No sales data found.\n");
        return;
    }

    float total_sales = 0, total_discount = 0, total_net = 0;
    char date[20];
    float bill, discount, net;

    printf("\n--- DAILY SALES REPORT ---\n");
    while (fscanf(fp, "%s %f %f %f", date, &bill, &discount, &net) != EOF) {
        total_sales += bill;
        total_discount += discount;
        total_net += net;
    }
    fclose(fp);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("As of %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Total billing for the day: %.2f\n", total_sales);
    printf("Total Discount provided for the day: %.2f\n", total_discount);
    printf("Total Net Billing for the day: %.2f\n", total_net);
}

int main() {
    int choice;
    do {
        printf("\nJOLLY - JIP RESTAURANT\n");
        printf("1. Billing Sub-Module\n");
        printf("2. Add New Menu Item\n");
        printf("3. Display Menu\n");
        printf("4. Generate Daily Total Sales\n");
        printf("5. Exit\n");
        printf("Choose Transaction: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                billing_module();
                break;
            case 2:
                addItem();
                break;
            case 3:
                displayMenu();
                break;
            case 4:
                generate_daily_sales();
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 5);

    return 0;
}


