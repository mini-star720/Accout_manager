#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QDebug>

SettingsWidget::SettingsWidget(std::shared_ptr<User> user, QWidget *parent)
    : QWidget(parent)
    , m_user(user)
    , m_dataService(std::make_shared<DataStorageService>())
{
    setupUI();
    createConnections();
    loadSettings();
}

SettingsWidget::~SettingsWidget() {
}

void SettingsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget(this);
    createTabs();
    
    mainLayout->addWidget(m_tabWidget);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪", this);
    mainLayout->addWidget(m_statusLabel);
}

void SettingsWidget::createTabs() {
    // 个人信息标签页
    QWidget* userInfoTab = new QWidget();
    QVBoxLayout* userLayout = new QVBoxLayout(userInfoTab);
    
    m_nameEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_saveUserButton = new QPushButton("保存信息", this);
    
    userLayout->addWidget(new QLabel("用户名:", this));
    userLayout->addWidget(m_nameEdit);
    userLayout->addWidget(new QLabel("邮箱:", this));
    userLayout->addWidget(m_emailEdit);
    userLayout->addWidget(m_saveUserButton);
    userLayout->addStretch();
    
    m_tabWidget->addTab(userInfoTab, "个人信息");
    
    // 数据管理标签页
    QWidget* dataTab = new QWidget();
    QVBoxLayout* dataLayout = new QVBoxLayout(dataTab);
    
    m_backupPathEdit = new QLineEdit(this);
    m_selectPathButton = new QPushButton("选择路径", this);
    m_backupButton = new QPushButton("备份数据", this);
    m_restoreButton = new QPushButton("恢复数据", this);
    m_exportButton = new QPushButton("导出数据", this);
    m_importButton = new QPushButton("导入数据", this);
    m_clearButton = new QPushButton("清除数据", this);
    m_backupInfoLabel = new QLabel("未备份", this);
    
    dataLayout->addWidget(new QLabel("备份路径:", this));
    
    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(m_backupPathEdit);
    pathLayout->addWidget(m_selectPathButton);
    dataLayout->addLayout(pathLayout);
    
    dataLayout->addWidget(new QLabel("备份操作:", this));
    
    QHBoxLayout* backupButtonsLayout = new QHBoxLayout();
    backupButtonsLayout->addWidget(m_backupButton);
    backupButtonsLayout->addWidget(m_restoreButton);
    dataLayout->addLayout(backupButtonsLayout);
    
    QHBoxLayout* importExportLayout = new QHBoxLayout();
    importExportLayout->addWidget(m_exportButton);
    importExportLayout->addWidget(m_importButton);
    dataLayout->addLayout(importExportLayout);
    
    dataLayout->addWidget(m_clearButton);
    dataLayout->addWidget(new QLabel("备份信息:", this));
    dataLayout->addWidget(m_backupInfoLabel);
    dataLayout->addStretch();
    
    m_tabWidget->addTab(dataTab, "数据管理");
    
    // 外观设置标签页
    QWidget* appearanceTab = new QWidget();
    QVBoxLayout* appearanceLayout = new QVBoxLayout(appearanceTab);
    
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem("浅色主题");
    m_themeCombo->addItem("深色主题");
    
    m_languageCombo = new QComboBox(this);
    m_languageCombo->addItem("中文");
    m_languageCombo->addItem("English");
    
    m_darkModeCheck = new QCheckBox("启用深色模式", this);
    
    appearanceLayout->addWidget(new QLabel("主题:", this));
    appearanceLayout->addWidget(m_themeCombo);
    appearanceLayout->addWidget(new QLabel("语言:", this));
    appearanceLayout->addWidget(m_languageCombo);
    appearanceLayout->addWidget(m_darkModeCheck);
    appearanceLayout->addStretch();
    
    m_tabWidget->addTab(appearanceTab, "外观设置");
    
    // 通知设置标签页
    QWidget* notificationTab = new QWidget();
    QVBoxLayout* notificationLayout = new QVBoxLayout(notificationTab);
    
    m_budgetAlertCheck = new QCheckBox("预算警告提醒", this);
    m_budgetAlertCheck->setChecked(true);
    
    m_periodicReportCheck = new QCheckBox("定期生成报告", this);
    m_reportIntervalSpin = new QSpinBox(this);
    m_reportIntervalSpin->setMinimum(1);
    m_reportIntervalSpin->setMaximum(30);
    m_reportIntervalSpin->setValue(7);
    m_reportIntervalSpin->setSuffix(" 天");
    
    m_autoBackupCheck = new QCheckBox("自动备份数据", this);
    m_backupIntervalSpin = new QSpinBox(this);
    m_backupIntervalSpin->setMinimum(1);
    m_backupIntervalSpin->setMaximum(30);
    m_backupIntervalSpin->setValue(7);
    m_backupIntervalSpin->setSuffix(" 天");
    
    notificationLayout->addWidget(m_budgetAlertCheck);
    notificationLayout->addWidget(m_periodicReportCheck);
    
    QHBoxLayout* reportLayout = new QHBoxLayout();
    reportLayout->addWidget(new QLabel("报告周期:"));
    reportLayout->addWidget(m_reportIntervalSpin);
    reportLayout->addStretch();
    notificationLayout->addLayout(reportLayout);
    
    notificationLayout->addWidget(m_autoBackupCheck);
    
    QHBoxLayout* backupLayout = new QHBoxLayout();
    backupLayout->addWidget(new QLabel("备份周期:"));
    backupLayout->addWidget(m_backupIntervalSpin);
    backupLayout->addStretch();
    notificationLayout->addLayout(backupLayout);
    
    notificationLayout->addStretch();
    
    m_tabWidget->addTab(notificationTab, "通知设置");
}

void SettingsWidget::createConnections() {
    connect(m_saveUserButton, &QPushButton::clicked, this, &SettingsWidget::onUserInfoChanged);
    connect(m_selectPathButton, &QPushButton::clicked, this, &SettingsWidget::onSelectBackupPath);
    connect(m_backupButton, &QPushButton::clicked, this, &SettingsWidget::onPerformBackup);
    connect(m_restoreButton, &QPushButton::clicked, this, &SettingsWidget::onRestoreBackup);
    connect(m_exportButton, &QPushButton::clicked, this, &SettingsWidget::onExportData);
    connect(m_importButton, &QPushButton::clicked, this, &SettingsWidget::onImportData);
    connect(m_clearButton, &QPushButton::clicked, this, &SettingsWidget::onClearData);
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onThemeChanged);
    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onLanguageChanged);
    connect(m_autoBackupCheck, &QCheckBox::toggled, this, &SettingsWidget::onAutoBackupToggled);
    connect(m_budgetAlertCheck, &QCheckBox::toggled, this, &SettingsWidget::onReminderToggled);
}

void SettingsWidget::loadUserInfo() {
}

void SettingsWidget::loadDataSettings() {
}

void SettingsWidget::loadAppearanceSettings() {
}

void SettingsWidget::loadNotificationSettings() {
}

void SettingsWidget::updateBackupInfo() {
}

void SettingsWidget::loadSettings() {
    loadUserInfo();
    loadDataSettings();
    loadAppearanceSettings();
    loadNotificationSettings();
}

void SettingsWidget::saveSettings() {
}

void SettingsWidget::onUserInfoChanged() {
    m_statusLabel->setText("用户信息已保存");
}

void SettingsWidget::onBackupPathChanged() {
}

void SettingsWidget::onSelectBackupPath() {
}

void SettingsWidget::onPerformBackup() {
    m_statusLabel->setText("正在备份数据...");
}

void SettingsWidget::onRestoreBackup() {
    m_statusLabel->setText("正在恢复数据...");
}

void SettingsWidget::onExportData() {
    m_statusLabel->setText("正在导出数据...");
}

void SettingsWidget::onImportData() {
    m_statusLabel->setText("正在导入数据...");
}

void SettingsWidget::onClearData() {
    m_statusLabel->setText("数据已清除");
}

void SettingsWidget::onThemeChanged(int index) {
}

void SettingsWidget::onLanguageChanged(int index) {
}

void SettingsWidget::onAutoBackupToggled(bool checked) {
}

void SettingsWidget::onReminderToggled(bool checked) {
}

void SettingsWidget::onBackupCompleted(const QString& backupPath) {
    m_statusLabel->setText(QString("备份已保存到: %1").arg(backupPath));
    updateBackupInfo();
}

void SettingsWidget::onRestoreCompleted() {
    m_statusLabel->setText("数据恢复完成");
}

void SettingsWidget::onErrorOccurred(const QString& error) {
    m_statusLabel->setText(QString("错误: %1").arg(error));
}
