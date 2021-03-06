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

#ifndef INDICATOR_DATETIME_TIMEZONE_MOCK_H
#define INDICATOR_DATETIME_TIMEZONE_MOCK_H

#include <datetime/timezone.h>

namespace unity {
namespace indicator {
namespace datetime {

class MockTimezone: public Timezone
{
public:
    MockTimezone() =default;
    explicit MockTimezone(const std::string& zone) {timezone.set(zone);}
    ~MockTimezone() =default;
};

} // namespace datetime
} // namespace indicator
} // namespace unity

#endif // INDICATOR_DATETIME_TIMEZONE_MOCK_H
