#ifndef TRANSACTIONWIDGET_H
#define TRANSACTIONWIDGET_H

#include <QWidget>
#include <QDate>
#include <QDialog>
#include <memory>
#include "../models/User.h"
#include "../models/Record.h"
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QTableView;
class QPushButton;
class QLineEdit;
class QComboBox;
class QDateEdit;
class QTimeEdit;
class QLabel;
class QCalendarWidget;
class QSortFilterProxyModel;
class QDialog;
class QDialogButtonBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

class TransactionModel;
class AddTransactionDialog;

// 添加交易对话框（声明）
class AddTransactionDialog : public QDialog {
    Q_OBJECT

public:
    // If 'record' is provided, dialog will edit that record in-place; otherwise it creates a new one
    explicit AddTransactionDialog(std::shared_ptr<User> user, std::shared_ptr<Record> record = nullptr, QWidget *parent = nullptr);
    ~AddTransactionDialog();

    // 返回创建的记录（在 accepted 后有效）
    std::shared_ptr<Record> getRecord() const;

private slots:
    void onAccepted();

private:
    std::shared_ptr<User> m_user;
    std::shared_ptr<Record> m_record;

    // UI
    QDateEdit* m_dateEdit;
    QTimeEdit* m_timeEdit;
    QComboBox* m_typeCombo;
    QComboBox* m_categoryCombo;
    QDoubleSpinBox* m_amountSpin;
    QLineEdit* m_noteEdit;
    QDialogButtonBox* m_buttonBox;
};

class TransactionWidget : public QWidget {
    Q_OBJECT

public:
    explicit TransactionWidget(std::shared_ptr<User> user, QWidget *parent = nullptr);
    ~TransactionWidget();

public slots:
    void refreshData();
    void filterByDate(const QDate& date);
    void filterByCategory(const QString& categoryId);
    void searchTransactions(const QString& searchText);

private slots:
    void onAddTransaction();
    void onEditTransaction();
    void onDeleteTransaction();
    void onExportTransactions();
    void onCalendarDateSelected(const QDate& date);
    void onTransactionSelected(const QModelIndex& index);
    void onCategoryFilterChanged(int index);
    void updateSummary();

signals:
    void recordsChanged();

private:
    void setupUI();
    void createLayout();
    void createConnections();
    void loadCategories();
    void updateCategoryFilter();
    void showContextMenu(const QPoint& pos);
    
    // UI组件
    QTableView* m_transactionView;
    QCalendarWidget* m_calendarWidget;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_exportButton;
    QLineEdit* m_searchEdit;
    QComboBox* m_categoryFilter;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    
    // 统计标签
    QLabel* m_totalIncomeLabel;
    QLabel* m_totalExpenseLabel;
    QLabel* m_balanceLabel;
    QLabel* m_transactionCountLabel;
    
    // 数据模型
    TransactionModel* m_model;
    QSortFilterProxyModel* m_proxyModel;
    
    // 对话框
    AddTransactionDialog* m_addDialog;
    
    // 数据
    std::shared_ptr<User> m_user;
    std::shared_ptr<Record> m_selectedRecord;
};

// 交易记录数据模型
class TransactionModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TransactionModel(std::shared_ptr<User> user, QObject *parent = nullptr);
    
    // 重载的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    
    // 自定义函数
    void setRecords(const QVector<std::shared_ptr<Record>>& records);
    void addRecord(std::shared_ptr<Record> record);
    void removeRecord(const QString& recordId);
    void updateRecord(std::shared_ptr<Record> record);
    int indexOfRecord(const QString& recordId) const;
    std::shared_ptr<Record> getRecord(int row) const;
    void clear();
    
    // 列定义
    enum Column {
        DateColumn,
        TypeColumn,
        CategoryColumn,
        AmountColumn,
        NoteColumn,
        ColumnCount
    };

private:
    QVector<std::shared_ptr<Record>> m_records;
    std::shared_ptr<User> m_user;
};

#endif // TRANSACTIONWIDGET_H