#include "BudgetWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include <QTableView>
#include <QDebug>

BudgetWidget::BudgetWidget(std::shared_ptr<User> user, QWidget *parent)
    : QWidget(parent)
    , m_user(user)
    , m_model(nullptr)
    , m_selectedBudget(nullptr)
{
    setupUI();
    createConnections();
    loadCategories();
    loadBudgets();
}

BudgetWidget::~BudgetWidget() {
}

void BudgetWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 工具栏
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    m_addButton = new QPushButton("添加预算", this);
    m_editButton = new QPushButton("编辑", this);
    m_deleteButton = new QPushButton("删除", this);
    
    toolbarLayout->addWidget(m_addButton);
    toolbarLayout->addWidget(m_editButton);
    toolbarLayout->addWidget(m_deleteButton);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);
    
    // 内容区域
    QHBoxLayout* contentLayout = new QHBoxLayout();
    
    // 左侧表格
    m_budgetView = new QTableView(this);
    m_model = new BudgetModel(this);
    m_model->setUser(m_user);
    m_budgetView->setModel(m_model);
    m_budgetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_budgetView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 右侧设置面板
    QVBoxLayout* settingsLayout = new QVBoxLayout();
    
    m_categoryCombo = new QComboBox(this);
    m_amountSpinBox = new QDoubleSpinBox(this);
    m_amountSpinBox->setMaximum(1e9);
    m_alertSlider = new QSlider(Qt::Horizontal, this);
    m_alertLabel = new QLabel("警告阈值: 80%", this);
    m_periodCombo = new QComboBox(this);
    m_saveButton = new QPushButton("保存预算", this);
    
    m_alertSlider->setMinimum(0);
    m_alertSlider->setMaximum(100);
    m_alertSlider->setValue(80);
    
    m_periodCombo->addItem("月度", "monthly");
    m_periodCombo->addItem("年度", "yearly");
    
    settingsLayout->addWidget(new QLabel("分类:", this));
    settingsLayout->addWidget(m_categoryCombo);
    settingsLayout->addWidget(new QLabel("预算金额:", this));
    settingsLayout->addWidget(m_amountSpinBox);
    settingsLayout->addWidget(m_alertLabel);
    settingsLayout->addWidget(m_alertSlider);
    settingsLayout->addWidget(new QLabel("预算周期:", this));
    settingsLayout->addWidget(m_periodCombo);
    settingsLayout->addWidget(m_saveButton);
    settingsLayout->addStretch();
    
    contentLayout->addWidget(m_budgetView, 2);
    contentLayout->addLayout(settingsLayout, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // 进度显示
    QHBoxLayout* progressLayout = new QHBoxLayout();
    m_progressBar = new QProgressBar(this);
    m_usedAmountLabel = new QLabel("已使用: ¥0.00", this);
    m_totalAmountLabel = new QLabel("总预算: ¥0.00", this);
    m_remainingAmountLabel = new QLabel("剩余: ¥0.00", this);
    m_usagePercentageLabel = new QLabel("使用比例: 0%", this);
    
    progressLayout->addWidget(new QLabel("进度:", this));
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_usedAmountLabel);
    progressLayout->addWidget(m_totalAmountLabel);
    progressLayout->addWidget(m_remainingAmountLabel);
    progressLayout->addWidget(m_usagePercentageLabel);
    
    mainLayout->addLayout(progressLayout);
}

void BudgetWidget::createLayout() {
}

void BudgetWidget::createConnections() {
    connect(m_addButton, &QPushButton::clicked, this, &BudgetWidget::onNewBudget);
    connect(m_editButton, &QPushButton::clicked, this, &BudgetWidget::onEditBudget);
    connect(m_deleteButton, &QPushButton::clicked, this, &BudgetWidget::onDeleteBudget);
    connect(m_budgetView, &QTableView::clicked, this, &BudgetWidget::onBudgetSelected);
    connect(m_amountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &BudgetWidget::onBudgetAmountChanged);
    connect(m_alertSlider, &QSlider::valueChanged, this, &BudgetWidget::onAlertThresholdChanged);
    connect(m_periodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BudgetWidget::onPeriodChanged);
    connect(m_saveButton, &QPushButton::clicked, this, &BudgetWidget::onAddBudget);
}

void BudgetWidget::loadCategories() {
    m_categoryCombo->clear();
    if (!m_user) return;
    auto cats = m_user->getTopLevelCategories();
    for (const auto& c : cats) {
        m_categoryCombo->addItem(c->getName(), c->getId());
    }
}

void BudgetWidget::onNewBudget() {
    // prepare the right-side inputs for a new budget
    m_selectedBudget = nullptr;
    if (m_categoryCombo->count() > 0) m_categoryCombo->setCurrentIndex(0);
    m_amountSpinBox->setValue(0.0);
    m_alertSlider->setValue(80);
    if (m_periodCombo->count() > 0) m_periodCombo->setCurrentIndex(0);
    updateBudgetProgress();
}

void BudgetWidget::updateBudgetDisplay() {
}

void BudgetWidget::showBudgetAlert(std::shared_ptr<Budget> budget) {
}

void BudgetWidget::refreshData() {
    loadBudgets();
}

void BudgetWidget::loadBudgets() {
    if (!m_user) return;
    auto budgets = m_user->getAllBudgets();
    m_model->setBudgets(budgets);
    updateBudgetProgress();
    checkBudgetAlerts();
}

void BudgetWidget::onAddBudget() {
    if (!m_user) return;

    QString categoryId = m_categoryCombo->currentData().toString();
    if (categoryId.isEmpty()) {
        // no category selected
        return;
    }

    double amount = m_amountSpinBox->value();
    if (amount <= 0.0) return;

    // If an existing budget is selected, update it
    if (m_selectedBudget) {
        m_selectedBudget->setCategoryId(categoryId);
        m_selectedBudget->setTotalAmount(amount);
        m_selectedBudget->setAlertPercent(m_alertSlider->value() / 100.0);
        QString period = m_periodCombo->currentData().toString();
        if (period == "monthly") m_selectedBudget->setPeriod(Budget::Period::Monthly);
        else if (period == "yearly") m_selectedBudget->setPeriod(Budget::Period::Yearly);
        m_selectedBudget->resetForNewPeriod();
        m_model->updateBudget(m_selectedBudget);
    } else {
        auto budget = std::make_shared<Budget>();
        budget->setCategoryId(categoryId);
        budget->setTotalAmount(amount);
        budget->setAlertPercent(m_alertSlider->value() / 100.0);
        QString period = m_periodCombo->currentData().toString();
        if (period == "monthly") budget->setPeriod(Budget::Period::Monthly);
        else if (period == "yearly") budget->setPeriod(Budget::Period::Yearly);
        budget->resetForNewPeriod();

        m_user->addBudget(budget);
        m_model->addBudget(budget);
    }

    // refresh display
    loadBudgets();
    updateBudgetProgress();
    checkBudgetAlerts();
}

void BudgetWidget::onEditBudget() {
    // Clicking 编辑 will populate right-side inputs with selected budget
    QModelIndex idx = m_budgetView->currentIndex();
    if (!idx.isValid()) return;
    m_selectedBudget = m_model->getBudget(idx.row());
    if (!m_selectedBudget) return;

    // populate
    // find category index
    for (int i = 0; i < m_categoryCombo->count(); ++i) {
        if (m_categoryCombo->itemData(i).toString() == m_selectedBudget->getCategoryId()) {
            m_categoryCombo->setCurrentIndex(i);
            break;
        }
    }
    m_amountSpinBox->setValue(m_selectedBudget->getTotalAmount());
    m_alertSlider->setValue(static_cast<int>(m_selectedBudget->getAlertPercent() * 100));
    // period
    QString p = (m_selectedBudget->getPeriod() == Budget::Period::Yearly) ? "yearly" : "monthly";
    for (int i = 0; i < m_periodCombo->count(); ++i) {
        if (m_periodCombo->itemData(i).toString() == p) {
            m_periodCombo->setCurrentIndex(i);
            break;
        }
    }
}

void BudgetWidget::onDeleteBudget() {
    QModelIndex idx = m_budgetView->currentIndex();
    if (!idx.isValid()) return;
    auto b = m_model->getBudget(idx.row());
    if (!b) return;
    m_user->removeBudget(b->getId());
    m_model->removeBudget(b->getId());
    m_selectedBudget = nullptr;
    loadBudgets();
    updateBudgetProgress();
    checkBudgetAlerts();
}

void BudgetWidget::onBudgetSelected(const QModelIndex& index) {
    if (!index.isValid()) return;
    m_selectedBudget = m_model->getBudget(index.row());
}

void BudgetWidget::onBudgetAmountChanged(double /*amount*/) {
    // no-op for now
}

void BudgetWidget::onAlertThresholdChanged(int value) {
    m_alertLabel->setText(QString("警告阈值: %1%").arg(value));
}

void BudgetWidget::onPeriodChanged(int index) {
    Q_UNUSED(index)
}

void BudgetWidget::onCategorySelected(const QString& categoryId) {
    Q_UNUSED(categoryId)
}

void BudgetWidget::updateBudgetProgress() {
    if (!m_user) return;

    // If a budget is selected, show its detailed progress
    if (m_selectedBudget) {
        double used = m_selectedBudget->getUsedAmount();
        double total = m_selectedBudget->getTotalAmount();
        double remaining = total - used;
        double percent = m_selectedBudget->getUsagePercentage() * 100.0;

        m_progressBar->setValue(static_cast<int>(percent));
        m_usedAmountLabel->setText(QString("已使用: ¥%1").arg(used, 0, 'f', 2));
        m_totalAmountLabel->setText(QString("总预算: ¥%1").arg(total, 0, 'f', 2));
        m_remainingAmountLabel->setText(QString("剩余: ¥%1").arg(remaining, 0, 'f', 2));
        m_usagePercentageLabel->setText(QString("使用比例: %1%").arg(static_cast<int>(percent)));
    } else {
        // otherwise clear / aggregate 0
        m_progressBar->setValue(0);
        m_usedAmountLabel->setText("已使用: ¥0.00");
        m_totalAmountLabel->setText("总预算: ¥0.00");
        m_remainingAmountLabel->setText("剩余: ¥0.00");
        m_usagePercentageLabel->setText("使用比例: 0%");
    }

}

void BudgetWidget::checkBudgetAlerts() {
    if (!m_user) return;

    auto budgets = m_user->getAllBudgets();
    for (const auto& b : budgets) {
        if (!b) continue;
        b->checkAndUpdateStatus();
        if (b->isOverBudget()) {
            auto c = m_user->getCategory(b->getCategoryId());
            QString name = c ? c->getName() : "未知分类";
            emit budgetOver(name, b->getUsedAmount() - b->getTotalAmount());
        } else if (b->isInWarning()) {
            auto c = m_user->getCategory(b->getCategoryId());
            QString name = c ? c->getName() : "未知分类";
            emit budgetWarning(name, b->getUsagePercentage());
        }
    }
}

// BudgetModel 实现
BudgetModel::BudgetModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_user(nullptr)
{
}

int BudgetModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_budgets.size();
}

int BudgetModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant BudgetModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    
    if (index.row() >= m_budgets.size())
        return QVariant();
    
    auto budget = m_budgets[index.row()];
    
    switch (index.column()) {
        case CategoryColumn: {
            QString catId = budget->getCategoryId();
            if (!m_user) return catId;
            auto c = m_user->getCategory(catId);
            return c ? c->getName() : catId;
        }
        case TotalAmountColumn:
            return QString("¥%1").arg(budget->getTotalAmount(), 0, 'f', 2);
        case UsedAmountColumn:
            return QString("¥%1").arg(budget->getUsedAmount(), 0, 'f', 2);
        case RemainingColumn:
            return QString("¥%1").arg(budget->getTotalAmount() - budget->getUsedAmount(), 0, 'f', 2);
        case UsageColumn:
            return QString("%1%").arg(int(budget->getUsagePercentage() * 100));
        case StatusColumn: {
            switch (budget->getStatus()) {
                case Budget::Status::Warning: return "警告";
                case Budget::Status::OverBudget: return "超支";
                case Budget::Status::Active: return "进行中";
                case Budget::Status::Created: return "已创建";
                default: return "正常";
            }
        }
        default:
            return QVariant();
    }
}

QVariant BudgetModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    
    const char* headers[] = {"分类", "总预算", "已使用", "剩余", "使用比例", "状态"};
    if (section < ColumnCount)
        return headers[section];
    
    return QVariant();
}

void BudgetModel::setBudgets(const QVector<std::shared_ptr<Budget>>& budgets) {
    beginResetModel();
    m_budgets = budgets;
    endResetModel();
}

void BudgetModel::addBudget(std::shared_ptr<Budget> budget) {
    beginInsertRows(QModelIndex(), m_budgets.size(), m_budgets.size());
    m_budgets.append(budget);
    endInsertRows();
}

void BudgetModel::setUser(std::shared_ptr<User> user) {
    m_user = user;
}

void BudgetModel::updateBudget(std::shared_ptr<Budget> budget) {
    if (!budget) return;
    for (int i = 0; i < m_budgets.size(); ++i) {
        if (m_budgets[i]->getId() == budget->getId()) {
            m_budgets[i] = budget;
            emit dataChanged(index(i, 0), index(i, ColumnCount - 1));
            return;
        }
    }
}

void BudgetModel::removeBudget(const QString& budgetId) {
    for (int i = 0; i < m_budgets.size(); ++i) {
        if (m_budgets[i]->getId() == budgetId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_budgets.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

std::shared_ptr<Budget> BudgetModel::getBudget(int row) const {
    if (row >= 0 && row < m_budgets.size())
        return m_budgets[row];
    return nullptr;
}

void BudgetModel::clear() {
    beginResetModel();
    m_budgets.clear();
    endResetModel();
}
