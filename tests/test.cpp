#include <gtest/gtest.h>
#include "../models/Record.h"
#include "../models/Category.h"
#include <QDateTime>

// Record类测试
TEST(RecordTest, Constructor_Default) {
    Record record;
    EXPECT_FALSE(record.getId().isEmpty());
    EXPECT_EQ(record.getType(), Record::Type::Expense);
    EXPECT_EQ(record.getAmount(), 0.0);
    EXPECT_EQ(record.getStatus(), Record::Status::Draft);
    EXPECT_FALSE(record.getNote().isEmpty()); // Assuming note is initialized
    EXPECT_FALSE(record.getCategoryId().isEmpty()); // Assuming categoryId is initialized
}

TEST(RecordTest, Constructor_WithId) {
    QString id = "test_id";
    Record record(id);
    EXPECT_EQ(record.getId(), id);
}

TEST(RecordTest, SetAndGetType) {
    Record record;
    record.setType(Record::Type::Income);
    EXPECT_EQ(record.getType(), Record::Type::Income);
    record.setType(Record::Type::Expense);
    EXPECT_EQ(record.getType(), Record::Type::Expense);
}

TEST(RecordTest, SetAndGetAmount) {
    Record record;
    record.setAmount(100.50);
    EXPECT_EQ(record.getAmount(), 100.50);
    record.setAmount(-50.0);
    EXPECT_EQ(record.getAmount(), -50.0);
    record.setAmount(0.0);
    EXPECT_EQ(record.getAmount(), 0.0);
}

TEST(RecordTest, SetAndGetCategoryId) {
    Record record;
    QString categoryId = "cat_001";
    record.setCategoryId(categoryId);
    EXPECT_EQ(record.getCategoryId(), categoryId);
}

TEST(RecordTest, SetAndGetDateTime) {
    Record record;
    QDateTime dt = QDateTime::currentDateTime();
    record.setDateTime(dt);
    EXPECT_EQ(record.getDateTime(), dt);
}

TEST(RecordTest, SetAndGetNote) {
    Record record;
    QString note = "Test note";
    record.setNote(note);
    EXPECT_EQ(record.getNote(), note);
}

TEST(RecordTest, SetAndGetStatus) {
    Record record;
    record.setStatus(Record::Status::Saved);
    EXPECT_EQ(record.getStatus(), Record::Status::Saved);
    record.setStatus(Record::Status::Deleted);
    EXPECT_EQ(record.getStatus(), Record::Status::Deleted);
}

TEST(RecordTest, IsDeleted) {
    Record record;
    EXPECT_FALSE(record.isDeleted());
    record.setStatus(Record::Status::Deleted);
    EXPECT_TRUE(record.isDeleted());
}

TEST(RecordTest, MarkAsDeleted) {
    Record record;
    record.markAsDeleted();
    EXPECT_EQ(record.getStatus(), Record::Status::Deleted);
    EXPECT_TRUE(record.isDeleted());
}

TEST(RecordTest, GetTypeString) {
    Record record;
    record.setType(Record::Type::Income);
    EXPECT_EQ(record.getTypeString(), "收入");
    record.setType(Record::Type::Expense);
    EXPECT_EQ(record.getTypeString(), "支出");
}

TEST(RecordTest, Boundary_Amount_Zero) {
    Record record;
    record.setAmount(0.0);
    EXPECT_EQ(record.getAmount(), 0.0);
}

TEST(RecordTest, Boundary_Amount_Negative) {
    Record record;
    record.setAmount(-1000.0);
    EXPECT_EQ(record.getAmount(), -1000.0);
}

TEST(RecordTest, Boundary_Amount_Large) {
    Record record;
    record.setAmount(1e10);
    EXPECT_EQ(record.getAmount(), 1e10);
}

// Category类测试
TEST(CategoryTest, Constructor_Default) {
    Category category;
    EXPECT_FALSE(category.getId().isEmpty());
    EXPECT_TRUE(category.getName().isEmpty());
    EXPECT_TRUE(category.getIcon().isEmpty());
    EXPECT_TRUE(category.getColor().isEmpty());
    EXPECT_TRUE(category.getParentId().isEmpty());
    EXPECT_FALSE(category.isIncomeCategory());
    EXPECT_EQ(category.getSortOrder(), 0);
    EXPECT_TRUE(category.isTopLevel());
    EXPECT_FALSE(category.hasSubCategories());
}

TEST(CategoryTest, Constructor_WithId) {
    QString id = "cat_id";
    Category category(id);
    EXPECT_EQ(category.getId(), id);
}

TEST(CategoryTest, SetAndGetName) {
    Category category;
    QString name = "Food";
    category.setName(name);
    EXPECT_EQ(category.getName(), name);
}

TEST(CategoryTest, SetAndGetIcon) {
    Category category;
    QString icon = ":/icons/food.png";
    category.setIcon(icon);
    EXPECT_EQ(category.getIcon(), icon);
}

TEST(CategoryTest, SetAndGetColor) {
    Category category;
    QString color = "#FF0000";
    category.setColor(color);
    EXPECT_EQ(category.getColor(), color);
}

TEST(CategoryTest, SetAndGetParentId) {
    Category category;
    QString parentId = "parent_cat";
    category.setParentId(parentId);
    EXPECT_EQ(category.getParentId(), parentId);
    EXPECT_FALSE(category.isTopLevel());
}

TEST(CategoryTest, SetAndGetIsIncomeCategory) {
    Category category;
    category.setIsIncomeCategory(true);
    EXPECT_TRUE(category.isIncomeCategory());
    category.setIsIncomeCategory(false);
    EXPECT_FALSE(category.isIncomeCategory());
}

TEST(CategoryTest, SetAndGetSortOrder) {
    Category category;
    category.setSortOrder(5);
    EXPECT_EQ(category.getSortOrder(), 5);
}

TEST(CategoryTest, AddSubCategory) {
    Category category;
    QString subId = "sub_cat";
    category.addSubCategory(subId);
    EXPECT_TRUE(category.getSubCategoryIds().contains(subId));
    EXPECT_TRUE(category.hasSubCategories());
}

TEST(CategoryTest, AddSubCategory_Duplicate) {
    Category category;
    QString subId = "sub_cat";
    category.addSubCategory(subId);
    category.addSubCategory(subId);
    EXPECT_EQ(category.getSubCategoryIds().size(), 1);
}

TEST(CategoryTest, RemoveSubCategory) {
    Category category;
    QString subId = "sub_cat";
    category.addSubCategory(subId);
    category.removeSubCategory(subId);
    EXPECT_FALSE(category.getSubCategoryIds().contains(subId));
    EXPECT_FALSE(category.hasSubCategories());
}

TEST(CategoryTest, RemoveSubCategory_NonExistent) {
    Category category;
    category.removeSubCategory("non_existent");
    EXPECT_TRUE(category.getSubCategoryIds().isEmpty());
}

TEST(CategoryTest, GetDisplayPath_TopLevel) {
    Category category;
    category.setName("Food");
    EXPECT_EQ(category.getDisplayPath(), "Food");
}

TEST(CategoryTest, GetDisplayPath_WithParent) {
    Category category;
    category.setName("Lunch");
    category.setParentId("Food");
    EXPECT_EQ(category.getDisplayPath(), "Food > Lunch");
}

TEST(CategoryTest, Boundary_Name_Empty) {
    Category category;
    category.setName("");
    EXPECT_TRUE(category.getName().isEmpty());
}

TEST(CategoryTest, Boundary_Name_Long) {
    Category category;
    QString longName(1000, 'A');
    category.setName(longName);
    EXPECT_EQ(category.getName(), longName);
}

TEST(CategoryTest, Boundary_SortOrder_Negative) {
    Category category;
    category.setSortOrder(-1);
    EXPECT_EQ(category.getSortOrder(), -1);
}

TEST(CategoryTest, Boundary_SortOrder_Large) {
    Category category;
    category.setSortOrder(10000);
    EXPECT_EQ(category.getSortOrder(), 10000);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}