/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 */

#ifndef INDICATOR_DATETIME_SNAP_H
#define INDICATOR_DATETIME_SNAP_H

#include <datetime/appointment.h>
#include <datetime/clock.h>
#include <datetime/formatter.h>

#include <memory>

namespace unity {
namespace indicator {
namespace datetime {

/**
 * \brief Pops up Snap Decisions for appointments
 */
class Snap
{
public:
    Snap(const std::shared_ptr<Clock>&);
    virtual ~Snap() =default;
    void operator()(const Appointment&);

private:
    PhoneFormatter m_formatter;
};

} // namespace datetime
} // namespace indicator
} // namespace unity

#endif // INDICATOR_DATETIME_SNAP_H
