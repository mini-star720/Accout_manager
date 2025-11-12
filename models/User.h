#ifndef USER_H
#define USER_H

#include <QString>
#include <QVector>
#include <memory>
#include "Record.h"
#include "Category.h"
#include "Budget.h"

class User {
public:
    User(const QString& id = QString(), const QString& name = QString());
    
    // Getter和Setter
    QString getId() const { return m_id; }
    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    QString getEmail() const { return m_email; }
    void setEmail(const QString& email) { m_email = email; }
    
    // 记录管理
    void addRecord(std::shared_ptr<Record> record);
    void removeRecord(const QString& recordId);
    std::shared_ptr<Record> getRecord(const QString& recordId) const;
    QVector<std::shared_ptr<Record>> getAllRecords() const;
    QVector<std::shared_ptr<Record>> getRecordsByDateRange(const QDate& start, const QDate& end) const;
    
    // 分类管理
    void addCategory(std::shared_ptr<Category> category);
    void removeCategory(const QString& categoryId);
    std::shared_ptr<Category> getCategory(const QString& categoryId) const;
    QVector<std::shared_ptr<Category>> getAllCategories() const;
    QVector<std::shared_ptr<Category>> getTopLevelCategories() const;
    
    // 预算管理
    void addBudget(std::shared_ptr<Budget> budget);
    void removeBudget(const QString& budgetId);
    std::shared_ptr<Budget> getBudget(const QString& categoryId) const;
    QVector<std::shared_ptr<Budget>> getAllBudgets() const;
    
    // 财务计算
    double getTotalIncome(const QDate& start, const QDate& end) const;
    double getTotalExpense(const QDate& start, const QDate& end) const;
    double getBalance(const QDate& start, const QDate& end) const;
    
private:
    QString m_id;
    QString m_name;
    QString m_email;
    
    QVector<std::shared_ptr<Record>> m_records;
    QVector<std::shared_ptr<Category>> m_categories;
    QVector<std::shared_ptr<Budget>> m_budgets;
};

#endif // USER_H