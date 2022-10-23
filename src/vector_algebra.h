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
// Файл vector_algebea.h
//
// Содержит определения классов, представляющих геометрические
// векторы 2D, 3D и 4D.
// -----------------------------------------------------------------------------
#ifndef _VECTOR_ALGEBRA_H_
#define _VECTOR_ALGEBRA_H_

#include <cmath>
#include "global_types.h"

// -----------------------------------------------------
// двумерный вектор
// -----------------------------------------------------

class Vector_2D
{
private:
	Real cx; // компонента x
	Real cy; // компонента y

public:
	explicit Vector_2D( Real x = Real(0), Real y = Real(0) );
	
	// методы чтения
	Real x( ) const { return cx; }
	Real y( ) const { return cy; }
	
	// методы записи
	void set_x( Real x ) { cx = x; }
	void set_y( Real y ) { cy = y; }
	
	Real sqr_abs( ) const { return (cx*cx+cy*cy); } // квадрат модуля
	Real     abs( ) const { return sqrt( sqr_abs( ) ); } // модуль
	
	void reverse_x( ) { cx = -cx; }
	void reverse_y( ) { cy = -cy; }
	
	// ------------------------------------------------
	//  операторы-члены
	// ------------------------------------------------
		
	Vector_2D operator+( const Vector_2D& v ) const;
	Vector_2D operator-( const Vector_2D& v ) const;
	
	Vector_2D operator+=( const Vector_2D& v );
	Vector_2D operator-=( const Vector_2D& v );
	Vector_2D operator*=( Real k );
	Vector_2D operator/=( Real k );
	
	// ------------------------------------------------
	// операторы-друзья
	// ------------------------------------------------
	Vector_2D friend operator*( const Vector_2D& v, Real k );
	Vector_2D friend operator*( Real k, const Vector_2D& v );
	Vector_2D friend operator/( const Vector_2D& v, Real k );
	Vector_2D friend operator/( Real k, const Vector_2D& v );
	
	// ------------------------------------------------
	// скалярное произведение
	// ------------------------------------------------
	Real friend scalar_product( const Vector_2D& a, 
					  const Vector_2D& b );

};


#endif // _VECTOR_ALGEBRA_H_
