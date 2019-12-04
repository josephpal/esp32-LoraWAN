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
#ifndef MUTEX_H
#define MUTEX_H

#include <Arduino.h>

class Mutex
{
    public:
        Mutex();
        ~Mutex();

        void        lock();
        void        unlock();
        boolean     isLocked();


    protected:
        uint8_t     _interlockedExchange(uint8_t v);

    private:
        uint8_t     m_count;
};

#endif // MUTEX_H
