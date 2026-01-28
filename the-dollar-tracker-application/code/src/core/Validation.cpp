#include "Validation.h"

bool Validation::isValidEmployeeId(const QString& id)
{
    if (id.isEmpty()) return false;
    if (id.length() < 4 || id.length() > 8) return false;

    for (const QChar& ch : id) {
        if (!ch.isDigit())
            return false;
    }
    return true;
}

bool Validation::isValidHourlyRate(double rate)
{
    return rate > 0.0 && rate < 500.0;
}

bool Validation::isValidShiftTimes(const QDateTime& start,
                                   const QDateTime& end)
{
    if (!start.isValid() || !end.isValid())
        return false;

    if (end <= start)
        return false;

    // Prevent excessively long shifts
    return start.secsTo(end) <= 16 * 3600;
}
