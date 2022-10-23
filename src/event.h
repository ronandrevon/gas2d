// Copyright (c) 2008, 2012 Kirill Diduk (k_diduk@ukr.net)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// -----------------------------------------------------------------------------
// Файл event.h
//
// содержит класс Event (событие)
// -----------------------------------------------------------------------------

#ifndef _EVENT_H_
#define _EVENT_H_

#include "global_types.h"

enum Event_kind { COLLISION, CROSSING };

class Event
{
public:
	// Конструктор
	explicit Event( Event_kind k, int p, Real t ) 
		: m_kind( k ), m_partner( p ), m_time( t )
	{	}
	
	// -----------------------------------------------
	
	Event_kind kind( ) const { return m_kind; }
	int partner( ) const { return m_partner; }
	Real time( ) const { return m_time; }

        bool friend operator<( const Event& e1, const Event& e2 )
        { 
            return (e1.m_time < e2.m_time); 
        }

    //static const Event max_event;
	
private:
	Event_kind m_kind;
	int m_partner;
	Real m_time;
};

// -----------------------------------------------------------


// максимальное значение типа Event
const Event event_max = Event( Event_kind( 0 ), 0, REAL_MAX );
//const Event Event::max_event = Event( Event_kind( 0 ), 0, REAL_MAX );

#endif //  _EVENT_H_
