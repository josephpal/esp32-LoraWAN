/*
  Mutex class library for Arduino (tm), version 1.0

  Copyright (C) 2013 F1RMB, Daniel Caujolle-Bert <f1rmb.daniel@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Mutex.h"

Mutex::Mutex()
{
    //ctor
    m_count = 1;
}

Mutex::~Mutex()
{
    //dtor
}

uint8_t Mutex::_interlockedExchange(uint8_t v)
{
    uint8_t ov = m_count;
    m_count = v;

    return (ov);
}

void Mutex::lock()
{
    uint8_t prev;

    while(1)
    {
		while (m_count == 0) { delay(1); };

        prev = _interlockedExchange(0);

        if (prev == 1)
            break;
    }
}

void Mutex::unlock()
{
    (void) _interlockedExchange(1);
}


boolean Mutex::isLocked()
{
    if (m_count == 0)
        return (true);

    return (false);
}
