#include "Payroll.h"

#include <QMap>
#include <QPair>
#include <QStringList>
#include <QTime>
#include <QDate>
#include <QtMath>

// ---- helpers --------------------------------------------------------------

static inline QString T(const QString& s) { return s.trimmed(); }

static bool parseRoleToken(const QString& token, double& rateOut, bool& hourlyOut) {
    // token format: RoleName$<rate>/<H|F>/
    // e.g. "Service$20/H/"
    int dollar = token.indexOf('$');
    if (dollar < 0) return false;
    int slash1 = token.indexOf('/', dollar + 1);
    if (slash1 < 0) return false;
    int slash2 = token.indexOf('/', slash1 + 1);
    if (slash2 < 0) return false;

    bool ok = false;
    const QString rateStr = token.mid(dollar + 1, slash1 - dollar - 1);
    const QString kindStr = token.mid(slash1 + 1, slash2 - slash1 - 1).trimmed().toUpper();

    double rate = rateStr.toDouble(&ok);
    if (!ok) return false;

    rateOut = rate;
    hourlyOut = (kindStr == "H");
    return true;
}

// Parse date from logsTxt line like:
//   "emp312, 2025.11.09 -12:00:00/15:30:00"
static bool parseLogDate(const QString& rest, QDate& outDate) {
    // rest = "yyyy.MM.dd -HH:MM:SS/..."
    int space = rest.indexOf(' ');
    if (space < 0) return false;
    const QString dateStr = T(rest.left(space));
    QDate d = QDate::fromString(dateStr, "yyyy.MM.dd");
    if (!d.isValid()) return false;
    outDate = d;
    return true;
}

// Core implementation used by all public APIs
static std::vector<Payroll::Line> computeInternal(
    const QString& idsTxt,
    const QString& logsTxt,
    bool useDateRange,
    const QDate& startDate,
    const QDate& endDate,
    double overtimeThresholdHours,
    double overtimeMultiplier
    )
{
    using namespace Payroll;

    // Build role map: empId -> (rate, hourly)
    QMap<QString, QPair<double, bool>> roleMap;

    for (const QString& rawLine : idsTxt.split('\n')) {
        const QString line = T(rawLine);
        if (line.isEmpty()) continue;

        // Accept either just "empId"
        // or "empId, Role$rate/H|F/"
        int comma = line.indexOf(',');
        QString empId = line;
        if (comma >= 0) empId = T(line.left(comma));

        double rate = 0.0;
        bool hourly = true;

        if (comma >= 0) {
            const QString token = T(line.mid(comma + 1)); // Role$20/H/
            double r; bool h;
            if (parseRoleToken(token, r, h)) { rate = r; hourly = h; }
        }

        roleMap[empId] = qMakePair(rate, hourly);
    }

    // Aggregate hours and pay from logs
    QMap<QString, Line> acc;

    for (const QString& rawLine : logsTxt.split('\n')) {
        const QString L = T(rawLine);
        if (L.isEmpty() || L.startsWith('#')) continue;

        // Expected formats in emp_clock.txt:
        //   empId, yyyy.MM.dd -HH:MM:SS/*
        //   empId, yyyy.MM.dd -HH:MM:SS/HH:MM:SS
        // We only compute when there is a close time (no "/*" at end).
        int comma = L.indexOf(',');
        if (comma < 0) continue;

        const QString empId = T(L.left(comma));
        const QString rest  = T(L.mid(comma + 1));

        // Parse date if needed (for weekly/monthly/ range)
        QDate logDate;
        if (!parseLogDate(rest, logDate)) continue;
        if (useDateRange) {
            if (logDate < startDate || logDate > endDate) {
                continue; // not in requested period
            }
        }

        int dash  = rest.indexOf('-');
        if (dash < 0) continue;
        int slash = rest.indexOf('/', dash + 1);
        if (slash < 0) continue;

        const QString tInStr  = T(rest.mid(dash + 1, slash - (dash + 1))); // HH:MM:SS
        const QString tOutStr = T(rest.mid(slash + 1));                     // HH:MM:SS OR "/*"

        if (tOutStr.endsWith("/*")) {
            // still open; ignore for payroll
            continue;
        }

        const QTime tIn  = QTime::fromString(tInStr,  "HH:mm:ss");
        const QTime tOut = QTime::fromString(tOutStr, "HH:mm:ss");
        if (!tIn.isValid() || !tOut.isValid()) continue;

        qint64 secs = tIn.secsTo(tOut);
        if (secs <= 0) continue;

        const double hours = double(secs) / 3600.0;

        const auto pair = roleMap.value(empId, qMakePair(0.0, true));
        Line& line = acc[empId];
        line.empId = empId;
        line.rate  = pair.first;
        line.hourly= pair.second;
        line.hours += hours;

        // For non-hourly (flat) workers, we keep old behavior:
        // rate is a flat amount per closed session.
        if (!line.hourly) {
            line.pay += line.rate; // flat per shift
        }
    }

    // Post-process: compute overtime for hourly employees.
    std::vector<Line> out;
    out.reserve(acc.size());

    for (auto it = acc.cbegin(); it != acc.cend(); ++it) {
        Line line = it.value(); // copy

        if (line.hourly && line.rate > 0.0) {
            const double totalH = line.hours;
            const double regH   = qMin(overtimeThresholdHours, totalH);
            const double otH    = qMax(0.0, totalH - overtimeThresholdHours);

            line.regularHours  = regH;
            line.overtimeHours = otH;
            line.overtimePay   = otH * line.rate * overtimeMultiplier;

            const double regularPay = regH * line.rate;
            line.pay = regularPay + line.overtimePay;
        }
        // else non-hourly: pay already accumulated as flat per shift,
        // regular/overtime fields left at 0.

        out.push_back(line);
    }

    return out;
}

// ---- API implementation ---------------------------------------------------

namespace Payroll {

std::vector<Line> compute(
    const QString& idsTxt,
    const QString& logsTxt,
    double overtimeThresholdHours,
    double overtimeMultiplier)
{
    // no date range: use all logs
    QDate dummyStart;
    QDate dummyEnd;
    return computeInternal(
        idsTxt, logsTxt,
        false, dummyStart, dummyEnd,
        overtimeThresholdHours,
        overtimeMultiplier
        );
}

std::vector<Line> computeForRange(
    const QString& idsTxt,
    const QString& logsTxt,
    const QDate& startDate,
    const QDate& endDate,
    double overtimeThresholdHours,
    double overtimeMultiplier)
{
    return computeInternal(
        idsTxt, logsTxt,
        true, startDate, endDate,
        overtimeThresholdHours,
        overtimeMultiplier
        );
}

std::vector<Line> computeWeekly(
    const QString& idsTxt,
    const QString& logsTxt,
    const QDate& weekStart,
    double overtimeThresholdHours,
    double overtimeMultiplier)
{
    QDate weekEnd = weekStart.addDays(6);
    return computeForRange(
        idsTxt, logsTxt,
        weekStart, weekEnd,
        overtimeThresholdHours,
        overtimeMultiplier
        );
}

std::vector<Line> computeMonthly(
    const QString& idsTxt,
    const QString& logsTxt,
    int year,
    int month,
    double overtimeThresholdHours,
    double overtimeMultiplier)
{
    QDate firstDay(year, month, 1);
    QDate lastDay = firstDay.addMonths(1).addDays(-1);
    return computeForRange(
        idsTxt, logsTxt,
        firstDay, lastDay,
        overtimeThresholdHours,
        overtimeMultiplier
        );
}

QString toText(const std::vector<Line>& lines) {
    if (lines.empty()) {
        return "No closed sessions found.\n"
               "Make sure emp_clock.txt has lines like:\n"
               "  emp312, 2025.11.09 -12:00:00/15:30:00\n"
               "and ids.txt has role specs like:\n"
               "  emp312, Service$20/H/\n";
    }
    QString out;
    for (const Line& l : lines) {
        out += "**************************************************************\n";
        out += l.empId + "\n";
        out += QString("Total worked hours: %1\n")
                   .arg(QString::number(l.hours, 'f', 2));

        if (l.hourly && l.rate > 0.0) {
            out += QString("Regular hours: %1\n")
            .arg(QString::number(l.regularHours, 'f', 2));
            out += QString("Overtime hours: %1\n")
                       .arg(QString::number(l.overtimeHours, 'f', 2));
            out += QString("Overtime pay: %1\n")
                       .arg(QString::number(l.overtimePay, 'f', 2));
        }

        out += QString("Net pay: %1\n")
                   .arg(QString::number(l.pay, 'f', 2));
    }
    return out;
}

} // namespace Payroll
