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
// Файл particle.h
//
// Содержит определение класса Particle (частица).
// -----------------------------------------------------------------------------
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <cmath>
#include "global_types.hpp"
#include "vector_algebra.hpp"

class Particle
{
public:
	// ------------------------------------------------
	// Конструктор;
	// параматры: позиция (вектор), скорость (вектор),
	// радиус и масса.
	// ------------------------------------------------
	explicit Particle( Vector_2D position, Vector_2D velocity,
		  Real r = Real(0.25), Real m = Real(1) )
		: m_pos( position ), m_vel( velocity )
	{
		m_radius = r;
		m_mass = m;
		m_time = Real( 0 );
		m_i = int( std::floor( m_pos.x( ) ) );
		m_j = int( std::floor( m_pos.y( ) ) );
	}

	// ------------------------------------------------
	// методы чтения параметров

	Real R( ) const { return m_radius; }
	Real mass( ) const { return m_mass; }

	// координаты
	const Vector_2D& pos( ) const { return m_pos; }
	Real x( ) const { return m_pos.x( ); }
	Real y( ) const { return m_pos.y( ); }

	// скорость
	const Vector_2D& v( ) const { return m_vel; }
	Real vx( ) const { return m_vel.x( ); }
	Real vy( ) const { return m_vel.y( ); }

	// индексы подсистемы
	int i( ) const { return m_i; }
	int j( ) const { return m_j; }

	Real t( ) const { return m_time; } // локальное время

	// ------------------------------------------------

	// задать новую скорость
	void set_velocity( const Vector_2D& new_vel ) { m_vel = new_vel; }
	void reverse_vx( ) { m_vel.reverse_x( ); }
	void reverse_vy( ) { m_vel.reverse_y( ); }

	// задать новые индексы подсистемы
	void set_i( int inew ) { m_i = inew; }
	void set_j( int jnew ) { m_j = jnew; }

	// переместить частицу к новому локальному времени
	void move_to( Real t_next )
	{
		Real dt = t_next - m_time;
		m_pos += (dt * m_vel );
		m_time = t_next;
	}

private:
	Real m_radius; // радиус частицы
	Real m_mass; // масса частицы

	Vector_2D m_pos; // координаты центра частицы
	Vector_2D m_vel; // вектор скорости

	int m_i, m_j;  // мультииндекс подсистемы

	Real m_time;   // локальное время частицы
};

#endif // _PARTICLE_H_
