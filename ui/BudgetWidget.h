#ifndef BUDGETWIDGET_H
#define BUDGETWIDGET_H

#include <QWidget>
#include <QObject>
#include <memory>
#include "../models/User.h"
#include "../models/Budget.h"
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QTableView;
class QPushButton;
class QLineEdit;
class QDoubleSpinBox;
class QSlider;
class QProgressBar;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class BudgetModel;

class BudgetWidget : public QWidget {
    Q_OBJECT

public:
    explicit BudgetWidget(std::shared_ptr<User> user, QWidget *parent = nullptr);
    ~BudgetWidget();

public slots:
    void refreshData();
    void loadBudgets();

private slots:
    void onAddBudget();
    void onNewBudget();
    void onEditBudget();
    void onDeleteBudget();
    void onBudgetSelected(const QModelIndex& index);
    void onBudgetAmountChanged(double amount);
    void onAlertThresholdChanged(int value);
    void onPeriodChanged(int index);
    void onCategorySelected(const QString& categoryId);
    void updateBudgetProgress();
    void checkBudgetAlerts();

signals:
    void budgetWarning(const QString& categoryName, double usedPercent);
    void budgetOver(const QString& categoryName, double overAmount);

private:
    void setupUI();
    void createLayout();
    void createConnections();
    void loadCategories();
    void updateBudgetDisplay();
    void showBudgetAlert(std::shared_ptr<Budget> budget);
    
    // UI组件
    QTableView* m_budgetView;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    
    // 预算设置组件
    QComboBox* m_categoryCombo;
    QDoubleSpinBox* m_amountSpinBox;
    QSlider* m_alertSlider;
    QLabel* m_alertLabel;
    QComboBox* m_periodCombo;
    QPushButton* m_saveButton;
    
    // 进度显示
    QProgressBar* m_progressBar;
    QLabel* m_usedAmountLabel;
    QLabel* m_totalAmountLabel;
    QLabel* m_remainingAmountLabel;
    QLabel* m_usagePercentageLabel;
    
    // 数据模型
    BudgetModel* m_model;
    
    // 数据
    std::shared_ptr<User> m_user;
    std::shared_ptr<Budget> m_selectedBudget;
};

// 预算数据模型
class BudgetModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit BudgetModel(QObject *parent = nullptr);
    void setUser(std::shared_ptr<User> user);
    
    // 重载的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 自定义函数
    void setBudgets(const QVector<std::shared_ptr<Budget>>& budgets);
    void addBudget(std::shared_ptr<Budget> budget);
    void updateBudget(std::shared_ptr<Budget> budget);
    void removeBudget(const QString& budgetId);
    std::shared_ptr<Budget> getBudget(int row) const;
    void clear();
    
    // 列定义
    enum Column {
        CategoryColumn,
        TotalAmountColumn,
        UsedAmountColumn,
        RemainingColumn,
        UsageColumn,
        StatusColumn,
        ColumnCount
    };

private:
    QVector<std::shared_ptr<Budget>> m_budgets;
    std::shared_ptr<User> m_user;
};

#endif // BUDGETWIDGET_H