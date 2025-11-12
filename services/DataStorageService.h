#ifndef DATASTORAGESERVICE_H
#define DATASTORAGESERVICE_H

#include <QObject>
#include <QString>
#include <memory>
#include "../models/User.h"
#include "../models/Record.h"
#include "../models/Category.h"
#include "../models/Budget.h"

class DataStorageService : public QObject {
    Q_OBJECT

public:
    explicit DataStorageService(QObject *parent = nullptr);
    ~DataStorageService();
    
    // 初始化数据库
    bool initialize(const QString& dbPath = QString());
    
    // 用户数据操作
    bool saveUser(std::shared_ptr<User> user);
    std::shared_ptr<User> loadUser(const QString& userId);
    bool deleteUser(const QString& userId);
    
    // 记录数据操作
    bool saveRecord(std::shared_ptr<Record> record, const QString& userId);
    bool saveRecords(const QVector<std::shared_ptr<Record>>& records, const QString& userId);
    QVector<std::shared_ptr<Record>> loadRecords(const QString& userId);
    bool deleteRecord(const QString& recordId);
    bool deleteRecordsPermanently(const QVector<QString>& recordIds);
    
    // 分类数据操作
    bool saveCategory(std::shared_ptr<Category> category, const QString& userId);
    bool saveCategories(const QVector<std::shared_ptr<Category>>& categories, const QString& userId);
    QVector<std::shared_ptr<Category>> loadCategories(const QString& userId);
    bool deleteCategory(const QString& categoryId);
    
    // 预算数据操作
    bool saveBudget(std::shared_ptr<Budget> budget, const QString& userId);
    bool saveBudgets(const QVector<std::shared_ptr<Budget>>& budgets, const QString& userId);
    QVector<std::shared_ptr<Budget>> loadBudgets(const QString& userId);
    bool deleteBudget(const QString& budgetId);
    
    // 数据导出
    bool exportToCSV(const QString& filePath, const QDate& startDate, const QDate& endDate);
    bool exportToJSON(const QString& filePath);
    
    // 数据备份和恢复
    bool backupData(const QString& backupPath);
    bool restoreData(const QString& backupPath);
    
    // 数据清理
    bool cleanupOldData(int daysToKeep);
    bool vacuumDatabase();
    
    // 数据库信息
    QString getDatabasePath() const;
    qint64 getDatabaseSize() const;
    QString getLastBackupTime() const;

signals:
    void dataSaved(const QString& tableName);
    void dataLoaded(const QString& tableName);
    void backupCompleted(const QString& backupPath);
    void restoreCompleted();
    void errorOccurred(const QString& error);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    
    // 数据库升级
    bool upgradeDatabase(int oldVersion);
    int getDatabaseVersion();
    
    // 数据验证
    bool validateRecord(std::shared_ptr<Record> record);
    bool validateCategory(std::shared_ptr<Category> category);
    bool validateBudget(std::shared_ptr<Budget> budget);
};

#endif // DATASTORAGESERVICE_H