#include "db/Database.h"
#include "db/Statement.h"
#include <iostream>
#include <cmath>

int main() {
    std:: cout << "Welcome to the Expense Tracker! thats goated" << std::endl;
    std:: cout << "When promped write down your expenses and the program will keep track of them for you" << std::endl;
    try {
        Database db("expenses.db");
        db.exec("CREATE TABLE IF NOT EXISTS categories (id INTEGER PRIMARY KEY, name TEXT NOT NULL UNIQUE);");
        db.exec(R"(
            CREATE TABLE IF NOT EXISTS transactions (
            id           INTEGER PRIMARY KEY,
            date         TEXT NOT NULL DEFAULT (date('now', 'localtime')),
            amount_cents INTEGER NOT NULL,
            category_id  INTEGER NOT NULL REFERENCES categories(id),
            description  TEXT
        );
    )");
        
    // Insert with bound parameters (never build SQL by concatenating strings)
    Statement insert(db, "INSERT INTO categories (name) VALUES (?);");
        while(true){
            std::string name;
            std::cout << "Enter a category name (or 'done' to finish): ";
            std::getline(std::cin, name);
            if (name.empty()) continue;
            try{
                if (name == "done") {
                    break;
                }
                insert.bind(1, name);
                insert.step();
                std::cout << "Category '" << name << "' added successfully.\n";
            }
            catch (const std::exception& e) {
                std::cout << "Error! That category already exists. Try a different name." << std::endl;
            }
            insert.reset();
            
        }

    Statement find(db, "SELECT id FROM categories WHERE name = ?;");
    Statement addExpense(db, "INSERT INTO transactions (amount_cents, category_id, description) VALUES (?, ?, ?);");
    while(true){
            std::string categoryName;
            std::cout << "Enter a category name for the expense (or 'done' to finish): ";
            std::getline(std::cin, categoryName);
            if (categoryName.empty()) continue;
            if (categoryName == "done") {
                break;
            }
            find.bind(1, categoryName);
            if (!find.step()) {
                std::cout << "Category not found. Please enter a valid category name.\n";
                find.reset();
                continue;
            }
            int64_t categoryId = find.columnInt(0);
            find.reset();
            try{
                std::string amountStr;
                std::cout << "Enter the expense amount (in dollars): ";
                std::getline(std::cin, amountStr);
                int64_t amountCents = std::llround(std::stod(amountStr) * 100);
                if (amountCents <= 0) {
                    std::cout << "Amount must be greater than zero.\n";
                    continue;
                }
                std::string description;
                std::cout << "Enter a description for the expense: ";
                std::getline(std::cin, description);
                addExpense.bind(1, amountCents);
                addExpense.bind(2, categoryId);
                addExpense.bind(3, description);
                addExpense.step();
                std::cout << "Expense added successfully.\n";
            } catch (const std::exception& e) {
                std::cout << "Error adding expense. Not a valid input!" << "\n";
            }
            addExpense.reset();
        }


    Statement report(db,
        "SELECT t.date, c.name, t.amount_cents, t.description "
        "FROM transactions t JOIN categories c ON c.id = t.category_id "
        "ORDER BY t.id;");
    while (report.step()) {
        int64_t cents = report.columnInt(2);
        int64_t dollars = cents / 100;      // 1028 / 100 = 10  (integer division drops the .28)
        int64_t remainder = cents % 100;    // 1028 % 100 = 28  (what's left over)
        std::cout << report.columnText(0) << "  "
                << report.columnText(1) << "  "
                << "$" << dollars << ".";
        if (remainder < 10) std::cout << "0"; 
        std::cout << remainder << "  ";
        std::cout<< report.columnText(3) << "\n";
    }
        } catch (const std::exception& e) {
            std::cerr << "Database error: " << e.what() << std::endl;
            return 1;
        }
        return 0;
}