#include "ReportService.h"
#include <QDate>
#include <QDebug>

ReportService::ReportService(std::shared_ptr<User> user, QObject *parent)
    : QObject(parent)
    , m_user(user) {
}

ReportService::StatisticsData ReportService::generateStatistics(const QDate& startDate, const QDate& endDate) {
    StatisticsData data;
    
    // 获取基础统计数据
    data.totalIncome = m_user->getTotalIncome(startDate, endDate);
    data.totalExpense = m_user->getTotalExpense(startDate, endDate);
    data.balance = data.totalIncome - data.totalExpense;
    data.avgDailyExpense = calculateAverageDailyExpense(startDate, endDate);
    
    // 获取分类统计数据
    data.categoryExpenses = getCategoryExpenseDistribution(startDate, endDate);
    data.categoryIncomes = getCategoryIncomeDistribution(startDate, endDate);
    
    // 获取交易数量
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    data.transactionCount = records.size();
    
    emit reportGenerated(data);
    return data;
}

ReportService::ChartData ReportService::generateChartData(ReportType type, TimeDimension dimension, 
                                                         const QDate& startDate, const QDate& endDate) {
    ChartData chartData;
    
    switch (type) {
        case ReportType::IncomeExpense:
            chartData.title = "收支分析";
            chartData.unit = "元";
            chartData.values = {m_user->getTotalIncome(startDate, endDate), 
                               m_user->getTotalExpense(startDate, endDate)};
            chartData.labels = {"收入", "支出"};
            chartData.colors = {QColor("#27AE60"), QColor("#E74C3C")};
            break;
            
        case ReportType::CategoryAnalysis:
            chartData.title = "分类统计";
            chartData.unit = "元";
            {
                auto categoryData = getCategoryExpenseDistribution(startDate, endDate);
                for (auto it = categoryData.begin(); it != categoryData.end(); ++it) {
                    chartData.values.append(it.value());
                    chartData.labels.append(it.key());
                }
            }
            break;
            
        case ReportType::TrendAnalysis:
            chartData.title = "趋势分析";
            chartData.unit = "元";
            {
                auto trendData = getExpenseTrend(startDate, endDate);
                for (auto it = trendData.begin(); it != trendData.end(); ++it) {
                    chartData.values.append(it.value());
                    chartData.labels.append(it.key().toString("MM-dd"));
                }
            }
            break;
            
        default:
            break;
    }
    
    emit chartDataReady(chartData);
    return chartData;
}

QMap<QDate, double> ReportService::getExpenseTrend(const QDate& startDate, const QDate& endDate) {
    QMap<QDate, double> trendData;
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    
    // 按日期分组统计支出
    for (const auto& record : records) {
        if (record->isExpense()) {
            QDate date = record->getDateTime().date();
            trendData[date] += record->getAmount();
        }
    }
    
    return trendData;
}

QMap<QDate, double> ReportService::getIncomeTrend(const QDate& startDate, const QDate& endDate) {
    QMap<QDate, double> trendData;
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    
    // 按日期分组统计收入
    for (const auto& record : records) {
        if (record->isIncome()) {
            QDate date = record->getDateTime().date();
            trendData[date] += record->getAmount();
        }
    }
    
    return trendData;
}

QMap<QString, double> ReportService::getCategoryExpenseDistribution(const QDate& startDate, const QDate& endDate) {
    QMap<QString, double> categoryExpenses;
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    
    // 按分类统计支出
    for (const auto& record : records) {
        if (record->isExpense()) {
            QString categoryId = record->getCategoryId();
            auto category = m_user->getCategory(categoryId);
            QString categoryName = category ? category->getName() : "未知分类";
            categoryExpenses[categoryName] += record->getAmount();
        }
    }
    
    return categoryExpenses;
}

QMap<QString, double> ReportService::getCategoryIncomeDistribution(const QDate& startDate, const QDate& endDate) {
    QMap<QString, double> categoryIncomes;
    auto records = m_user->getRecordsByDateRange(startDate, endDate);
    
    // 按分类统计收入
    for (const auto& record : records) {
        if (record->isIncome()) {
            QString categoryId = record->getCategoryId();
            auto category = m_user->getCategory(categoryId);
            QString categoryName = category ? category->getName() : "未知分类";
            categoryIncomes[categoryName] += record->getAmount();
        }
    }
    
    return categoryIncomes;
}

QMap<QString, double> ReportService::getBudgetUsageReport(const QDate& date) {
    QMap<QString, double> budgetUsage;
    auto budgets = m_user->getAllBudgets();
    
    // 计算每个预算的使用情况
    for (const auto& budget : budgets) {
        if (budget->getStartDate() <= date && budget->getEndDate() >= date) {
            auto category = m_user->getCategory(budget->getCategoryId());
            QString categoryName = category ? category->getName() : "未知分类";
            budgetUsage[categoryName] = budget->getUsagePercentage();
        }
    }
    
    return budgetUsage;
}

double ReportService::calculateAverageDailyExpense(const QDate& startDate, const QDate& endDate) {
    int days = getDaysInPeriod(startDate, endDate);
    if (days <= 0) return 0.0;
    
    double totalExpense = m_user->getTotalExpense(startDate, endDate);
    return totalExpense / days;
}

int ReportService::getDaysInPeriod(const QDate& startDate, const QDate& endDate) {
    return startDate.daysTo(endDate) + 1;
}