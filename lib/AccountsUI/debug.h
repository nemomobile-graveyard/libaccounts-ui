/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef ACCOUNTSUI_DEBUG_H
#define ACCOUNTSUI_DEBUG_H

#include <unistd.h>
#include <time.h>
#include <stddef.h>

#include <QDebug>

#ifdef PERFORMANCE_MEASUREMENTS
    #define PWATCHER_INIT(arg)  static             \
                                AccountsUI::PerformanceWatcher \
                                arg
    #define PWATCHER_TRACE(arg) qDebug() << __FILE__ << __func__ << __LINE__ << ":" << arg.elapsed()
    #define PWATCHER_START(arg) arg.start()
    #define PWATCHER_RESTART(arg) arg.restart()
#else
    #define PWATCHER_INIT(arg)
    #define PWATCHER_TRACE(arg) if (1) ; else qDebug()
    #define PWATCHER_START(arg)
    #define PWATCHER_RESTART(arg)
#endif

namespace AccountsUI {

class PerformanceWatcher {
public:
    PerformanceWatcher() {
        start();
    }
    ~PerformanceWatcher() {};

    void start() {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_start);
    }

    long restart() {
        long delta = elapsed();
        start();
        return delta;
    }

    long elapsed() {
        timespec now;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
        long delta = (now.tv_sec * 1000 + now.tv_nsec * 1.0e-6) -
            (m_start.tv_sec * 1000 + m_start.tv_nsec * 1.0e-6);

        return delta;
    }

private:
    timespec m_start;
};

} // namespace

#endif // ACCOUNTSUI_DEBUG_H

