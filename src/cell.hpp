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

#ifndef __CELL_H_
#define __CELL_H_

// ----------------------------------------------------------------------------
// Файл cell.h
//
// Содержит класс Cell, представляющий собой ячейку-подсистему.
// --------------------------------------------------------------------------
// Ячейка хранит номера частиц, содержащихся в ней. Интерфейс класса
// предоставляет методы для добавления и удаления частицы с заданным номером.
// Доступ к элементам ячейки осуществляется с помощью операции индексации [].
// ----------------------------------------------------------------------------

#include <algorithm>
#include <vector>

class Cell
{
public:
	// -----------------------------------
	// добавление и удаление
	void add_particle( int idx ) { m_particle.push_back( idx ); }
	
	void remove_particle( int idx ) 
	{
		v_iter it = std::find( m_particle.begin( ), m_particle.end( ),
				       idx );
		m_particle.erase( it );
	}
	
	// количество частиц в ячейке
	int quantity( ) const { return int( m_particle.size( ) ); }

	// -----------------------------------
	// операция индексации
	int operator[]( int i ) const { return m_particle[i]; }
	
private:
	typedef std::vector <int>::iterator v_iter;
	std::vector <int> m_particle;
};

#endif // __CELL_H_
