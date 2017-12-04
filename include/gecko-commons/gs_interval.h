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

#pragma once

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <gecko-commons/stdinc.h>

// ---------------------------------------------------------------------------------------------------------------------
// M A C R O S
// ---------------------------------------------------------------------------------------------------------------------

#define INTERVAL_SPAN(interval)                                          \
    ({                                                                   \
        (interval->end - interval->begin);                               \
    })

#define INTERVAL_CONTAINS(interval, elem)                                \
    ({                                                                   \
        (elem >= interval->begin && elem < interval->end);               \
    })

#define INTERVAL_EQUALS(a, b)                                            \
    ({                                                                   \
        (a->begin == b->begin && a->end == b->end);                      \
    })

// ---------------------------------------------------------------------------------------------------------------------
// D A T A   T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct gs_interval_t {
    size_t begin;
    size_t end;
} gs_interval_t;
