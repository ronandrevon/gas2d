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
// Файл vector_algebea.cpp
//
// Содержит реализацию классов, представляющих геометрические
// векторы 2D, 3D и 4D.
// -----------------------------------------------------------------------------
#include "vector_algebra.h"

Vector_2D::Vector_2D( Real x, Real y ) : cx( x ), cy( y ) { }

// -------------------------------------------------------------
//  операторы-члены
// -------------------------------------------------------------

Vector_2D Vector_2D::operator+( const Vector_2D& v ) const
{
	return Vector_2D( cx + v.cx, cy + v.cy );
}

Vector_2D Vector_2D::operator-( const Vector_2D& v ) const
{
	return Vector_2D( cx - v.cx, cy - v.cy );
}

Vector_2D Vector_2D::operator+=( const Vector_2D& v )
{
	cx += v.cx;
	cy += v.cy;
	return (*this);
}

Vector_2D Vector_2D::operator-=( const Vector_2D& v )
{
	cx -= v.cx;
	cy -= v.cy;
	return (*this);
}

Vector_2D Vector_2D::operator*=( Real k )
{
	cx *= k;
	cy *= k;
	return (*this);
}

Vector_2D Vector_2D::operator/=( Real k )
{
	cx /= k;
	cy /= k;
	return (*this);
}

// -----------------------------------------------------------
// операторы-друзья
// -----------------------------------------------------------

Vector_2D operator*( const Vector_2D& v, Real k )
{
	return Vector_2D( v.cx * k, v.cy * k );
}

Vector_2D operator*( Real k, const Vector_2D& v )
{
	return Vector_2D( v.cx * k, v.cy * k );
}

Vector_2D operator/( const Vector_2D& v, Real k )
{
	return Vector_2D( v.cx / k, v.cy / k );
}

Vector_2D operator/( Real k, const Vector_2D& v )
{
	return Vector_2D( v.cx / k, v.cy / k );
}

// ------------------------------------------------
// скалярное произведение
// ------------------------------------------------
Real scalar_product( const Vector_2D& a, const Vector_2D& b )
{
	return (a.cx*b.cx + a.cy*b.cy);
}
