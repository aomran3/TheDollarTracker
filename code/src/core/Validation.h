#pragma once

#include <QString>
#include <QDateTime>

// Centralized validation utilities (core layer)
class Validation {
public:
    // Employee IDs must be 4–8 digits
    static bool isValidEmployeeId(const QString& id);

    // Reasonable hourly rate bounds
    static bool isValidHourlyRate(double rate);

    // Shift times must be valid, end after start, max 16 hours
    static bool isValidShiftTimes(const QDateTime& start,
                                  const QDateTime& end);
};
