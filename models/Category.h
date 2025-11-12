#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include <QUuid>
#include <QVector>

class Category {
public:
    Category(const QString& id = QUuid::createUuid().toString());
    
    // Getter和Setter
    QString getId() const { return m_id; }
    
    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    QString getIcon() const { return m_icon; }
    void setIcon(const QString& icon) { m_icon = icon; }
    
    QString getColor() const { return m_color; }
    void setColor(const QString& color) { m_color = color; }
    
    QString getParentId() const { return m_parentId; }
    void setParentId(const QString& parentId) { m_parentId = parentId; }
    
    bool isIncomeCategory() const { return m_isIncomeCategory; }
    void setIsIncomeCategory(bool isIncome) { m_isIncomeCategory = isIncome; }
    
    int getSortOrder() const { return m_sortOrder; }
    void setSortOrder(int order) { m_sortOrder = order; }
    
    // 层级关系管理
    bool isTopLevel() const { return m_parentId.isEmpty(); }
    QVector<QString> getSubCategoryIds() const { return m_subCategoryIds; }
    void addSubCategory(const QString& categoryId);
    void removeSubCategory(const QString& categoryId);
    
    // 辅助方法
    bool hasSubCategories() const { return !m_subCategoryIds.isEmpty(); }
    QString getDisplayPath() const;
    
private:
    QString m_id;
    QString m_name;
    QString m_icon;
    QString m_color;
    QString m_parentId;
    bool m_isIncomeCategory;
    int m_sortOrder;
    
    QVector<QString> m_subCategoryIds; // 子分类ID列表
};

#endif // CATEGORY_H