#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "models/User.h"
#include "services/ReportService.h"
#include "services/ReminderService.h"
#include "services/DataStorageService.h"

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QToolBar;
class QAction;
class QLabel;
QT_END_NAMESPACE

class TransactionWidget;
class StatisticsWidget;
class BudgetWidget;
class SettingsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNavigationTriggered(QAction* action);
    void onBudgetWarning(const QString& categoryName, double usedPercent);
    void onBudgetOver(const QString& categoryName, double overAmount);
    void onUnreadRemindersChanged(int count);
    void showAboutDialog();
    void showHelpDialog();

private:
    void setupUI();
    void createActions();
    void createToolBar();
    void createStatusBar();
    void initializeServices();
    void loadUserData();
    void saveUserData();
    void updateBalanceDisplay();
    
    // UI组件
    QStackedWidget* m_centralStack;
    QToolBar* m_navigationBar;
    
    // 动作
    QAction* m_transactionAction;
    QAction* m_statisticsAction;
    QAction* m_budgetAction;
    QAction* m_settingsAction;
    
    // 界面组件
    TransactionWidget* m_transactionWidget;
    StatisticsWidget* m_statisticsWidget;
    BudgetWidget* m_budgetWidget;
    SettingsWidget* m_settingsWidget;
    
    // 数据和服务
    std::shared_ptr<User> m_currentUser;
    std::shared_ptr<ReportService> m_reportService;
    std::shared_ptr<ReminderService> m_reminderService;
    std::shared_ptr<DataStorageService> m_dataService;
    
    // 状态栏标签
    QLabel* m_statusLabel;
    QLabel* m_balanceLabel;
    QLabel* m_reminderLabel;
};

#endif // MAINWINDOW_H