// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 1993-2026 Shannon Smith

/*
 * Indian National (Saka) calendar arithmetic.
 *
 * This standalone provider has exactly one chronology. Keep the public
 * mode-aware ABI for Common, but do not retain dispatch tables for calendar
 * families which this shared object cannot implement.
 * Conversion and month-length rules remain in calendar-arithmetic-indian.c.
 */

#include "calendar-arithmetic.h"
#include "calendar-arithmetic-internal.h"
#include "julian-day.h"

gboolean
calendar_arithmetic_fields_from_jdn(
    CalendarCalendarMode mode,
    gint64 jdn,
    CalendarCalendarFields *fields)
{
    g_return_val_if_fail(fields != NULL, FALSE);
    *fields = (CalendarCalendarFields){ 0 };

    if (mode != CALENDAR_CALENDAR_MODE_INDIAN)
        return FALSE;

    calendar_arithmetic_indian_fields_from_jdn(jdn, fields);
    return TRUE;
}

gint64
calendar_arithmetic_month_start(
    CalendarCalendarMode mode,
    gint64 jdn)
{
    CalendarCalendarFields fields;

    if (!calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
        return G_MAXINT64;

    return calendar_arithmetic_indian_fields_to_jdn(
        fields.year, fields.month, 1);
}

gint64
calendar_arithmetic_add_months(
    CalendarCalendarMode mode,
    gint64 jdn,
    gint amount)
{
    CalendarCalendarFields fields;
    gint64 serial;

    if (!calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
        return G_MAXINT64;

    serial = calendar_i64_add_saturating(
        calendar_i64_add_saturating(
            calendar_i64_multiply_saturating(fields.year, 12),
            fields.month - 1),
        amount);

    fields.year = calendar_floor_divide(serial, 12);
    fields.month = (gint)calendar_positive_modulo(serial, 12) + 1;
    fields.auxiliary = 0;
    fields.day = MIN(
        fields.day,
        calendar_arithmetic_indian_month_length(fields.year, fields.month));

    return calendar_arithmetic_indian_fields_to_jdn(
        fields.year, fields.month, fields.day);
}

gint64
calendar_arithmetic_add_years(
    CalendarCalendarMode mode,
    gint64 jdn,
    gint amount)
{
    CalendarCalendarFields fields;

    if (!calendar_arithmetic_fields_from_jdn(mode, jdn, &fields))
        return G_MAXINT64;

    fields.year = calendar_i64_add_saturating(fields.year, amount);
    fields.auxiliary = 0;
    fields.day = MIN(
        fields.day,
        calendar_arithmetic_indian_month_length(fields.year, fields.month));

    return calendar_arithmetic_indian_fields_to_jdn(
        fields.year, fields.month, fields.day);
}
