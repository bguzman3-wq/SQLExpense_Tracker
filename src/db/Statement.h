#pragma once
#include "Database.h"
#include <sqlite3.h>
#include <cstdint>
#include <string>

class Statement {
public:
    Statement(Database& db, const std::string& sql);
    ~Statement();

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&& other) noexcept;
    Statement& operator=(Statement&& other) noexcept;

    // Bind parameters (indexes start at 1, matching SQLite)
    void bind(int index, int64_t value);
    void bind(int index, const std::string& value);
    void bindNull(int index);

    // Returns true if a row is available, false when done
    bool step();
    void reset();

    // Read columns from the current row (indexes start at 0)
    int64_t columnInt(int col) const;
    std::string columnText(int col) const;

private:
    sqlite3* db_ = nullptr;      // not owned, only used for error messages
    sqlite3_stmt* stmt_ = nullptr;
};