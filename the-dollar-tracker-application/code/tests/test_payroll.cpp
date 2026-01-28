#include <gtest/gtest.h>
#include <QString>

#include "../src/core/Payroll.h"

// Helper to find a line by empId
static const Payroll::Line* findLine(const std::vector<Payroll::Line>& lines,
                                     const QString& id)
{
    for (const auto& l : lines) {
        if (l.empId == id)
            return &l;
    }
    return nullptr;
}

TEST(PayrollTest, ComputesRegularHoursAndPay) {
    QString ids =
        "emp312, Service$20/H/\n";

    // Two 8-hour shifts = 16h
    QString logs =
        "emp312, 2025.11.01 -09:00:00/17:00:00\n"
        "emp312, 2025.11.02 -09:00:00/17:00:00\n";

    auto lines = Payroll::compute(ids, logs);   // default 40h overtime threshold
    const Payroll::Line* l = findLine(lines, "emp312");
    ASSERT_NE(l, nullptr);

    EXPECT_DOUBLE_EQ(l->hours, 16.0);
    EXPECT_DOUBLE_EQ(l->regularHours, 16.0);
    EXPECT_DOUBLE_EQ(l->overtimeHours, 0.0);
    EXPECT_NEAR(l->pay, 16.0 * 20.0, 1e-6);    // 320
}

TEST(PayrollTest, ComputesOvertimeAboveThreshold) {
    QString ids =
        "emp312, Service$10/H/\n";

    // 5 * 8h = 40h + 1h = 41h total
    QString logs;
    for (int day = 1; day <= 5; ++day) {
        logs += QString("emp312, 2025.11.%1 -09:00:00/17:00:00\n")
        .arg(day, 2, 10, QChar('0'));
    }
    logs += "emp312, 2025.11.06 -09:00:00/10:00:00\n";

    auto lines = Payroll::compute(ids, logs, 40.0, 1.5);
    const Payroll::Line* l = findLine(lines, "emp312");
    ASSERT_NE(l, nullptr);

    EXPECT_DOUBLE_EQ(l->hours, 41.0);
    EXPECT_DOUBLE_EQ(l->regularHours, 40.0);
    EXPECT_DOUBLE_EQ(l->overtimeHours, 1.0);

    // regular = 40 * 10 = 400
    // overtime = 1 * 10 * 1.5 = 15
    EXPECT_NEAR(l->overtimePay, 15.0, 1e-6);
    EXPECT_NEAR(l->pay, 415.0, 1e-6);
}

TEST(PayrollTest, FlatRateRolePaysPerShift) {
    QString ids =
        "emp999, Flat$150/F/\n";

    QString logs =
        "emp999, 2025.11.01 -09:00:00/10:00:00\n"
        "emp999, 2025.11.02 -09:00:00/11:00:00\n";

    auto lines = Payroll::compute(ids, logs);
    const Payroll::Line* l = findLine(lines, "emp999");
    ASSERT_NE(l, nullptr);

    // two closed sessions, each 150 flat
    EXPECT_NEAR(l->pay, 300.0, 1e-6);
    EXPECT_FALSE(l->hourly);
}
