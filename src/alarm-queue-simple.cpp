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

#include <datetime/alarm-queue-simple.h>

#include <cmath>
#include <set>

namespace unity {
namespace indicator {
namespace datetime {

/***
****
***/

class SimpleAlarmQueue::Impl
{
public:

    Impl(const std::shared_ptr<Clock>& clock,
         const std::shared_ptr<Planner>& planner,
         const std::shared_ptr<WakeupTimer>& timer):
      m_clock{clock},
      m_planner{planner},
      m_timer{timer},
      m_datetime{clock->localtime()}
    {
        m_planner->appointments().changed().connect([this](const std::vector<Appointment>&){
            g_debug("AlarmQueue %p calling requeue() due to appointments changed", this);
            requeue();
        });

        m_clock->minute_changed.connect([this]{
            const auto now = m_clock->localtime();
            constexpr auto skew_threshold_usec = int64_t{90} * G_USEC_PER_SEC;
            const bool clock_jumped = std::abs(now - m_datetime) > skew_threshold_usec;
            m_datetime = now;
            if (clock_jumped) {
                g_debug("AlarmQueue %p calling requeue() due to clock skew", this);
                requeue();
            }
        });

        m_timer->timeout().connect([this](){
            g_debug("AlarmQueue %p calling requeue() due to timeout", this);
            requeue();
        });

        requeue();
    }

    ~Impl()
    {
    }

    core::Signal<const Appointment&, const Alarm&>& alarm_reached()
    {
        return m_alarm_reached;
    }

private:

    void requeue()
    {
        const auto appointments = m_planner->appointments().get();
        const Alarm* alarm;

        // kick any current alarms
        for (const auto& appointment : appointments)
        {
            if ((alarm = appointment_get_current_alarm(appointment)))
            {
                m_triggered.insert(std::make_pair(appointment.uid, alarm->time));
                m_alarm_reached(appointment, *alarm);
            }
        }

        // idle until the next alarm
        if ((alarm = find_next_alarm(appointments)))
        {
            g_debug ("setting timer to wake up for next appointment '%s' at %s", 
                     alarm->text.c_str(),
                     alarm->time.format("%F %T").c_str());

            m_timer->set_wakeup_time(alarm->time);
        }
    }

    bool already_triggered (const Appointment& appt, const Alarm& alarm) const
    {
        const std::pair<const std::string&,const DateTime&> key{appt.uid, alarm.time};
        return m_triggered.count(key) != 0;
    }

    // return the next Alarm (if any) that will kick now or in the future
    const Alarm* find_next_alarm(const std::vector<Appointment>& appointments) const
    {
        const Alarm* best {};
        const auto now = m_clock->localtime();
        const auto beginning_of_minute = now.start_of_minute();

        g_debug ("planner has %zu appointments in it", (size_t)appointments.size());

        for(const auto& appointment : appointments)
        {
            for(const auto& alarm : appointment.alarms)
            {
                if (already_triggered(appointment, alarm))
                    continue;

                if (alarm.time < beginning_of_minute) // has this one already passed?
                    continue;

                if (best && (best->time < alarm.time)) // do we already have a better match?
                    continue;

                best = &alarm;
            }
        }

        return best;
    }

    // return the Appointment's current Alarm (if any)
    const Alarm* appointment_get_current_alarm(const Appointment& appointment) const
    {
        const auto now = m_clock->localtime();

        for (const auto& alarm : appointment.alarms)
            if (!already_triggered(appointment, alarm) && DateTime::is_same_minute(now, alarm.time))
                return &alarm;

        return nullptr;
    }


    std::set<std::pair<std::string,DateTime>> m_triggered;
    const std::shared_ptr<Clock> m_clock;
    const std::shared_ptr<Planner> m_planner;
    const std::shared_ptr<WakeupTimer> m_timer;
    core::Signal<const Appointment&, const Alarm&> m_alarm_reached;
    DateTime m_datetime;
};

/***
****  Public API
***/


SimpleAlarmQueue::SimpleAlarmQueue(const std::shared_ptr<Clock>& clock,
                                   const std::shared_ptr<Planner>& planner,
                                   const std::shared_ptr<WakeupTimer>& timer):
    impl{new Impl{clock, planner, timer}}
{
}

SimpleAlarmQueue::~SimpleAlarmQueue()
{
}

core::Signal<const Appointment&, const Alarm&>&
SimpleAlarmQueue::alarm_reached()
{
    return impl->alarm_reached();
}

/***
****
***/

} // namespace datetime
} // namespace indicator
} // namespace unity
