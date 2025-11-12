#ifndef RECORD_H
#define RECORD_H

#include <QString>
#include <QDateTime>
#include <QUuid>

class Record {
public:
    enum class Type {
        Income,
        Expense
    };
    
    enum class Status {
        Draft,
        Saved,
        Modified,
        Deleted,
        Restored
    };
    
    Record(const QString& id = QUuid::createUuid().toString());
    
    // Getter和Setter
    QString getId() const { return m_id; }
    
    Type getType() const { return m_type; }
    void setType(Type type) { m_type = type; }
    
    double getAmount() const { return m_amount; }
    void setAmount(double amount) { m_amount = amount; }
    
    QString getCategoryId() const { return m_categoryId; }
    void setCategoryId(const QString& categoryId) { m_categoryId = categoryId; }
    
    QDateTime getDateTime() const { return m_dateTime; }
    void setDateTime(const QDateTime& dateTime) { m_dateTime = dateTime; }
    
    QString getNote() const { return m_note; }
    void setNote(const QString& note) { m_note = note; }
    
    Status getStatus() const { return m_status; }
    void setStatus(Status status) { m_status = status; }
    
    bool isDeleted() const { return m_status == Status::Deleted; }
    void markAsDeleted() { m_status = Status::Deleted; }
    
    QDateTime getCreatedAt() const { return m_createdAt; }
    QDateTime getUpdatedAt() const { return m_updatedAt; }
    void updateTimestamp() { m_updatedAt = QDateTime::currentDateTime(); }
    
    // 辅助方法
    bool isIncome() const { return m_type == Type::Income; }
    bool isExpense() const { return m_type == Type::Expense; }
    QString getTypeString() const;
    
private:
    QString m_id;
    Type m_type;
    double m_amount;
    QString m_categoryId;
    QDateTime m_dateTime;
    QString m_note;
    Status m_status;
    
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // RECORD_H