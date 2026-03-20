#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME "credit.dat"
#define MAX_ACCOUNTS 100

struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// Function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listAccounts(FILE *fPtr);
void initializeFile(FILE *fPtr);

int main() {
    FILE *cfPtr;
    unsigned int choice;

    // Open or create file
    if ((cfPtr = fopen(FILE_NAME, "rb+")) == NULL) {
        cfPtr = fopen(FILE_NAME, "wb+");
        if (cfPtr == NULL) {
            printf("File could not be created.\n");
            exit(1);
        }
        initializeFile(cfPtr);
    }

    while ((choice = enterChoice()) != 6) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            case 5: listAccounts(cfPtr); break;
            default: printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

// Initialize file with empty records
void initializeFile(FILE *fPtr) {
    struct clientData blank = {0, "", "", 0.0};
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    }
}

// Create text file
void textFile(FILE *readPtr) {
    FILE *writePtr = fopen("accounts.txt", "w");
    struct clientData client;

    if (!writePtr) {
        printf("Error creating text file\n");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(client), 1, readPtr) == 1) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    printf("accounts.txt generated successfully.\n");
}

// Update record
void updateRecord(FILE *fPtr) {
    struct clientData client;
    unsigned int account;
    double transaction;

    printf("Enter account (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) return;

    fseek(fPtr, (account - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account does not exist\n");
        return;
    }

    printf("Current balance: %.2f\n", client.balance);
    printf("Enter amount (+credit / -debit): ");
    scanf("%lf", &transaction);

    client.balance += transaction;

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);

    printf("Updated balance: %.2f\n", client.balance);
}

// Delete record
void deleteRecord(FILE *fPtr) {
    struct clientData client, blank = {0, "", "", 0};
    unsigned int account;

    printf("Enter account to delete (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) return;

    fseek(fPtr, (account - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account does not exist\n");
        return;
    }

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&blank, sizeof(client), 1, fPtr);

    printf("Account deleted.\n");
}

// Add new record
void newRecord(FILE *fPtr) {
    struct clientData client;
    unsigned int account;

    printf("Enter account number (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) return;

    fseek(fPtr, (account - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account already exists\n");
        return;
    }

    printf("Enter lastname firstname balance: ");
    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = account;

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);

    printf("Account added.\n");
}

// List all accounts (NEW FEATURE)
void listAccounts(FILE *fPtr) {
    struct clientData client;

    rewind(fPtr);
    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(client), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// Menu
unsigned int enterChoice(void) {
    unsigned int choice;

    printf("\n1 - Generate text file\n"
           "2 - Update account\n"
           "3 - Add account\n"
           "4 - Delete account\n"
           "5 - List accounts\n"
           "6 - Exit\n"
           "Choice: ");

    scanf("%u", &choice);
    return choice;
}
