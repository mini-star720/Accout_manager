#ifndef REMINDERSERVICE_H
#define REMINDERSERVICE_H

#include <QObject>
#include <QTimer>
#include <memory>
#include "../models/User.h"
#include "../models/Budget.h"

class ReminderService : public QObject {
    Q_OBJECT

public:
    enum class ReminderType {
        BudgetWarning,
        BudgetOver,
        PeriodicReport,
        BillReminder
    };
    
    struct Reminder {
        QString id;
        ReminderType type;
        QString title;
        QString message;
        QDateTime timestamp;
        bool isRead;
        QVariant data;
    };
    
    explicit ReminderService(std::shared_ptr<User> user, QObject *parent = nullptr);
    ~ReminderService();
    
    // 启动提醒服务
    void start();
    void stop();
    
    // 设置提醒配置
    void setBudgetAlertEnabled(bool enabled);
    void setBudgetAlertThreshold(double threshold); // 0.0 - 1.0
    void setPeriodicReportEnabled(bool enabled);
    void setReportPeriod(int days);
    
    // 检查预算状态
    void checkBudgetStatus();
    
    // 获取提醒列表
    QVector<Reminder> getAllReminders() const;
    QVector<Reminder> getUnreadReminders() const;
    
    // 标记提醒已读
    void markReminderAsRead(const QString& reminderId);
    void markAllRemindersAsRead();
    
    // 清除提醒
    void clearReminder(const QString& reminderId);
    void clearAllReminders();

signals:
    void reminderTriggered(const Reminder& reminder);
    void budgetWarning(const QString& categoryName, double usedPercent);
    void budgetOver(const QString& categoryName, double overAmount);
    void unreadCountChanged(int count);

private slots:
    void onPeriodicCheck();
    void onPeriodicReport();

private:
    std::shared_ptr<User> m_user;
    QTimer* m_periodicTimer;
    QTimer* m_reportTimer;
    
    bool m_budgetAlertEnabled;
    double m_budgetAlertThreshold;
    bool m_periodicReportEnabled;
    int m_reportPeriodDays;
    
    QVector<Reminder> m_reminders;
    
    // 生成提醒ID
    QString generateReminderId();
    
    // 创建提醒
    Reminder createBudgetWarningReminder(const QString& categoryName, double usedPercent);
    Reminder createBudgetOverReminder(const QString& categoryName, double overAmount);
    Reminder createPeriodicReportReminder();
    
    // 发送通知
    void sendNotification(const Reminder& reminder);
    
    // 检查是否需要发送提醒
    bool shouldSendBudgetWarning(std::shared_ptr<Budget> budget);
    bool shouldSendBudgetOver(std::shared_ptr<Budget> budget);
};

#endif // REMINDERSERVICE_H