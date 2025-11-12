# 智能记账本系统

基于C++和Qt开发的个人记账管理系统，完全按照UML设计模式实现。

## 项目概述

这是一个功能完整的个人记账管理系统，采用现代C++设计模式，基于Qt框架开发，具有以下特点：

- **基于UML设计**: 严格按照软件工程UML图设计实现
- **现代化UI**: 采用Qt Widgets和Qt Charts提供优秀的用户体验
- **模块化架构**: 清晰的MVC架构，易于维护和扩展
- **数据安全**: 支持本地数据存储和备份
- **智能提醒**: 预算超支和定期报告提醒功能

## 功能特性

### 核心功能
- ✅ **流水记账**: 记录收入和支出，支持分类管理
- ✅ **多维度统计**: 日/周/月/年统计和图表展示
- ✅ **预算管理**: 设置预算并接收超支提醒
- ✅ **数据查询**: 多条件搜索和筛选
- ✅ **数据导出**: 支持CSV格式导出
- ✅ **数据备份**: 本地数据备份和恢复

### 技术特性
- 🏗️ **UML驱动设计**: 基于类图、用例图、状态图、序列图实现
- 🔧 **现代C++**: 使用C++11/14特性，智能指针管理
- 🎨 **响应式UI**: 支持多主题切换和深色模式
- 📊 **数据可视化**: 使用Qt Charts展示统计图表
- 🔐 **数据安全**: 本地SQLite数据库存储
- ⚡ **性能优化**: 异步数据处理和懒加载

## 项目结构

```
AccountSystem/
├── main.cpp                 # 程序入口
├── MainWindow.h/cpp         # 主窗口类
├── AccountSystem.pro        # Qt项目文件
├── resources.qrc           # 资源文件
├── models/                 # 数据模型层
│   ├── User.h/cpp          # 用户模型（聚合根）
│   ├── Record.h/cpp        # 记账记录模型
│   ├── Category.h/cpp      # 分类模型
│   └── Budget.h/cpp        # 预算模型
├── services/               # 业务逻辑层
│   ├── ReportService.h/cpp # 统计报告服务
│   ├── ReminderService.h/cpp # 提醒服务
│   └── DataStorageService.h/cpp # 数据存储服务
└── ui/                     # 用户界面层
    ├── TransactionWidget.h/cpp   # 流水记账界面
    ├── StatisticsWidget.h/cpp    # 统计分析界面
    ├── BudgetWidget.h/cpp        # 预算管理界面
    ├── SettingsWidget.h/cpp      # 系统设置界面
    └── dialogs/              # 对话框
        ├── AddTransactionDialog.h/cpp
        └── CategoryManagerDialog.h/cpp
```

## 如何根据UML图实现代码

### 1. 基于UML类图的实现

根据您的实验报告中的UML类图，我实现了以下核心类结构：

#### User类（聚合根）
```cpp
class User {
    QString m_id;           // UUID主键
    QString m_name;         // 用户名
    QString m_email;        // 邮箱
    
    // 组合关系：1-N关联
    QVector<std::shared_ptr<Record>> m_records;
    QVector<std::shared_ptr<Category>> m_categories;
    QVector<std::shared_ptr<Budget>> m_budgets;
};
```

**实现说明**：
- 使用`shared_ptr`管理对象生命周期，符合RAII原则
- User作为聚合根，统一管理所有相关实体
- 提供完整的CRUD操作接口

#### Record类（记账流水）
```cpp
class Record {
    enum class Type { Income, Expense };
    enum class Status { Draft, Saved, Modified, Deleted, Restored };
    
    QString m_id;           // UUID主键
    Type m_type;            // 收支类型
    double m_amount;        // 金额
    QString m_categoryId;   // 外键关联Category
    QDateTime m_dateTime;   // 时间字段
    QString m_note;         // 备注
    Status m_status;        // 状态管理
};
```

**实现说明**：
- 状态模式实现：根据UML状态图设计生命周期
- 支持逻辑删除（isDeleted标志）和撤销操作
- 时间戳字段支持审计和同步

#### Budget类（预算管理）
```cpp
class Budget {
    enum class Status { Created, Active, Warning, OverBudget, Normal };
    enum class Period { Monthly, Weekly, Yearly };
    
    QString m_id;
    QString m_categoryId;   // 分类预算模式
    double m_totalAmount;
    double m_usedAmount;
    double m_alertPercent;  // 阈值设置
    Period m_period;        // 预算周期
    Status m_status;        // 状态管理
};
```

**实现说明**：
- 根据UML状态图实现状态转换逻辑
- 支持周期重置和阈值检查
- 状态变化时自动触发提醒

### 2. 基于UML状态图的实现

根据您的多对象状态图，我实现了以下状态管理：

#### Budget状态机
```cpp
void Budget::checkAndUpdateStatus() {
    double usagePercent = getUsagePercentage();
    
    if (usagePercent >= 1.0) {
        m_status = Status::OverBudget;
    } else if (usagePercent >= m_alertPercent) {
        m_status = Status::Warning;
    } else if (usagePercent > 0) {
        m_status = Status::Active;
    } else {
        m_status = Status::Created;
    }
}
```

**实现说明**：
- 严格按照状态图的状态转换逻辑
- 状态变化时自动触发相应的entry/exit动作
- 支持双向状态转换（正常↔警告↔超支）

#### Record状态机
```cpp
enum class Status {
    Draft,      // 草稿状态
    Saved,      // 已保存
    Modified,   // 已修改
    Deleted,    // 已删除（逻辑删除）
    Restored    // 已还原
};
```

**实现说明**：
- 支持完整的生命周期管理
- 逻辑删除支持撤销操作
- 修改时记录时间戳用于冲突检测

### 3. 基于UML序列图的实现

根据您的序列图"记录支出→预算检查→弹窗提醒"，我实现了以下交互逻辑：

```cpp
// 1. 记录支出（RecordService完成落库）
void User::addRecord(std::shared_ptr<Record> record) {
    m_records.append(record);
    record->setStatus(Record::Status::Saved);
    
    // 2. 异步调用BudgetService进行阈值计算
    checkBudgetStatus(record);
}

// 2. 预算检查
void ReminderService::checkBudgetStatus() {
    for (const auto& budget : m_user->getAllBudgets()) {
        budget->checkAndUpdateStatus();
        
        // 3. 当且仅当达到阈值时构造弹窗数据
        if (shouldSendBudgetWarning(budget)) {
            // 4. 回调通知前端
            emit budgetWarning(categoryName, usagePercent);
        }
    }
}
```

**实现说明**：
- 严格按照"前端→服务→数据库"分层架构
- 异步处理避免阻塞UI
- 回调通知机制替代轮询

### 4. 基于UML用例图的实现

根据您的用例图，我实现了以下用例：

#### 核心用例映射
- **UC1 记录收/支** ← TransactionWidget + Record模型
- **UC2 管理分类** ← CategoryManagerDialog + Category模型  
- **UC3 查询搜索** ← TransactionWidget的搜索和筛选功能
- **UC4 查看统计** ← StatisticsWidget + ReportService
- **UC5 设置预算** ← BudgetWidget + Budget模型
- **UC6 接收提醒** ← ReminderService + 系统通知

#### 扩展关系实现
```cpp
// «include»关系：统计必含提醒
void StatisticsWidget::updateCharts() {
    generateReport();
    // 检查预算状态，确保"看图→预警"闭环
    m_reminderService->checkBudgetStatus();
}

// «extend»关系：导出为可选功能
void StatisticsWidget::onExportChart() {
    // 仅在用户主动触发时执行
    // 不破坏主流程完整性
}
```

### 5. 架构设计原则

#### 分层架构
```
UI层 (Widgets) 
    ↓ 信号/槽
服务层 (Services)
    ↓ 调用
模型层 (Models)
    ↓ 持久化
数据层 (SQLite)
```

#### 设计模式应用
- **观察者模式**: Qt信号槽机制实现事件通知
- **单例模式**: 服务类确保全局唯一实例
- **工厂模式**: 模型对象的创建和管理
- **策略模式**: 不同的图表生成策略

#### 依赖注入
```cpp
// 服务类依赖注入User对象
explicit ReportService(std::shared_ptr<User> user, QObject *parent = nullptr);
```

## 编译和运行

### 环境要求
- Qt 5.12 或更高版本
- C++11 兼容的编译器
- SQLite3 数据库支持

### 编译步骤
```bash
# 1. 生成Makefile
qmake AccountSystem.pro

# 2. 编译项目
make

# 3. 运行程序
./AccountSystem
```

### Windows用户
```bash
# 使用Qt Creator打开AccountSystem.pro文件
# 点击构建和运行按钮
```

## 使用说明

### 首次使用
1. 启动程序后，系统会自动创建默认用户
2. 在"流水记账"页面添加第一条记录
3. 在"预算管理"页面设置月度预算
4. 在"统计分析"页面查看收支情况

### 主要操作
- **添加记录**: 点击"+"按钮或使用Ctrl+N
- **编辑记录**: 双击记录或选择后按Ctrl+E
- **删除记录**: 选择记录后按Delete键
- **搜索筛选**: 使用顶部的搜索框和分类过滤器
- **数据导出**: 在设置页面可以导出CSV格式数据

## 数据存储

### 数据库结构
系统使用SQLite数据库存储数据，主要表结构：
- `users`: 用户信息表
- `records`: 记账记录表
- `categories`: 分类信息表
- `budgets`: 预算信息表

### 备份恢复
- 自动备份: 可设置定期自动备份
- 手动备份: 在设置页面执行手动备份
- 数据恢复: 选择备份文件进行数据恢复

## 扩展开发

### 添加新功能
1. 在对应的模型类中添加数据结构和逻辑
2. 在服务层实现业务逻辑
3. 在UI层添加界面和交互
4. 更新数据库结构（如需要）

### 自定义主题
1. 修改`resources.qrc`中的样式文件
2. 在`SettingsWidget`中添加主题选项
3. 实现主题切换逻辑

## 技术亮点

1. **UML驱动开发**: 严格按照软件工程规范设计
2. **现代C++特性**: 智能指针、lambda表达式、auto关键字
3. **Qt最佳实践**: 信号槽、模型视图、资源系统
4. **响应式设计**: 异步处理、非阻塞UI
5. **数据安全**: 事务处理、备份机制

## 许可证

本项目仅供学习和研究使用，基于您的实验报告需求开发。

## 致谢

感谢Qt框架提供的优秀开发工具，以及您详细的UML设计文档，为本项目的实现提供了清晰的指导。