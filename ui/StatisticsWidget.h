#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <memory>
#include "../models/User.h"
#include "../services/ReportService.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QDateEdit;
class QPushButton;
class QChartView;
class QLabel;
QT_END_NAMESPACE

class StatisticsWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsWidget(std::shared_ptr<User> user, QWidget *parent = nullptr);
    ~StatisticsWidget();

public slots:
    void refreshData();
    void updateCharts();

private slots:
    void onTimeRangeChanged(int index);
    void onStartDateChanged(const QDate& date);
    void onEndDateChanged(const QDate& date);
    void onChartTypeChanged(int index);
    void onExportChart();
    void onGenerateReport();
    void onReportGenerated(const ReportService::StatisticsData& data);
    void onChartDataReady(const ReportService::ChartData& data);

private:
    void setupUI();
    void createLayout();
    void createConnections();
    void initializeCharts();
    void updateIncomeExpenseChart();
    void updateCategoryChart();
    void updateTrendChart();
    void updateSummaryLabels();
    
    // UI组件
    QComboBox* m_timeRangeCombo;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QComboBox* m_chartTypeCombo;
    QPushButton* m_exportButton;
    QPushButton* m_generateButton;
    
    // 图表视图
    QChartView* m_mainChartView;
    QChartView* m_categoryChartView;
    QChartView* m_trendChartView;
    
    // 统计标签
    QLabel* m_totalIncomeLabel;
    QLabel* m_totalExpenseLabel;
    QLabel* m_balanceLabel;
    QLabel* m_avgDailyExpenseLabel;
    QLabel* m_transactionCountLabel;
    
    // 数据
    std::shared_ptr<User> m_user;
    std::shared_ptr<ReportService> m_reportService;
    ReportService::StatisticsData m_currentStatistics;
};

#endif // STATISTICSWIDGET_H