#include <stdio.h>
#include <string.h>


typedef struct User
{
    int id;
    char name[25]; //to simplify things name contains only the first name entered by the user i.e one word name
    float amt;
} User;

typedef struct Expense
{
    int id;
    float amt;
    User bearer;
    User payers[4];
    int nPayers;
} Expense;

typedef struct Payer
{
    User user;
    float amt;
} Payer;

typedef struct Payers
{
    Payer payers[1000];
    int nPayers;
    float total;
} Payers;


User users[1000];
int nUsers = 0;
Expense expenses[1000];
int nExpenses = 0;


int ValidUserID(int UserID)
{
    int flag = 0;

    User *user = users;
    int i = 0;

    while (i < nUsers && flag == 0)
    {
        if (user->id == UserID) flag = 1;
        user++;
        i++;
    }

    return flag;
}


int ValidExpenseID(int ExpenseID)
{
    int flag = 0;

    Expense *expense = expenses;
    int i = 0;

    while (i < nExpenses && flag == 0)
    {
        if (expense->id == ExpenseID) flag = 1;
        expense++;
        i++;
    }

    return flag;
}


User * RetrieveUser(int UserID)
{
    User *user;
    for (user = users; user->id != UserID; user++);
    return user;
}


Expense * RetrieveExpense(int ExpenseID)
{
    Expense *expense;
    for (expense = expenses; expense->id != ExpenseID; expense++);
    return expense;
}


void AddUser()
{
    int id;
    char name[25];
    int flag = 0;

    do
    {
        printf("UserID: ");
        scanf("%d", &id);

        if (id > 0 && id < 1001) 
            flag = !ValidUserID(id);

        if (flag == 0) printf("Invalid UserID\n");
    } 
    while (flag == 0);

    printf("Username: ");
    scanf("%s", name);

    users[nUsers].id = id;
    strcpy(users[nUsers].name, name);
    users[nUsers].amt = 0;
    User temp;
    
    for (int i = nUsers; users[i-1].id > id; i--)
    {
        temp = users[i-1];
        users[i-1] = users[i];
        users[i] = temp;
    }
    nUsers++;

    printf("User added successfully.\n");
}


void printAllUsers()
{
    for (int i = 0; i < nUsers; i++)
    {
        printf("%d ", users[i].id);
        printf("%s ", users[i].name);
        printf("%.2f\n", users[i].amt);
    }
}


void AddExpense()
{
    int id;
    float amt;
    int bearerID;
    int nPayers;
    int flag = 0;
    Expense *expense = &expenses[nExpenses];

    do
    {
        printf("ExpenseID: ");
        scanf("%d", &id);

        if (id > 0 && id < 1001) 
            flag = !ValidExpenseID(id);

        if (flag == 0) printf("Invalid ExpenseID\n");
    } 
    while (flag == 0);

    expense->id = id;

    do 
    {
        printf("Expense amount: ");
        scanf("%f", &amt);

        if (amt <= 0) printf("Invalid Amount\n");
    } 
    while (amt <= 0);

    expense->amt = amt;

    do
    {
        printf("UserID of Expense bearer: ");
        scanf("%d", &bearerID);

        flag = ValidUserID(bearerID);
        
        if (flag == 0) printf("Invalid UserID\n");
    }
    while (flag == 0);

    User *bearer = RetrieveUser(bearerID);
    expense->bearer = *bearer;

    do
    {
        printf("Number of Users contributing to the Expense (1-4): ");
        scanf("%d", &nPayers);

        flag = 1;

        if (nPayers < 1 || nPayers > 4) 
        {
            flag = 0;
            printf("Invalid number of Users\n");
        }
    }
    while (flag == 0);

    expense->nPayers = nPayers;

    for (int i = 0; i < nPayers; i++)
    {
        int payerID;
        do
        {
            printf("ID of User %d sharing the Expense: ", i+1);
            scanf("%d", &payerID);

            flag = ValidUserID(payerID) && (bearerID != payerID);
            
            if (flag == 0) 
                printf("Invalid UserID\n");
            else
                (expense->payers)[i] = *RetrieveUser(payerID);
        }
        while (flag == 0);
    }

    bearer->amt += amt;

    Expense temp;
    
    for (int i = nExpenses; expenses[i-1].id > id; i--)
    {
        temp = expenses[i-1];
        expenses[i-1] = expenses[i];
        expenses[i] = temp;
    }
    nExpenses++;

    printf("Expense added successfully.\n");
}


void printAllExpenses()
{
    Expense *expense;
    
    for (int i = 0; i < nExpenses; i++)
    {
        expense = &expenses[i];
        printf("%d\n", expense->id);
        printf("%.2f\n", expense->amt);
        printf("%d\n", expense->bearer.id);
        
        for (int i = 0; i < expense->nPayers; i++)
        {
            printf("%d ", (expense->payers)[i].id);
        }
        printf("\n\n");
    }
}


Payers AmountOwed(int UserID)
{
    Payers allPayers;
    allPayers.nPayers = 0;
    allPayers.total = 0;

    for (int i = 0; i < nExpenses; i++)
    {
        Expense *expense = &expenses[i];

        if (expense->bearer.id == UserID)
        {
            float amtPerUser = expense->amt / expense->nPayers;

            for (int j = 0; j < expense->nPayers; j++)
            {
                int payerID = (expense->payers)[j].id;

                int userAlreadyPayer = 0;
                for (int k = 0; k < allPayers.nPayers; k++)
                {
                    Payer payer = (allPayers.payers)[k];
                    if (payer.user.id == payerID)
                    {
                        userAlreadyPayer = 1;
                        payer.amt += amtPerUser;
                    }
                }

                if (userAlreadyPayer == 0)
                {
                    allPayers.payers[allPayers.nPayers].user = *RetrieveUser(payerID);
                    allPayers.payers[allPayers.nPayers].amt = amtPerUser;
                    allPayers.nPayers++;
                }
            }

            allPayers.total += expense->amt;
        }
    }
    return allPayers;
}


Payers AmountToPay(int UserID)
{
    Payers allPayers;
    allPayers.nPayers = 0;
    allPayers.total = 0;

    for (int i = 0; i < nExpenses; i++)
    {
        Expense *expense = &expenses[i];

        if (expense->bearer.id != UserID)
        {
            int userIsPayer = 0;
            for (int j = 0; j < expense->nPayers && userIsPayer == 0; j++)
            {
                if ((expense->payers)[j].id == UserID)
                    userIsPayer = 1;
            }

            if (userIsPayer)
            {
                float amtPerUser = expense->amt / expense->nPayers;
                allPayers.total += amtPerUser;

                User bearer = expense->bearer;

                int userAlreadyPayer = 0;
                for (int k = 0; k < allPayers.nPayers; k++)
                {
                    Payer payer = (allPayers.payers)[k];
                    if (payer.user.id == bearer.id)
                    {
                        userAlreadyPayer = 1;
                        payer.amt += amtPerUser;
                    }
                }

                if (userAlreadyPayer == 0)
                {
                    allPayers.payers[allPayers.nPayers].user = *RetrieveUser(bearer.id);
                    allPayers.payers[allPayers.nPayers].amt = amtPerUser;
                    allPayers.nPayers++;
                }
            }
        }
    }
    return allPayers;
}


void UserBalances()
{
    User *user;
    Payers Payees;
    Payers Owers;
    float balance;

    for (int i = 0; i < nUsers; i++)
    {
        user = &(users[i]);
        Owers = AmountOwed(user->id);
        Payees = AmountToPay(user->id);

        balance = user->amt + Payees.total - Owers.total;

        printf("%d %s\n", user->id, user->name);
        printf("Final total amount spent: %.2f\n", balance);
        printf("Payees:\n");
        for (int i = 0; i < Payees.nPayers; i++)
        {
            User user = Payees.payers[i].user;
            printf("%d %s %.2f\n", user.id, user.name, ((Payees.payers)[i]).amt);
        }
        printf("Owers:\n");
        for (int i = 0; i < Owers.nPayers; i++)
        {
            User user = Owers.payers[i].user;
            printf("%d %s %.2f\n", user.id, user.name, ((Owers.payers)[i]).amt);
        }
        printf("\n");
    }
}


void DeleteUser(int UserID)
{
    Payers Owers = AmountOwed(UserID);
    Payers Payees = AmountToPay(UserID);

    int balancesSettled = (Owers.nPayers == 0) && (Payees.nPayers == 0);

    if (balancesSettled)
    {
        int userFound = 0;

        for (int i = 0; i < nUsers-1; i++)
        {
            if (users[i].id == UserID)
                userFound = 1;

            if (userFound)
                users[i] = users[i+1];
        }
        nUsers -= 1;
        printf("User deleted successfully.\n");
    }
    else 
    {
        printf("Cannot delete this User. Following have not been settled:\n");
        printf("Payees:\n");
        for (int i = 0; i < Payees.nPayers; i++)
        {
            User user = Payees.payers[i].user;
            printf("%d %s %.2f\n", user.id, user.name, ((Payees.payers)[i]).amt);
        }
        printf("Owers:\n");
        for (int i = 0; i < Owers.nPayers; i++)
        {
            User user = Owers.payers[i].user;
            printf("%d %s %.2f\n", user.id, user.name, ((Owers.payers)[i]).amt);
        }
        printf("Please settle expenses before deleting.\n");
    }
}


int DeleteExpense(int UserID, int ExpenseID)
{
    int validIDs = ValidExpenseID(ExpenseID) && ValidUserID(UserID);
    int expenseDeleted = 0;

    if (validIDs)
    {
        Expense *reqExpense = RetrieveExpense(ExpenseID);
        if (reqExpense->bearer.id == UserID)
        {
            User *bearer = RetrieveUser(UserID);
            bearer->amt -= reqExpense->amt;

            int expenseFound = 0;

            for (int i = 0; i < nExpenses-1; i++)
            {
                if (expenses[i].id == reqExpense->id) 
                    expenseFound = 1;

                if (expenseFound)
                    expenses[i] = expenses[i+1];
            }
            nExpenses -= 1;
            expenseDeleted = 1;
        }
    }
    return expenseDeleted;
}


void SettleOwe(int payerID, int recieverID)
{
    int nReqExpenses = 0;

    for (int i = 0; i < nExpenses; i++)
    {
        Expense expense = expenses[i];
        if (expense.bearer.id == recieverID)
        {
            int userIsPayer = 0;
            for (int j = 0; j < expense.nPayers && userIsPayer == 0; j++)
            {
                if (expense.payers[j].id == payerID) userIsPayer = 1;
            }

            if (userIsPayer)
            {
                if (nReqExpenses == 0) printf("Expenses to settle:\n");

                nReqExpenses += 1;

                printf("%d %.2f\n", expense.id, expense.amt / expense.nPayers);
            }
        }
    }
    
    if (nReqExpenses > 0)
    {
        int expenseID;
        int flag = 0;
        Expense *expense;

        do
        {
            printf("Enter Expense ID to settle: ");
            scanf("%d", &expenseID);

            if (expenseID > 0 && expenseID < 1001)
            {
                flag = ValidExpenseID(expenseID);
            }
        } while (flag == 0);

        expense = RetrieveExpense(expenseID);

        float amtPP = expense->amt / expense->nPayers;

        User *reciever = RetrieveUser(recieverID);
        User *payer = RetrieveUser(payerID);

        reciever->amt -= amtPP;
        payer->amt += amtPP;

        expense->amt -= amtPP;

        int expenseDeleted = 0;

        if (expense->nPayers-1 == 0)
        {
            expenseDeleted = DeleteExpense(recieverID, expense->id);
            if (expenseDeleted) printf("Expense settled and deleted successfully.\n");
            else printf("Something went wrong.\n");
        }
        else if (!expenseDeleted)
        {
            expense->bearer = *reciever;
            int flag = 0;
            for (int j = 0; j < expense->nPayers-1; j++)
            {
                if (expense->payers[j].id == payerID) flag = 1;
                if (flag == 1)
                {
                    expense->payers[j] = expense->payers[j+1];
                }
            }
            expense->nPayers -= 1;
            printf("Expense settled successfully.\n");
        }
    }
    else printf("No expenses to settle.\n");
}


void main()
{
    int option = 0;
    do
    {
        printf("1.Add User\n11.Print all Users\n2.Add Expense\n21.Print all Expenses\n3.Amount owed to a user\n4.Amount owed by a user\n5.Display all user balances\n6.Settle owe\n7.Delete User\n8.Delete Expense\n9.Quit\n> ");
        scanf("%d", &option);
        if (option == 1)
        {
            if (nUsers < 1000)
            {
                AddUser();
            }
            else
                printf("User limit exceeded\n");
            printf("\n");
        }
        else if (option == 11)
        {
            printAllUsers();
        }
        else if (option == 2)
        {
            if (nExpenses < 1000)
            {
                AddExpense();
            }
            else
                printf("Expense limit exceeded\n");
            printf("\n");
        }
        else if (option == 21)
        {
            printAllExpenses();
        }
        else if (option == 3)
        {
            int UserID;
            int flag = 0;

            do
            {
                printf("UserID: ");
                scanf("%d", &UserID);

                if (UserID > 0 && UserID < 1001) 
                    flag = ValidUserID(UserID);

                if (flag == 0) printf("Invalid UserID\n");
            } 
            while (flag == 0);

            Payers payersInfo = AmountOwed(UserID);

            printf("Total Owed to UserID %d: %.2f\n\n", UserID, payersInfo.total);

            for (int i = 0; i < payersInfo.nPayers; i++)
            {
                User user = payersInfo.payers[i].user;
                printf("%d %s %.2f\n", user.id, user.name, ((payersInfo.payers)[i]).amt);
            }
        }
        else if (option == 4)
        {
            int UserID;
            int flag = 0;

            do
            {
                printf("UserID: ");
                scanf("%d", &UserID);

                if (UserID > 0 && UserID < 1001) 
                    flag = ValidUserID(UserID);

                if (flag == 0) printf("Invalid UserID\n");
            } 
            while (flag == 0);

            Payers payersInfo = AmountToPay(UserID);

            printf("Total Amount UserID %d has to pay: %.2f\n\n", UserID, payersInfo.total);

            for (int i = 0; i < payersInfo.nPayers; i++)
            {
                User user = payersInfo.payers[i].user;
                printf("%d %s %.2f\n", user.id, user.name, ((payersInfo.payers)[i]).amt);
            }
        }
        else if (option == 5)
        {
            UserBalances();
        }
        else if (option == 6)
        {
            int payerID;
            int recieverID;

            do
            {
                printf("payerID: ");
                scanf("%d", &payerID);

                if (!(payerID > 0 && payerID < 1001)) printf("Invalid UserID\n");
            } 
            while (!(payerID > 0 && payerID < 1001));

            do
            {
                printf("recieverID: ");
                scanf("%d", &recieverID);

                if (!(recieverID > 0 && recieverID < 1001)) printf("Invalid UserID\n");
            } 
            while (!(recieverID > 0 && recieverID < 1001));

            SettleOwe(payerID, recieverID);
        }
        else if (option == 7)
        {
            int UserID;

            do
            {
                printf("UserID: ");
                scanf("%d", &UserID);

                if (!(UserID > 0 && UserID < 1001)) printf("Invalid UserID\n");
            } 
            while (!(UserID > 0 && UserID < 1001));

            DeleteUser(UserID);
        }
        else if (option == 8)
        {
            int ExpenseID;
            int UserID;

            do
            {
                printf("ExpenseID: ");
                scanf("%d", &ExpenseID);

                if (!(ExpenseID > 0 && ExpenseID < 1001)) printf("Invalid ExpenseID\n");
            } 
            while (!(ExpenseID > 0 && ExpenseID < 1001));

            do
            {
                printf("UserID: ");
                scanf("%d", &UserID);

                if (!(UserID > 0 && UserID < 1001)) printf("Invalid UserID\n");
            } 
            while (!(UserID > 0 && UserID < 1001));

            int expenseDeleted = DeleteExpense(UserID, ExpenseID);

            if (expenseDeleted) printf("Expense deleted successfully.\n");
            else printf("Something went wrong.\n");
        }
        else if (option != 9)
            printf("Enter valid input, Try again\n\n");
        printf("\n");
    } while (option != 9);
}