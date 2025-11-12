#include "Budget.h"

Budget::Budget(const QString& id) 
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_totalAmount(0.0)
    , m_usedAmount(0.0)
    , m_alertPercent(0.8) // 默认80%警告阈值
    , m_period(Period::Monthly)
    , m_status(Status::Created) {
}

double Budget::getUsagePercentage() const {
    if (m_totalAmount <= 0) {
        return 0.0;
    }
    return m_usedAmount / m_totalAmount;
}

void Budget::addExpense(double amount) {
    if (amount > 0) {
        m_usedAmount += amount;
        checkAndUpdateStatus();
    }
}

void Budget::removeExpense(double amount) {
    if (amount > 0 && m_usedAmount >= amount) {
        m_usedAmount -= amount;
        checkAndUpdateStatus();
    }
}

void Budget::resetForNewPeriod() {
    m_usedAmount = 0.0;
    m_status = Status::Active;
    
    // 根据周期类型设置新的日期范围
    QDate currentDate = QDate::currentDate();
    m_startDate = currentDate;
    
    switch (m_period) {
        case Period::Weekly:
            m_endDate = currentDate.addDays(6);
            break;
        case Period::Monthly:
            m_endDate = QDate(currentDate.year(), currentDate.month(), 
                            currentDate.daysInMonth());
            break;
        case Period::Yearly:
            m_endDate = QDate(currentDate.year(), 12, 31);
            break;
    }
}

void Budget::checkAndUpdateStatus() {
    double usagePercent = getUsagePercentage();
    
    if (usagePercent >= 1.0) {
        m_status = Status::OverBudget;
    } else if (usagePercent >= m_alertPercent) {
        m_status = Status::Warning;
    } else if (usagePercent > 0) {
        m_status = Status::Active;
    } else {
        m_status = Status::Created;
    }
}