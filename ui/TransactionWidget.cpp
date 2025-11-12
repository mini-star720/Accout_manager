#include "TransactionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QLabel>
#include <QTableView>
#include <QCalendarWidget>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QDateTime>

TransactionWidget::TransactionWidget(std::shared_ptr<User> user, QWidget *parent)
    : QWidget(parent)
    , m_user(user)
    , m_model(nullptr)
    , m_proxyModel(nullptr)
    , m_addDialog(nullptr)
{
    setupUI();
    createConnections();
    loadCategories();
}

TransactionWidget::~TransactionWidget() {
}

void TransactionWidget::setupUI() {
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 创建顶部工具栏
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("添加交易", this);
    m_editButton = new QPushButton("编辑", this);
    m_deleteButton = new QPushButton("删除", this);
    m_exportButton = new QPushButton("导出", this);
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索交易...");
    
    m_categoryFilter = new QComboBox(this);
    
    toolbarLayout->addWidget(new QLabel("分类:"));
    toolbarLayout->addWidget(m_categoryFilter);
    toolbarLayout->addWidget(new QLabel("搜索:"));
    toolbarLayout->addWidget(m_searchEdit);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_addButton);
    toolbarLayout->addWidget(m_editButton);
    toolbarLayout->addWidget(m_deleteButton);
    toolbarLayout->addWidget(m_exportButton);

    // 初始时禁用编辑与删除按钮
    m_editButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    
    mainLayout->addLayout(toolbarLayout);
    
    // 创建内容区域
    QHBoxLayout* contentLayout = new QHBoxLayout();
    
    // 左侧：日历
    m_calendarWidget = new QCalendarWidget(this);
    
    // 中间：表格
    m_transactionView = new QTableView(this);
    m_model = new TransactionModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_transactionView->setModel(m_proxyModel);
    
    // 右侧：统计信息
    QVBoxLayout* statsLayout = new QVBoxLayout();
    
    m_totalIncomeLabel = new QLabel("收入: ¥0.00", this);
    m_totalExpenseLabel = new QLabel("支出: ¥0.00", this);
    m_balanceLabel = new QLabel("余额: ¥0.00", this);
    m_transactionCountLabel = new QLabel("交易数: 0", this);
    
    statsLayout->addWidget(m_totalIncomeLabel);
    statsLayout->addWidget(m_totalExpenseLabel);
    statsLayout->addWidget(m_balanceLabel);
    statsLayout->addWidget(m_transactionCountLabel);
    statsLayout->addStretch();
    
    contentLayout->addWidget(m_calendarWidget, 1);
    contentLayout->addWidget(m_transactionView, 2);
    contentLayout->addLayout(statsLayout, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // 创建底部日期范围选择
    QHBoxLayout* dateRangeLayout = new QHBoxLayout();
    m_startDateEdit = new QDateEdit(this);
    m_endDateEdit = new QDateEdit(this);
    m_startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    m_endDateEdit->setDate(QDate::currentDate());
    
    dateRangeLayout->addWidget(new QLabel("开始日期:"));
    dateRangeLayout->addWidget(m_startDateEdit);
    dateRangeLayout->addWidget(new QLabel("结束日期:"));
    dateRangeLayout->addWidget(m_endDateEdit);
    dateRangeLayout->addStretch();
    
    mainLayout->addLayout(dateRangeLayout);
}

void TransactionWidget::createLayout() {
    // 已在 setupUI 中实现
}

void TransactionWidget::createConnections() {
    connect(m_addButton, &QPushButton::clicked, this, &TransactionWidget::onAddTransaction);
    connect(m_editButton, &QPushButton::clicked, this, &TransactionWidget::onEditTransaction);
    connect(m_deleteButton, &QPushButton::clicked, this, &TransactionWidget::onDeleteTransaction);
    connect(m_exportButton, &QPushButton::clicked, this, &TransactionWidget::onExportTransactions);
    connect(m_calendarWidget, &QCalendarWidget::clicked, this, &TransactionWidget::onCalendarDateSelected);
    connect(m_transactionView, &QTableView::clicked, this, &TransactionWidget::onTransactionSelected);
    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TransactionWidget::onCategoryFilterChanged);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &TransactionWidget::searchTransactions);
}

void TransactionWidget::loadCategories() {
    m_categoryFilter->addItem("全部分类", "");
    // 从用户数据中加载分类
}

void TransactionWidget::updateCategoryFilter() {
}

void TransactionWidget::showContextMenu(const QPoint& pos) {
}

void TransactionWidget::refreshData() {
    updateSummary();
}

void TransactionWidget::filterByDate(const QDate& date) {
}

void TransactionWidget::filterByCategory(const QString& categoryId) {
}

void TransactionWidget::searchTransactions(const QString& searchText) {
    m_proxyModel->setFilterFixedString(searchText);
}

void TransactionWidget::onAddTransaction() {
    // 打开添加交易对话框
    AddTransactionDialog dlg(m_user, nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
        auto record = dlg.getRecord();
        if (record) {
            // 添加到用户数据和模型
            m_user->addRecord(record);
            if (m_model) {
                m_model->addRecord(record);
            }
            updateSummary();
        }
    }
}

void TransactionWidget::onEditTransaction() {
    if (!m_selectedRecord) return;

    AddTransactionDialog dlg(m_user, m_selectedRecord, this);
    if (dlg.exec() == QDialog::Accepted) {
        // record modified in-place
        if (m_model) {
            m_model->updateRecord(m_selectedRecord);
        }
    }
}

void TransactionWidget::onDeleteTransaction() {
    if (!m_selectedRecord) return;

    QString id = m_selectedRecord->getId();
    // 从用户中移除
    m_user->removeRecord(id);
    // 从模型中移除
    if (m_model) m_model->removeRecord(id);

    m_selectedRecord = nullptr;
    m_editButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
}

void TransactionWidget::onExportTransactions() {
}

void TransactionWidget::onCalendarDateSelected(const QDate& date) {
}

void TransactionWidget::onTransactionSelected(const QModelIndex& index) {
    if (!index.isValid()) {
        m_selectedRecord = nullptr;
        m_editButton->setEnabled(false);
        m_deleteButton->setEnabled(false);
        return;
    }

    QModelIndex src = m_proxyModel ? m_proxyModel->mapToSource(index) : index;
    int row = src.row();
    if (m_model) {
        m_selectedRecord = m_model->getRecord(row);
        m_editButton->setEnabled(m_selectedRecord != nullptr);
        m_deleteButton->setEnabled(m_selectedRecord != nullptr);
    }
}

void TransactionWidget::onCategoryFilterChanged(int index) {
}

void TransactionWidget::updateSummary() {
}

// TransactionModel 实现
TransactionModel::TransactionModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TransactionModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_records.size();
}

int TransactionModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    
    if (index.row() >= m_records.size())
        return QVariant();
    
    auto record = m_records[index.row()];
    
    switch (index.column()) {
        case DateColumn:
            return record ? record->getDateTime().toString("yyyy-MM-dd hh:mm") : "";
        case TypeColumn:
            return record ? (record->isIncome() ? "收入" : "支出") : "";
        case CategoryColumn:
            return ""; // 需要实现
        case AmountColumn:
            return record ? QString::number(record->getAmount(), 'f', 2) : "";
        case NoteColumn:
            return record ? record->getNote() : "";
        default:
            return QVariant();
    }
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    
    const char* headers[] = {"日期", "类型", "分类", "金额", "备注"};
    if (section < ColumnCount)
        return headers[section];
    
    return QVariant();
}

bool TransactionModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid())
        return false;
    
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_records.removeAt(row);
    endRemoveRows();
    
    return true;
}

void TransactionModel::setRecords(const QVector<std::shared_ptr<Record>>& records) {
    beginResetModel();
    m_records = records;
    endResetModel();
}

void TransactionModel::addRecord(std::shared_ptr<Record> record) {
    beginInsertRows(QModelIndex(), m_records.size(), m_records.size());
    m_records.append(record);
    endInsertRows();
}

void TransactionModel::removeRecord(const QString& recordId) {
    for (int i = 0; i < m_records.size(); ++i) {
        if (m_records[i] && m_records[i]->getId() == recordId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_records.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void TransactionModel::updateRecord(std::shared_ptr<Record> record) {
    if (!record) return;
    for (int i = 0; i < m_records.size(); ++i) {
        if (m_records[i] && m_records[i]->getId() == record->getId()) {
            m_records[i] = record;
            QModelIndex topLeft = index(i, 0);
            QModelIndex bottomRight = index(i, ColumnCount - 1);
            emit dataChanged(topLeft, bottomRight);
            return;
        }
    }
}

int TransactionModel::indexOfRecord(const QString& recordId) const {
    for (int i = 0; i < m_records.size(); ++i) {
        if (m_records[i] && m_records[i]->getId() == recordId) return i;
    }
    return -1;
}

std::shared_ptr<Record> TransactionModel::getRecord(int row) const {
    if (row >= 0 && row < m_records.size())
        return m_records[row];
    return nullptr;
}

void TransactionModel::clear() {
    beginResetModel();
    m_records.clear();
    endResetModel();
}

// AddTransactionDialog implementation
AddTransactionDialog::AddTransactionDialog(std::shared_ptr<User> user, std::shared_ptr<Record> record, QWidget *parent)
    : QDialog(parent)
    , m_user(user)
    , m_record(record)
{
    setWindowTitle("添加交易");
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* form = new QFormLayout();

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_timeEdit = new QTimeEdit(QTime::currentTime(), this);

    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem("支出");
    m_typeCombo->addItem("收入");

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItem("未分类", "");
    if (m_user) {
        for (const auto& cat : m_user->getAllCategories()) {
            m_categoryCombo->addItem(cat->getName(), cat->getId());
        }
    }

    // 如果是编辑已有记录，填充初始值
    if (m_record) {
        QDateTime dt = m_record->getDateTime();
        m_dateEdit->setDate(dt.date());
        m_timeEdit->setTime(dt.time());
        m_typeCombo->setCurrentText(m_record->isIncome() ? "收入" : "支出");
        QString cid = m_record->getCategoryId();
        int idx = m_categoryCombo->findData(cid);
        if (idx >= 0) m_categoryCombo->setCurrentIndex(idx);
        m_amountSpin->setValue(m_record->getAmount());
        m_noteEdit->setText(m_record->getNote());
    }

    m_amountSpin = new QDoubleSpinBox(this);
    m_amountSpin->setMinimum(0.01);
    m_amountSpin->setMaximum(1e9);
    m_amountSpin->setDecimals(2);

    m_noteEdit = new QLineEdit(this);

    form->addRow("日期:", m_dateEdit);
    form->addRow("时间:", m_timeEdit);
    form->addRow("类型:", m_typeCombo);
    form->addRow("分类:", m_categoryCombo);
    form->addRow("金额:", m_amountSpin);
    form->addRow("备注:", m_noteEdit);

    mainLayout->addLayout(form);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AddTransactionDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(m_buttonBox);
}

AddTransactionDialog::~AddTransactionDialog() {}

std::shared_ptr<Record> AddTransactionDialog::getRecord() const {
    return m_record;
}

void AddTransactionDialog::onAccepted() {
    // 构造 Record
    if (!m_record) m_record = std::make_shared<Record>();

    // 设置日期时间
    QDate date = m_dateEdit->date();
    QTime time = m_timeEdit->time();
    m_record->setDateTime(QDateTime(date, time));

    // 类型
    QString type = m_typeCombo->currentText();
    if (type == "收入") m_record->setType(Record::Type::Income);
    else m_record->setType(Record::Type::Expense);

    // 分类
    QString catId = m_categoryCombo->currentData().toString();
    m_record->setCategoryId(catId);

    // 金额与备注
    m_record->setAmount(m_amountSpin->value());
    m_record->setNote(m_noteEdit->text());

    accept();
}
