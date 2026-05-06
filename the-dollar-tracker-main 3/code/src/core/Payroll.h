#pragma once

#include <QString>
#include <QDate>
#include <vector>

namespace Payroll {

struct Line {
    QString empId;
    double  hours  = 0.0;   // total hours (all)
    double  rate   = 0.0;   // role rate
    bool    hourly = true;  // H=true, F=false

    // existing total pay (net, including overtime if applicable)
    double  pay    = 0.0;

    // NEW: overtime breakdown (for hourly employees)
    double  regularHours  = 0.0;
    double  overtimeHours = 0.0;
    double  overtimePay   = 0.0;
};

// Compute payroll over ALL closed sessions in logsTxt.
//  - overtimeThresholdHours: hours above this are overtime
//  - overtimeMultiplier:     overtime pay = rate * multiplier
std::vector<Line> compute(
    const QString& idsTxt,
    const QString& logsTxt,
    double overtimeThresholdHours = 40.0,
    double overtimeMultiplier = 1.5
    );

// Compute payroll for a specific date range [startDate, endDate] (inclusive).
std::vector<Line> computeForRange(
    const QString& idsTxt,
    const QString& logsTxt,
    const QDate& startDate,
    const QDate& endDate,
    double overtimeThresholdHours = 40.0,
    double overtimeMultiplier = 1.5
    );

// Weekly summary: weekStart + next 6 days (7 days total).
std::vector<Line> computeWeekly(
    const QString& idsTxt,
    const QString& logsTxt,
    const QDate& weekStart,
    double overtimeThresholdHours = 40.0,
    double overtimeMultiplier = 1.5
    );

// Monthly summary: entire month containing (year, month).
std::vector<Line> computeMonthly(
    const QString& idsTxt,
    const QString& logsTxt,
    int year,
    int month,
    double overtimeThresholdHours = 160.0, // 4 weeks * 40h
    double overtimeMultiplier = 1.5
    );

// Turn results into a readable report
QString toText(const std::vector<Line>& lines);

} // namespace Payroll
