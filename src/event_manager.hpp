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
// файл event_manager.h
//
// содержит определение класса Event_manager
// -----------------------------------------------------------------------------

#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_


#include <vector>


// -----------------------------------------------------------
// Шаблонный класс Event_manager предназначен для управлением
// событиями.
//
// ----------------
// Параметр шаблона - тип "событие". Для события должна быть
// определена операция '<'.
// -----------------------------------------------------------

template <class E>
	class Event_manager
{
public:
	// -------------------------------------------
	// конструктор принимает количество событий,
	// и фиктивный элемент, имеющий заведомо
	// большое значение ("бесконечность")
	// -------------------------------------------	
	Event_manager( int n, const E& infinity );
	
	// индекс ближайшего событие
	int top( ) const { return heap[1]-1; }
	
	// событие, связанное с i-м участником
	const E& event( int i ) const { return events[i+1]; }
	
	// задать новое событие для i-го участника
	void set_event( int i, const E& new_event );
	
private:
	void pull_through( int i ); // протащить i-й элемент по пирамиде
	
private:
	std::vector <E> events; // вектор событий
	
	std::vector <int> heap; // пирамида
	int i_leaf; // индекс первого листа дерева
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


template <class E>
	Event_manager <E>::Event_manager( int n, const E& infinity )
		: events( n+2, infinity )
{	
	i_leaf = 1;
	while (2*i_leaf <= n)
	{
		i_leaf *= 2;
	}
	
	int n_h = 2*i_leaf - 1;
	
	heap.resize( n_h+2 );
	for (size_t i=0; i<heap.size( ); i++)
	{
		heap[i] = 0;
	}
}

template <class E>
	void Event_manager <E>::set_event( int i, const E& new_event )
{
	events[i+1] = new_event;
	pull_through( i+1 );
}

template <class E>
	void Event_manager <E>::pull_through( int i )
{
	int ii = (i-1)/2 + i_leaf;
	int j = i;
	
	(i%2) ? (j++) : (j--);

	if (events[i] < events[j])
		heap[ii] = i;
	else
		heap[ii] = j;
	
	i = ii;
	do
	{
		ii = i / 2;
		(i%2) ? (j = i-1) : (j = i+1);
		
		if (events[ heap[i] ] < events[ heap[j] ])
			heap[ii] = heap[i];
		else
			heap[ii] = heap[j];
		
		i = ii;
		
	} while (i > 1);
}

#endif // _EVENT_MANAGER_H_
