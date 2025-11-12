#include "mainwindow.h"
#include "ui/TransactionWidget.h"
#include "ui/StatisticsWidget.h"
#include "ui/BudgetWidget.h"
#include "ui/SettingsWidget.h"
#include <QStackedWidget>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentUser(std::make_shared<User>("user_001", "默认用户"))
{
    setupUI();
    createActions();
    createToolBar();
    createStatusBar();
    initializeServices();
    loadUserData();
}

MainWindow::~MainWindow() {
    saveUserData();
}

void MainWindow::setupUI() {
    setWindowTitle("智能记账本系统");
    setMinimumSize(1200, 800);
    
    // 创建中央堆叠窗口
    m_centralStack = new QStackedWidget(this);
    setCentralWidget(m_centralStack);
    
    // 创建各个功能界面
    m_transactionWidget = new TransactionWidget(m_currentUser, this);
    m_statisticsWidget = new StatisticsWidget(m_currentUser, this);
    m_budgetWidget = new BudgetWidget(m_currentUser, this);
    m_settingsWidget = new SettingsWidget(m_currentUser, this);
    
    // 添加到堆叠窗口
    m_centralStack->addWidget(m_transactionWidget);
    m_centralStack->addWidget(m_statisticsWidget);
    m_centralStack->addWidget(m_budgetWidget);
    m_centralStack->addWidget(m_settingsWidget);
}

void MainWindow::createActions() {
    // 流水记账动作
    m_transactionAction = new QAction(QIcon(":/icons/transaction.png"), "流水记账", this);
    m_transactionAction->setCheckable(true);
    m_transactionAction->setChecked(true);
    
    // 统计分析动作
    m_statisticsAction = new QAction(QIcon(":/icons/statistics.png"), "统计分析", this);
    m_statisticsAction->setCheckable(true);
    
    // 预算管理动作
    m_budgetAction = new QAction(QIcon(":/icons/budget.png"), "预算管理", this);
    m_budgetAction->setCheckable(true);
    
    // 系统设置动作
    m_settingsAction = new QAction(QIcon(":/icons/settings.png"), "系统设置", this);
    m_settingsAction->setCheckable(true);
    
    // 创建动作组
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(m_transactionAction);
    actionGroup->addAction(m_statisticsAction);
    actionGroup->addAction(m_budgetAction);
    actionGroup->addAction(m_settingsAction);
    actionGroup->setExclusive(true);
    
    // 连接信号
    connect(actionGroup, &QActionGroup::triggered, this, &MainWindow::onNavigationTriggered);
}

void MainWindow::createToolBar() {
    m_navigationBar = addToolBar("导航");
    m_navigationBar->setMovable(false);
    m_navigationBar->setFloatable(false);
    
    // 添加动作到工具栏
    m_navigationBar->addAction(m_transactionAction);
    m_navigationBar->addAction(m_statisticsAction);
    m_navigationBar->addAction(m_budgetAction);
    m_navigationBar->addAction(m_settingsAction);
    
    // 添加分隔符和额外按钮
    m_navigationBar->addSeparator();
    
    // 关于按钮
    QAction* aboutAction = m_navigationBar->addAction("关于");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    
    // 帮助按钮
    QAction* helpAction = m_navigationBar->addAction("帮助");
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelpDialog);
}

void MainWindow::createStatusBar() {
    // 状态标签
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);
    
    // 余额标签
    m_balanceLabel = new QLabel("本月余额: ¥0.00");
    m_balanceLabel->setStyleSheet("QLabel { color: #27AE60; font-weight: bold; }");
    statusBar()->addPermanentWidget(m_balanceLabel);
    
    // 提醒标签
    m_reminderLabel = new QLabel("提醒: 0");
    m_reminderLabel->setStyleSheet("QLabel { color: #E74C3C; }");
    statusBar()->addPermanentWidget(m_reminderLabel);
}

void MainWindow::initializeServices() {
    // 创建报告服务
    m_reportService = std::make_shared<ReportService>(m_currentUser);
    
    // 创建提醒服务
    m_reminderService = std::make_shared<ReminderService>(m_currentUser);
    connect(m_reminderService.get(), &ReminderService::budgetWarning, 
            this, &MainWindow::onBudgetWarning);
    connect(m_reminderService.get(), &ReminderService::budgetOver, 
            this, &MainWindow::onBudgetOver);
    connect(m_reminderService.get(), &ReminderService::unreadCountChanged, 
            this, &MainWindow::onUnreadRemindersChanged);

    // 如果交易或预算发生变化，立即检查预算状态
    if (m_transactionWidget) {
    connect(m_transactionWidget, &TransactionWidget::recordsChanged,
        m_reminderService.get(), &ReminderService::checkBudgetStatus);
    // also update balance display and refresh budget view when transactions change
    connect(m_transactionWidget, &TransactionWidget::recordsChanged,
        this, &MainWindow::updateBalanceDisplay);
    connect(m_transactionWidget, &TransactionWidget::recordsChanged,
        m_budgetWidget, &BudgetWidget::refreshData);
    }
    if (m_budgetWidget) {
    connect(m_budgetWidget, &BudgetWidget::budgetWarning, this, &MainWindow::onBudgetWarning);
    connect(m_budgetWidget, &BudgetWidget::budgetOver, this, &MainWindow::onBudgetOver);
    }
    
    // 创建数据存储服务
    m_dataService = std::make_shared<DataStorageService>();
    
    // 启动提醒服务
    m_reminderService->start();
}

void MainWindow::loadUserData() {
    // 这里可以从数据库加载用户数据
    // 现在使用默认数据
    
    // 添加一些示例分类
    auto foodCategory = std::make_shared<Category>();
    foodCategory->setName("餐饮");
    foodCategory->setIcon(":/icons/categories/food.png");
    foodCategory->setColor("#FF6B35");
    m_currentUser->addCategory(foodCategory);
    
    auto transportCategory = std::make_shared<Category>();
    transportCategory->setName("交通");
    transportCategory->setIcon(":/icons/categories/transport.png");
    transportCategory->setColor("#4A90E2");
    m_currentUser->addCategory(transportCategory);
    
    auto shoppingCategory = std::make_shared<Category>();
    shoppingCategory->setName("购物");
    shoppingCategory->setIcon(":/icons/categories/shopping.png");
    shoppingCategory->setColor("#9013FE");
    m_currentUser->addCategory(shoppingCategory);
    
    // 更新余额显示
    updateBalanceDisplay();
}

void MainWindow::saveUserData() {
    // 保存用户数据到数据库
    if (m_dataService) {
        m_dataService->saveUser(m_currentUser);
    }
}

void MainWindow::onNavigationTriggered(QAction* action) {
    if (action == m_transactionAction) {
        m_centralStack->setCurrentWidget(m_transactionWidget);
        m_statusLabel->setText("流水记账");
    } else if (action == m_statisticsAction) {
        m_centralStack->setCurrentWidget(m_statisticsWidget);
        m_statusLabel->setText("统计分析");
    } else if (action == m_budgetAction) {
        m_centralStack->setCurrentWidget(m_budgetWidget);
        m_statusLabel->setText("预算管理");
    } else if (action == m_settingsAction) {
        m_centralStack->setCurrentWidget(m_settingsWidget);
        m_statusLabel->setText("系统设置");
    }
}

void MainWindow::onBudgetWarning(const QString& categoryName, double usedPercent) {
    QString message = QString("预算警告: %1 已使用 %.1f%%").arg(categoryName).arg(usedPercent * 100);
    QMessageBox::warning(this, "预算警告", message);
    m_statusLabel->setText(message);
}

void MainWindow::onBudgetOver(const QString& categoryName, double overAmount) {
    QString message = QString("预算超支: %1 超支 ¥%.2f").arg(categoryName).arg(overAmount);
    QMessageBox::critical(this, "预算超支", message);
    m_statusLabel->setText(message);
}

void MainWindow::onUnreadRemindersChanged(int count) {
    m_reminderLabel->setText(QString("提醒: %1").arg(count));
    if (count > 0) {
        m_reminderLabel->setStyleSheet("QLabel { color: #E74C3C; font-weight: bold; }");
    } else {
        m_reminderLabel->setStyleSheet("QLabel { color: #27AE60; }");
    }
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "关于智能记账本",
        "<h2>智能记账本系统 v1.0.0</h2>"
        "<p>基于Qt C++开发的个人记账管理系统</p>"
        "<p>功能特点:</p>"
        "<ul>"
        "<li>流水记账与分类管理</li>"
        "<li>多维度统计分析</li>"
        "<li>智能预算提醒</li>"
        "<li>数据安全备份</li>"
        "</ul>"
        "<p>开发者: AI助手</p>"
        "<p>基于UML设计模式实现</p>");
}

void MainWindow::showHelpDialog() {
    QMessageBox::information(this, "使用帮助",
        "<h2>智能记账本使用帮助</h2>"
        "<h3>基本功能</h3>"
        "<p><b>流水记账:</b> 记录收入和支出，支持分类管理</p>"
        "<p><b>统计分析:</b> 查看收支趋势和分类分布图表</p>"
        "<p><b>预算管理:</b> 设置预算并接收超支提醒</p>"
        "<p><b>系统设置:</b> 管理个人偏好和数据备份</p>"
        "<h3>快捷键</h3>"
        "<p><b>Ctrl+N:</b> 新建记录</p>"
        "<p><b>Ctrl+E:</b> 编辑记录</p>"
        "<p><b>Ctrl+D:</b> 删除记录</p>"
        "<p><b>Ctrl+S:</b> 保存数据</p>"
        "<p><b>Ctrl+Q:</b> 退出程序</p>");
}

void MainWindow::updateBalanceDisplay() {
    QDate currentDate = QDate::currentDate();
    QDate startOfMonth(currentDate.year(), currentDate.month(), 1);
    
    double balance = m_currentUser->getBalance(startOfMonth, currentDate);
    
    if (balance >= 0) {
        m_balanceLabel->setText(QString("本月余额: ¥%1").arg(balance, 0, 'f', 2));
        m_balanceLabel->setStyleSheet("QLabel { color: #27AE60; font-weight: bold; }");
    } else {
        m_balanceLabel->setText(QString("本月赤字: ¥%1").arg(-balance, 0, 'f', 2));
        m_balanceLabel->setStyleSheet("QLabel { color: #E74C3C; font-weight: bold; }");
    }
}