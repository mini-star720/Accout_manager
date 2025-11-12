#ifndef REPORTSERVICE_H
#define REPORTSERVICE_H

#include <QObject>
#include <QDate>
#include <QVector>
#include <QMap>
#include <QColor>
#include <memory>
#include "../models/User.h"
#include "../models/Record.h"

class ReportService : public QObject {
    Q_OBJECT

public:
    enum class TimeDimension {
        Daily,
        Weekly,
        Monthly,
        Yearly
    };
    
    enum class ReportType {
        IncomeExpense,
        CategoryAnalysis,
        TrendAnalysis,
        BudgetPerformance
    };
    
    struct ChartData {
        QVector<double> values;
        QVector<QString> labels;
        QVector<QColor> colors;
        QString title;
        QString unit;
    };
    
    struct StatisticsData {
        double totalIncome;
        double totalExpense;
        double balance;
        double avgDailyExpense;
        QMap<QString, double> categoryExpenses;
        QMap<QString, double> categoryIncomes;
        int transactionCount;
    };
    
    explicit ReportService(std::shared_ptr<User> user, QObject *parent = nullptr);
    
    // 生成统计报告
    StatisticsData generateStatistics(const QDate& startDate, const QDate& endDate);
    
    // 生成图表数据
    ChartData generateChartData(ReportType type, TimeDimension dimension, 
                               const QDate& startDate, const QDate& endDate);
    
    // 趋势分析
    QMap<QDate, double> getExpenseTrend(const QDate& startDate, const QDate& endDate);
    QMap<QDate, double> getIncomeTrend(const QDate& startDate, const QDate& endDate);
    
    // 分类分析
    QMap<QString, double> getCategoryExpenseDistribution(const QDate& startDate, const QDate& endDate);
    QMap<QString, double> getCategoryIncomeDistribution(const QDate& startDate, const QDate& endDate);
    
    // 预算执行情况
    QMap<QString, double> getBudgetUsageReport(const QDate& date);
    
signals:
    void reportGenerated(const StatisticsData& data);
    void chartDataReady(const ChartData& data);
    void reportGenerationFailed(const QString& error);

private:
    std::shared_ptr<User> m_user;
    
    // 辅助方法
    QMap<QDate, QVector<std::shared_ptr<Record>>> groupRecordsByDate(
        const QVector<std::shared_ptr<Record>>& records, TimeDimension dimension);
    
    double calculateAverageDailyExpense(const QDate& startDate, const QDate& endDate);
    int getDaysInPeriod(const QDate& startDate, const QDate& endDate);
};

#endif // REPORTSERVICE_H