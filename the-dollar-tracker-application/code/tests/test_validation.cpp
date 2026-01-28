#include <gtest/gtest.h>
#include "../src/core/Validation.h"
#include <QDateTime>

// ------------------------------
// Validation::isValidEmployeeId
// ------------------------------
TEST(ValidationTest, ValidEmployeeIdAccepted) {
    EXPECT_TRUE(Validation::isValidEmployeeId("1234"));
    EXPECT_TRUE(Validation::isValidEmployeeId("25121901"));
}

TEST(ValidationTest, InvalidEmployeeIdRejected) {
    EXPECT_FALSE(Validation::isValidEmployeeId(""));
    EXPECT_FALSE(Validation::isValidEmployeeId("12"));
    EXPECT_FALSE(Validation::isValidEmployeeId("abcd"));
    EXPECT_FALSE(Validation::isValidEmployeeId("123456789"));
}

// ------------------------------
// Validation::isValidShiftTimes
// ------------------------------
TEST(ValidationTest, ValidShiftTimesAccepted) {
    QDateTime start(QDate(2025,1,1), QTime(9,0));
    QDateTime end(QDate(2025,1,1), QTime(17,0));
    EXPECT_TRUE(Validation::isValidShiftTimes(start, end));
}

TEST(ValidationTest, InvalidShiftTimesRejected) {
    QDateTime start(QDate(2025,1,1), QTime(17,0));
    QDateTime end(QDate(2025,1,1), QTime(9,0));
    EXPECT_FALSE(Validation::isValidShiftTimes(start, end));
}
