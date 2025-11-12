#ifndef BUDGET_H
#define BUDGET_H

#include <QString>
#include <QDate>
#include <QUuid>

class Budget {
public:
    enum class Status {
        Created,
        Active,
        Warning,
        OverBudget,
        Normal
    };
    
    enum class Period {
        Monthly,
        Weekly,
        Yearly
    };
    
    Budget(const QString& id = QUuid::createUuid().toString());
    
    // Getter和Setter
    QString getId() const { return m_id; }
    
    QString getCategoryId() const { return m_categoryId; }
    void setCategoryId(const QString& categoryId) { m_categoryId = categoryId; }
    
    double getTotalAmount() const { return m_totalAmount; }
    void setTotalAmount(double amount) { m_totalAmount = amount; }
    
    double getUsedAmount() const { return m_usedAmount; }
    void setUsedAmount(double amount) { m_usedAmount = amount; }
    
    double getAlertPercent() const { return m_alertPercent; }
    void setAlertPercent(double percent) { m_alertPercent = percent; }
    
    Period getPeriod() const { return m_period; }
    void setPeriod(Period period) { m_period = period; }
    
    QDate getStartDate() const { return m_startDate; }
    void setStartDate(const QDate& date) { m_startDate = date; }
    
    QDate getEndDate() const { return m_endDate; }
    void setEndDate(const QDate& date) { m_endDate = date; }
    
    Status getStatus() const { return m_status; }
    void setStatus(Status status) { m_status = status; }
    
    // 状态管理
    double getUsagePercentage() const;
    bool isInWarning() const { return m_status == Status::Warning; }
    bool isOverBudget() const { return m_status == Status::OverBudget; }
    
    // 预算操作
    void addExpense(double amount);
    void removeExpense(double amount);
    void resetForNewPeriod();
    
    // 状态检查
    void checkAndUpdateStatus();
    
private:
    QString m_id;
    QString m_categoryId;
    double m_totalAmount;
    double m_usedAmount;
    double m_alertPercent; // 警告阈值，如0.8表示80%
    
    Period m_period;
    QDate m_startDate;
    QDate m_endDate;
    
    Status m_status;
};

#endif // BUDGET_H