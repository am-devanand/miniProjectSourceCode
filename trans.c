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

// ---------- Function Prototypes ----------
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listAccounts(FILE *fPtr);
void initializeFile(FILE *fPtr);
void validateFile(FILE *fPtr);
int seekToRecord(FILE *fPtr, unsigned int account);

// ---------- MAIN ----------
int main() {
    FILE *cfPtr;

    cfPtr = fopen(FILE_NAME, "rb+");

    if (cfPtr == NULL) {
        cfPtr = fopen(FILE_NAME, "wb+");
        if (cfPtr == NULL) {
            perror("File creation failed");
            exit(1);
        }
        initializeFile(cfPtr);
    } else {
        validateFile(cfPtr);
    }

    unsigned int choice;
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

// ---------- FILE INITIALIZATION ----------
void initializeFile(FILE *fPtr) {
    struct clientData blank = {0, "", "", 0.0};
    rewind(fPtr);

    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        fwrite(&blank, sizeof(blank), 1, fPtr);
    }
    fflush(fPtr);
}

// ---------- FILE VALIDATION ----------
void validateFile(FILE *fPtr) {
    fseek(fPtr, 0, SEEK_END);
    long size = ftell(fPtr);

    if (size != MAX_ACCOUNTS * sizeof(struct clientData)) {
        printf("File corrupted. Reinitializing...\n");
        initializeFile(fPtr);
    }
}

// ---------- SEEK HELPER ----------
int seekToRecord(FILE *fPtr, unsigned int account) {
    if (account < 1 || account > MAX_ACCOUNTS) {
        printf("Invalid account number\n");
        return 0;
    }

    if (fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET) != 0) {
        perror("Seek failed");
        return 0;
    }
    return 1;
}

// ---------- CREATE TEXT FILE ----------
void textFile(FILE *readPtr) {
    FILE *writePtr = fopen("accounts.txt", "w");
    if (!writePtr) {
        perror("Text file error");
        return;
    }

    struct clientData client;
    rewind(readPtr);

    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(client), 1, readPtr) == 1) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    printf("✔ accounts.txt generated successfully\n");
}

// ---------- ADD RECORD ----------
void newRecord(FILE *fPtr) {
    struct clientData client = {0};
    unsigned int account;

    printf("Enter account number (1-100): ");
    if (scanf("%u", &account) != 1) return;

    if (!seekToRecord(fPtr, account)) return;

    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account already exists\n");
        return;
    }

    printf("Enter lastname firstname balance: ");
    if (scanf("%14s %9s %lf",
              client.lastName,
              client.firstName,
              &client.balance) != 3) return;

    client.acctNum = account;

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("✔ Account created successfully\n");
}

// ---------- UPDATE ----------
void updateRecord(FILE *fPtr) {
    struct clientData client;
    unsigned int account;
    double transaction;

    printf("Enter account: ");
    if (scanf("%u", &account) != 1) return;

    if (!seekToRecord(fPtr, account)) return;

    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account not found\n");
        return;
    }

    printf("Current balance: %.2f\n", client.balance);
    printf("Enter transaction (+/-): ");
    if (scanf("%lf", &transaction) != 1) return;

    client.balance += transaction;

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("✔ Updated balance: %.2f\n", client.balance);
}

// ---------- DELETE ----------
void deleteRecord(FILE *fPtr) {
    struct clientData client, blank = {0};
    unsigned int account;

    printf("Enter account to delete: ");
    if (scanf("%u", &account) != 1) return;

    if (!seekToRecord(fPtr, account)) return;

    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account does not exist\n");
        return;
    }

    fseek(fPtr, -sizeof(client), SEEK_CUR);
    fwrite(&blank, sizeof(blank), 1, fPtr);
    fflush(fPtr);

    printf("✔ Account deleted\n");
}

// ---------- LIST ----------
void listAccounts(FILE *fPtr) {
    struct clientData client;

    rewind(fPtr);
    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(client), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// ---------- MENU ----------
unsigned int enterChoice(void) {
    unsigned int choice;

    printf("\n1 - Generate text file\n"
           "2 - Update account\n"
           "3 - Add account\n"
           "4 - Delete account\n"
           "5 - List accounts\n"
           "6 - Exit\nChoice: ");

    scanf("%u", &choice);
    return choice;
}
