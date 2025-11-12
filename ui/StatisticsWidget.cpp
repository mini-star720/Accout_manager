#include "StatisticsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QChartView>
#include <QChart>
#include <QPieSeries>
#include <QBarSeries>
#include <QLineSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>

StatisticsWidget::StatisticsWidget(std::shared_ptr<User> user, QWidget *parent)
    : QWidget(parent)
    , m_user(user)
{
    setupUI();
    createConnections();
    
    // 创建报告服务
    m_reportService = std::make_shared<ReportService>(m_user);
    connect(m_reportService.get(), &ReportService::reportGenerated, 
            this, &StatisticsWidget::onReportGenerated);
    connect(m_reportService.get(), &ReportService::chartDataReady, 
            this, &StatisticsWidget::onChartDataReady);
}

StatisticsWidget::~StatisticsWidget() {
}

void StatisticsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 控制面板
    QGroupBox* controlGroup = new QGroupBox("统计设置");
    QFormLayout* controlLayout = new QFormLayout(controlGroup);
    
    // 时间范围选择
    m_timeRangeCombo = new QComboBox();
    m_timeRangeCombo->addItems({"本月", "上月", "本季度", "本年", "自定义"});
    controlLayout->addRow("时间范围:", m_timeRangeCombo);
    
    // 自定义日期范围
    QHBoxLayout* dateLayout = new QHBoxLayout();
    m_startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    m_endDateEdit = new QDateEdit(QDate::currentDate());
    m_startDateEdit->setCalendarPopup(true);
    m_endDateEdit->setCalendarPopup(true);
    dateLayout->addWidget(new QLabel("开始:"));
    dateLayout->addWidget(m_startDateEdit);
    dateLayout->addWidget(new QLabel("结束:"));
    dateLayout->addWidget(m_endDateEdit);
    controlLayout->addRow("自定义日期:", dateLayout);
    
    // 图表类型选择
    m_chartTypeCombo = new QComboBox();
    m_chartTypeCombo->addItems({"收支分析", "分类统计", "趋势分析", "预算执行"});
    controlLayout->addRow("图表类型:", m_chartTypeCombo);
    
    // 操作按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_generateButton = new QPushButton("生成报告");
    m_exportButton = new QPushButton("导出图表");
    buttonLayout->addWidget(m_generateButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addStretch();
    controlLayout->addRow("", buttonLayout);
    
    mainLayout->addWidget(controlGroup);
    
    // 统计摘要
    QGroupBox* summaryGroup = new QGroupBox("统计摘要");
    QFormLayout* summaryLayout = new QFormLayout(summaryGroup);
    
    m_totalIncomeLabel = new QLabel("¥0.00");
    m_totalIncomeLabel->setStyleSheet("QLabel { color: #27AE60; font-weight: bold; }");
    summaryLayout->addRow("总收入:", m_totalIncomeLabel);
    
    m_totalExpenseLabel = new QLabel("¥0.00");
    m_totalExpenseLabel->setStyleSheet("QLabel { color: #E74C3C; font-weight: bold; }");
    summaryLayout->addRow("总支出:", m_totalExpenseLabel);
    
    m_balanceLabel = new QLabel("¥0.00");
    m_balanceLabel->setStyleSheet("QLabel { color: #3498DB; font-weight: bold; }");
    summaryLayout->addRow("净余额:", m_balanceLabel);
    
    m_avgDailyExpenseLabel = new QLabel("¥0.00");
    summaryLayout->addRow("日均支出:", m_avgDailyExpenseLabel);
    
    m_transactionCountLabel = new QLabel("0");
    summaryLayout->addRow("交易笔数:", m_transactionCountLabel);
    
    mainLayout->addWidget(summaryGroup);
    
    // 图表显示区域
    QGroupBox* chartGroup = new QGroupBox("数据图表");
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    
    m_mainChartView = new QChartView();
    m_mainChartView->setRenderHint(QPainter::Antialiasing);
    chartLayout->addWidget(m_mainChartView);
    
    mainLayout->addWidget(chartGroup);
    
    setLayout(mainLayout);
}

void StatisticsWidget::createConnections() {
    connect(m_timeRangeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsWidget::onTimeRangeChanged);
    connect(m_startDateEdit, &QDateEdit::dateChanged,
            this, &StatisticsWidget::onStartDateChanged);
    connect(m_endDateEdit, &QDateEdit::dateChanged,
            this, &StatisticsWidget::onEndDateChanged);
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsWidget::onChartTypeChanged);
    connect(m_generateButton, &QPushButton::clicked,
            this, &StatisticsWidget::onGenerateReport);
    connect(m_exportButton, &QPushButton::clicked,
            this, &StatisticsWidget::onExportChart);
}

void StatisticsWidget::refreshData() {
    updateCharts();
    updateSummaryLabels();
}

void StatisticsWidget::updateCharts() {
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    // 根据选择的图表类型生成不同的图表
    int chartType = m_chartTypeCombo->currentIndex();
    
    switch (chartType) {
        case 0: // 收支分析
            updateIncomeExpenseChart();
            break;
        case 1: // 分类统计
            updateCategoryChart();
            break;
        case 2: // 趋势分析
            updateTrendChart();
            break;
        default:
            updateIncomeExpenseChart();
    }
}

void StatisticsWidget::updateIncomeExpenseChart() {
    QChart* chart = new QChart();
    chart->setTitle("收支分析");
    
    QPieSeries* series = new QPieSeries();
    
    // 获取数据并添加到图表
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    double totalIncome = m_user->getTotalIncome(startDate, endDate);
    double totalExpense = m_user->getTotalExpense(startDate, endDate);
    
    if (totalIncome > 0) {
        series->append("收入", totalIncome);
    }
    if (totalExpense > 0) {
        series->append("支出", totalExpense);
    }
    
    // 设置颜色
    if (series->count() > 0) {
        series->slices().at(0)->setColor(QColor("#27AE60")); // 绿色 - 收入
    }
    if (series->count() > 1) {
        series->slices().at(1)->setColor(QColor("#E74C3C")); // 红色 - 支出
    }
    
    chart->addSeries(series);
    chart->legend()->setAlignment(Qt::AlignRight);
    
    m_mainChartView->setChart(chart);
}

void StatisticsWidget::updateCategoryChart() {
    QChart* chart = new QChart();
    chart->setTitle("分类支出统计");
    
    QPieSeries* series = new QPieSeries();
    
    // 获取分类支出数据
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    auto categoryExpenses = m_reportService->getCategoryExpenseDistribution(startDate, endDate);
    
    QVector<QColor> colors = {
        QColor("#FF6B35"), QColor("#4A90E2"), QColor("#9013FE"), 
        QColor("#27AE60"), QColor("#F5A623"), QColor("#D0021B"),
        QColor("#7ED321"), QColor("#BD10E0"), QColor("#50E3C2")
    };
    
    int colorIndex = 0;
    for (auto it = categoryExpenses.begin(); it != categoryExpenses.end(); ++it) {
        if (it.value() > 0) {
            auto slice = series->append(it.key(), it.value());
            slice->setColor(colors[colorIndex % colors.size()]);
            colorIndex++;
        }
    }
    
    chart->addSeries(series);
    chart->legend()->setAlignment(Qt::AlignRight);
    
    m_mainChartView->setChart(chart);
}

void StatisticsWidget::updateTrendChart() {
    QChart* chart = new QChart();
    chart->setTitle("收支趋势分析");
    
    // 创建折线图
    QLineSeries* incomeSeries = new QLineSeries();
    incomeSeries->setName("收入");
    incomeSeries->setColor(QColor("#27AE60"));
    
    QLineSeries* expenseSeries = new QLineSeries();
    expenseSeries->setName("支出");
    expenseSeries->setColor(QColor("#E74C3C"));
    
    // 获取趋势数据
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    auto incomeTrend = m_reportService->getIncomeTrend(startDate, endDate);
    auto expenseTrend = m_reportService->getExpenseTrend(startDate, endDate);
    
    // 添加数据点
    for (auto it = incomeTrend.begin(); it != incomeTrend.end(); ++it) {
        incomeSeries->append(it.key().toJulianDay(), it.value());
    }
    
    for (auto it = expenseTrend.begin(); it != expenseTrend.end(); ++it) {
        expenseSeries->append(it.key().toJulianDay(), it.value());
    }
    
    chart->addSeries(incomeSeries);
    chart->addSeries(expenseSeries);
    
    // 设置坐标轴
    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("日期");
    chart->addAxis(axisX, Qt::AlignBottom);
    
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("金额 (¥)");
    chart->addAxis(axisY, Qt::AlignLeft);
    
    incomeSeries->attachAxis(axisX);
    incomeSeries->attachAxis(axisY);
    expenseSeries->attachAxis(axisX);
    expenseSeries->attachAxis(axisY);
    
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_mainChartView->setChart(chart);
}

void StatisticsWidget::updateSummaryLabels() {
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    double totalIncome = m_user->getTotalIncome(startDate, endDate);
    double totalExpense = m_user->getTotalExpense(startDate, endDate);
    double balance = totalIncome - totalExpense;
    
    m_totalIncomeLabel->setText(QString("¥%1").arg(totalIncome, 0, 'f', 2));
    m_totalExpenseLabel->setText(QString("¥%1").arg(totalExpense, 0, 'f', 2));
    
    if (balance >= 0) {
        m_balanceLabel->setText(QString("¥%1").arg(balance, 0, 'f', 2));
        m_balanceLabel->setStyleSheet("QLabel { color: #27AE60; font-weight: bold; }");
    } else {
        m_balanceLabel->setText(QString("-¥%1").arg(-balance, 0, 'f', 2));
        m_balanceLabel->setStyleSheet("QLabel { color: #E74C3C; font-weight: bold; }");
    }
    
    // 计算日均支出
    int days = startDate.daysTo(endDate) + 1;
    double avgDailyExpense = days > 0 ? totalExpense / days : 0;
    m_avgDailyExpenseLabel->setText(QString("¥%1").arg(avgDailyExpense, 0, 'f', 2));
    
    // 获取交易笔数
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    m_transactionCountLabel->setText(QString::number(records.size()));
}

void StatisticsWidget::onTimeRangeChanged(int index) {
    QDate currentDate = QDate::currentDate();
    
    switch (index) {
        case 0: // 本月
            m_startDateEdit->setDate(QDate(currentDate.year(), currentDate.month(), 1));
            m_endDateEdit->setDate(currentDate);
            break;
        case 1: // 上月
            m_startDateEdit->setDate(QDate(currentDate.year(), currentDate.month() - 1, 1));
            m_endDateEdit->setDate(QDate(currentDate.year(), currentDate.month(), 1).addDays(-1));
            break;
        case 2: // 本季度
            {
                int quarter = (currentDate.month() - 1) / 3 + 1;
                int startMonth = (quarter - 1) * 3 + 1;
                m_startDateEdit->setDate(QDate(currentDate.year(), startMonth, 1));
                m_endDateEdit->setDate(currentDate);
            }
            break;
        case 3: // 本年
            m_startDateEdit->setDate(QDate(currentDate.year(), 1, 1));
            m_endDateEdit->setDate(currentDate);
            break;
        case 4: // 自定义
            // 保持当前选择的日期
            break;
    }
    
    updateCharts();
    updateSummaryLabels();
}

void StatisticsWidget::onStartDateChanged(const QDate& date) {
    Q_UNUSED(date)
    updateCharts();
    updateSummaryLabels();
}

void StatisticsWidget::onEndDateChanged(const QDate& date) {
    Q_UNUSED(date)
    updateCharts();
    updateSummaryLabels();
}

void StatisticsWidget::onChartTypeChanged(int index) {
    Q_UNUSED(index)
    updateCharts();
}

void StatisticsWidget::onExportChart() {
    // 导出图表功能
    QMessageBox::information(this, "导出功能", "图表导出功能开发中...");
}

void StatisticsWidget::onGenerateReport() {
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    ReportService::StatisticsData data = m_reportService->generateStatistics(startDate, endDate);
    onReportGenerated(data);
    
    // 同时生成图表数据
    ReportService::ChartData chartData = m_reportService->generateChartData(
        ReportService::ReportType::IncomeExpense, 
        ReportService::TimeDimension::Monthly, 
        startDate, endDate);
    onChartDataReady(chartData);
}

void StatisticsWidget::onReportGenerated(const ReportService::StatisticsData& data) {
    m_currentStatistics = data;
    updateSummaryLabels();
}

void StatisticsWidget::onChartDataReady(const ReportService::ChartData& data) {
    // 使用图表数据更新显示
    Q_UNUSED(data)
}