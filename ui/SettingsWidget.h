#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <memory>
#include "../models/User.h"
#include "../services/DataStorageService.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(std::shared_ptr<User> user, QWidget *parent = nullptr);
    ~SettingsWidget();

public slots:
    void loadSettings();
    void saveSettings();

private slots:
    void onUserInfoChanged();
    void onBackupPathChanged();
    void onSelectBackupPath();
    void onPerformBackup();
    void onRestoreBackup();
    void onExportData();
    void onImportData();
    void onClearData();
    void onThemeChanged(int index);
    void onLanguageChanged(int index);
    void onAutoBackupToggled(bool checked);
    void onReminderToggled(bool checked);
    
    void onBackupCompleted(const QString& backupPath);
    void onRestoreCompleted();
    void onErrorOccurred(const QString& error);

private:
    void setupUI();
    void createTabs();
    void createConnections();
    void loadUserInfo();
    void loadDataSettings();
    void loadAppearanceSettings();
    void loadNotificationSettings();
    void updateBackupInfo();
    
    // 标签页
    QTabWidget* m_tabWidget;
    
    // 个人信息页面
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QPushButton* m_saveUserButton;
    
    // 数据管理页面
    QLineEdit* m_backupPathEdit;
    QPushButton* m_selectPathButton;
    QPushButton* m_backupButton;
    QPushButton* m_restoreButton;
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
    QPushButton* m_clearButton;
    QLabel* m_backupInfoLabel;
    
    // 外观设置页面
    QComboBox* m_themeCombo;
    QComboBox* m_languageCombo;
    QCheckBox* m_darkModeCheck;
    
    // 通知设置页面
    QCheckBox* m_budgetAlertCheck;
    QCheckBox* m_periodicReportCheck;
    QSpinBox* m_reportIntervalSpin;
    QCheckBox* m_autoBackupCheck;
    QSpinBox* m_backupIntervalSpin;
    
    // 状态标签
    QLabel* m_statusLabel;
    
    // 数据
    std::shared_ptr<User> m_user;
    std::shared_ptr<DataStorageService> m_dataService;
};

#endif // SETTINGSWIDGET_H