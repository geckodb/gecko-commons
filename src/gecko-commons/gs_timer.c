// Copyright (C) 2017 Marcus Pinnecke
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either user_port 3 of the License, or
// (at your option) any later user_port.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <gecko-commons/gs_timer.h>
#include <sys/time.h>

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E  I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static inline long long wallclock_current_ms(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec) * 1000)+(tv.tv_usec / 1000);
}

void gs_timer_start(gs_timer_t *timer)
{
    assert (timer);
    timer->start = wallclock_current_ms();
}

void gs_timer_stop(gs_timer_t *timer)
{
    assert (timer);
    timer->stop = wallclock_current_ms();
}

long long gs_timer_diff_ms(gs_timer_t *timer)
{
    assert (timer);
    return (timer->stop - timer->start);
}