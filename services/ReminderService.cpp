#include "ReminderService.h"
#include <QDateTime>
#include <QUuid>
#include <QDebug>

ReminderService::ReminderService(std::shared_ptr<User> user, QObject *parent)
    : QObject(parent)
    , m_user(user)
    , m_budgetAlertEnabled(true)
    , m_budgetAlertThreshold(0.8)
    , m_periodicReportEnabled(true)
    , m_reportPeriodDays(30) {
    
    // 创建定时器
    m_periodicTimer = new QTimer(this);
    m_reportTimer = new QTimer(this);
    
    // 设置定时器间隔
    m_periodicTimer->setInterval(60000); // 每分钟检查一次
    m_reportTimer->setInterval(m_reportPeriodDays * 24 * 3600 * 1000); // 按设置周期
    
    // 连接定时器信号
    connect(m_periodicTimer, &QTimer::timeout, this, &ReminderService::onPeriodicCheck);
    connect(m_reportTimer, &QTimer::timeout, this, &ReminderService::onPeriodicReport);
}

ReminderService::~ReminderService() {
    stop();
}

void ReminderService::start() {
    m_periodicTimer->start();
    if (m_periodicReportEnabled) {
        m_reportTimer->start();
    }
}

void ReminderService::stop() {
    m_periodicTimer->stop();
    m_reportTimer->stop();
}

void ReminderService::setBudgetAlertEnabled(bool enabled) {
    m_budgetAlertEnabled = enabled;
}

void ReminderService::setBudgetAlertThreshold(double threshold) {
    if (threshold >= 0.0 && threshold <= 1.0) {
        m_budgetAlertThreshold = threshold;
    }
}

void ReminderService::setPeriodicReportEnabled(bool enabled) {
    m_periodicReportEnabled = enabled;
    if (enabled) {
        m_reportTimer->start();
    } else {
        m_reportTimer->stop();
    }
}

void ReminderService::setReportPeriod(int days) {
    if (days > 0) {
        m_reportPeriodDays = days;
        m_reportTimer->setInterval(days * 24 * 3600 * 1000);
    }
}

void ReminderService::checkBudgetStatus() {
    if (!m_budgetAlertEnabled) {
        return;
    }
    
    auto budgets = m_user->getAllBudgets();
    QDate currentDate = QDate::currentDate();
    
    for (const auto& budget : budgets) {
        if (budget->getStartDate() <= currentDate && budget->getEndDate() >= currentDate) {
            budget->checkAndUpdateStatus();
            
            // 检查是否需要发送提醒
            if (shouldSendBudgetWarning(budget)) {
                auto category = m_user->getCategory(budget->getCategoryId());
                QString categoryName = category ? category->getName() : "未知分类";
                
                emit budgetWarning(categoryName, budget->getUsagePercentage());
                
                Reminder reminder = createBudgetWarningReminder(categoryName, budget->getUsagePercentage());
                sendNotification(reminder);
                
            } else if (shouldSendBudgetOver(budget)) {
                auto category = m_user->getCategory(budget->getCategoryId());
                QString categoryName = category ? category->getName() : "未知分类";
                double overAmount = budget->getUsedAmount() - budget->getTotalAmount();
                
                emit budgetOver(categoryName, overAmount);
                
                Reminder reminder = createBudgetOverReminder(categoryName, overAmount);
                sendNotification(reminder);
            }
        }
    }
}

QVector<ReminderService::Reminder> ReminderService::getAllReminders() const {
    return m_reminders;
}

QVector<ReminderService::Reminder> ReminderService::getUnreadReminders() const {
    QVector<Reminder> unreadReminders;
    for (const auto& reminder : m_reminders) {
        if (!reminder.isRead) {
            unreadReminders.append(reminder);
        }
    }
    return unreadReminders;
}

void ReminderService::markReminderAsRead(const QString& reminderId) {
    for (auto& reminder : m_reminders) {
        if (reminder.id == reminderId) {
            reminder.isRead = true;
            break;
        }
    }
    emit unreadCountChanged(getUnreadReminders().size());
}

void ReminderService::markAllRemindersAsRead() {
    for (auto& reminder : m_reminders) {
        reminder.isRead = true;
    }
    emit unreadCountChanged(0);
}

void ReminderService::clearReminder(const QString& reminderId) {
    m_reminders.erase(
        std::remove_if(m_reminders.begin(), m_reminders.end(),
            [&reminderId](const Reminder& reminder) {
                return reminder.id == reminderId;
            }),
        m_reminders.end()
    );
    emit unreadCountChanged(getUnreadReminders().size());
}

void ReminderService::clearAllReminders() {
    m_reminders.clear();
    emit unreadCountChanged(0);
}

void ReminderService::onPeriodicCheck() {
    checkBudgetStatus();
}

void ReminderService::onPeriodicReport() {
    if (m_periodicReportEnabled) {
        Reminder reminder = createPeriodicReportReminder();
        sendNotification(reminder);
    }
}

QString ReminderService::generateReminderId() {
    return QUuid::createUuid().toString();
}

ReminderService::Reminder ReminderService::createBudgetWarningReminder(const QString& categoryName, double usedPercent) {
    Reminder reminder;
    reminder.id = generateReminderId();
    reminder.type = ReminderType::BudgetWarning;
    reminder.title = "预算警告";
    reminder.message = QString("%1 预算已使用 %.1f%%，请注意控制支出。").arg(categoryName).arg(usedPercent * 100);
    reminder.timestamp = QDateTime::currentDateTime();
    reminder.isRead = false;
    
    return reminder;
}

ReminderService::Reminder ReminderService::createBudgetOverReminder(const QString& categoryName, double overAmount) {
    Reminder reminder;
    reminder.id = generateReminderId();
    reminder.type = ReminderType::BudgetOver;
    reminder.title = "预算超支";
    reminder.message = QString("%1 预算已超支 ¥%.2f，请立即调整支出计划。").arg(categoryName).arg(overAmount);
    reminder.timestamp = QDateTime::currentDateTime();
    reminder.isRead = false;
    
    return reminder;
}

ReminderService::Reminder ReminderService::createPeriodicReportReminder() {
    Reminder reminder;
    reminder.id = generateReminderId();
    reminder.type = ReminderType::PeriodicReport;
    reminder.title = "定期报告";
    reminder.message = "您的本月财务报告已生成，请查看统计分析页面。";
    reminder.timestamp = QDateTime::currentDateTime();
    reminder.isRead = false;
    
    return reminder;
}

void ReminderService::sendNotification(const Reminder& reminder) {
    m_reminders.append(reminder);
    emit reminderTriggered(reminder);
    emit unreadCountChanged(getUnreadReminders().size());
}

bool ReminderService::shouldSendBudgetWarning(std::shared_ptr<Budget> budget) {
    return budget->getStatus() == Budget::Status::Warning && 
           budget->getUsagePercentage() >= m_budgetAlertThreshold;
}

bool ReminderService::shouldSendBudgetOver(std::shared_ptr<Budget> budget) {
    return budget->getStatus() == Budget::Status::OverBudget;
}