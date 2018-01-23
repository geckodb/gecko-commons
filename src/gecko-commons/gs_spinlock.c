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
// If not, see .

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <gecko-commons/gs_spinlock.h>
#include <stdatomic.h>

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E  I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

GS_DECLARE(gs_status_t) gs_spinlock_create(gs_spinlock_t *spinlock)
{
    atomic_flag_clear(&spinlock->lock);
    return GS_SUCCESS;
}

GS_DECLARE(gs_status_t) gs_spinlock_lock(gs_spinlock_t *spinlock)
{
    while(atomic_flag_test_and_set(&spinlock->lock))
        ;
    return GS_SUCCESS;
}

GS_DECLARE(gs_status_t) gs_spinlock_unlock(gs_spinlock_t *spinlock)
{
    atomic_flag_clear(&spinlock->lock);
    return GS_SUCCESS;
}

