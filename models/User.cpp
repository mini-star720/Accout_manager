#include "User.h"
#include <algorithm>

User::User(const QString& id, const QString& name) 
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_name(name) {
}

// 记录管理
void User::addRecord(std::shared_ptr<Record> record) {
    if (record && !record->getId().isEmpty()) {
        m_records.append(record);
    }
}

void User::removeRecord(const QString& recordId) {
    auto it = std::find_if(m_records.begin(), m_records.end(), 
        [&recordId](const std::shared_ptr<Record>& record) {
            return record->getId() == recordId;
        });
    
    if (it != m_records.end()) {
        (*it)->markAsDeleted();
    }
}

std::shared_ptr<Record> User::getRecord(const QString& recordId) const {
    auto it = std::find_if(m_records.begin(), m_records.end(),
        [&recordId](const std::shared_ptr<Record>& record) {
            return record->getId() == recordId;
        });
    
    return (it != m_records.end()) ? *it : nullptr;
}

QVector<std::shared_ptr<Record>> User::getAllRecords() const {
    return m_records;
}

QVector<std::shared_ptr<Record>> User::getRecordsByDateRange(const QDate& start, const QDate& end) const {
    QVector<std::shared_ptr<Record>> result;
    
    for (const auto& record : m_records) {
        QDate recordDate = record->getDateTime().date();
        if (recordDate >= start && recordDate <= end && !record->isDeleted()) {
            result.append(record);
        }
    }
    
    return result;
}

// 分类管理
void User::addCategory(std::shared_ptr<Category> category) {
    if (category && !category->getId().isEmpty()) {
        m_categories.append(category);
    }
}

void User::removeCategory(const QString& categoryId) {
    m_categories.erase(
        std::remove_if(m_categories.begin(), m_categories.end(),
            [&categoryId](const std::shared_ptr<Category>& category) {
                return category->getId() == categoryId;
            }),
        m_categories.end()
    );
}

std::shared_ptr<Category> User::getCategory(const QString& categoryId) const {
    auto it = std::find_if(m_categories.begin(), m_categories.end(),
        [&categoryId](const std::shared_ptr<Category>& category) {
            return category->getId() == categoryId;
        });
    
    return (it != m_categories.end()) ? *it : nullptr;
}

QVector<std::shared_ptr<Category>> User::getAllCategories() const {
    return m_categories;
}

QVector<std::shared_ptr<Category>> User::getTopLevelCategories() const {
    QVector<std::shared_ptr<Category>> result;
    
    for (const auto& category : m_categories) {
        if (category->isTopLevel()) {
            result.append(category);
        }
    }
    
    return result;
}

// 预算管理
void User::addBudget(std::shared_ptr<Budget> budget) {
    if (budget && !budget->getId().isEmpty()) {
        m_budgets.append(budget);
    }
}

void User::removeBudget(const QString& budgetId) {
    m_budgets.erase(
        std::remove_if(m_budgets.begin(), m_budgets.end(),
            [&budgetId](const std::shared_ptr<Budget>& budget) {
                return budget->getId() == budgetId;
            }),
        m_budgets.end()
    );
}

std::shared_ptr<Budget> User::getBudget(const QString& categoryId) const {
    auto it = std::find_if(m_budgets.begin(), m_budgets.end(),
        [&categoryId](const std::shared_ptr<Budget>& budget) {
            return budget->getCategoryId() == categoryId;
        });
    
    return (it != m_budgets.end()) ? *it : nullptr;
}

QVector<std::shared_ptr<Budget>> User::getAllBudgets() const {
    return m_budgets;
}

// 财务计算
double User::getTotalIncome(const QDate& start, const QDate& end) const {
    double total = 0.0;
    auto records = getRecordsByDateRange(start, end);
    
    for (const auto& record : records) {
        if (record->isIncome()) {
            total += record->getAmount();
        }
    }
    
    return total;
}

double User::getTotalExpense(const QDate& start, const QDate& end) const {
    double total = 0.0;
    auto records = getRecordsByDateRange(start, end);
    
    for (const auto& record : records) {
        if (record->isExpense()) {
            total += record->getAmount();
        }
    }
    
    return total;
}

double User::getBalance(const QDate& start, const QDate& end) const {
    return getTotalIncome(start, end) - getTotalExpense(start, end);
}