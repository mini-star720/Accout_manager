#include "Category.h"

Category::Category(const QString& id) 
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_isIncomeCategory(false)
    , m_sortOrder(0) {
}

void Category::addSubCategory(const QString& categoryId) {
    if (!m_subCategoryIds.contains(categoryId)) {
        m_subCategoryIds.append(categoryId);
    }
}

void Category::removeSubCategory(const QString& categoryId) {
    m_subCategoryIds.removeAll(categoryId);
}

QString Category::getDisplayPath() const {
    if (m_parentId.isEmpty()) {
        return m_name;
    }
    return m_parentId + " > " + m_name;
}