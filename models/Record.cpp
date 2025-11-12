#include "Record.h"

Record::Record(const QString& id) 
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_type(Type::Expense)
    , m_amount(0.0)
    , m_status(Status::Draft)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(m_createdAt) {
}

QString Record::getTypeString() const {
    switch (m_type) {
        case Type::Income:
            return "收入";
        case Type::Expense:
            return "支出";
        default:
            return "未知";
    }
}