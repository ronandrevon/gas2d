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

#include <cmath>
#include "histogram.h"

const Real PI = Real( 3.141592653589793238 );

template <typename T>
    inline T sqr( T x ) 
{
    return x*x;
}

Histogram::Histogram( const vector<Particle>& vparticle, int max_histo )
    : vp( vparticle ), max_hst( max_histo ), h( Real(5) / max_histo )
{
    hst = new Longint[max_hst+1];
    norm = new double[max_hst+1];
    
    reset_histogram( );
    Mx = Real( 0 );
    Dx = Real( 0 );
}

Histogram::~Histogram( )
{
    delete [] norm;
    delete [] hst;
}

// --------------------------------------------------------
//              методы чтения данных
// --------------------------------------------------------

const double* Histogram::histogram( ) const { return norm; }

Real Histogram::expected_value( ) const { return Mx; }

Real Histogram::variance( ) const { return Dx; }

int Histogram::size_hst( ) const { return max_hst; }
// --------------------------------------------------------
// --------------------------------------------------------

// сброс показаний гистограммы
void Histogram::reset_histogram( )
{
    for (int i=0; i<=max_hst; i++)
    {
        hst[i] = 0;
        norm[i] = 0.0;
    }
    Dx = 0;
    Mx = 0;
}

// обновление всех данных
void Histogram::update_histogram( )
{
    // ---------------------------------------------------------------------
    // обновление гистограммы
    // ----------------------
    for (vector<Particle>::const_iterator
            it = vp.begin( ); it != vp.end( ); it++ )
    {
        int i = int( std::floor( it->v( ).abs( ) / h ) );
        
        if (i>=0 && i<max_hst)
        {
            hst[i]++;
        }
        else
        {
            hst[max_hst]++;
        }
    }
    
    Longint hsum = 0;
    for (int i=0; i<=max_hst; i++)
    {
        hsum += hst[i];
    }
    
    // ------------------------------------------------
    // нормализация гистограммы
    // и вычисление мат. ожидания
    // -----------------------------------
    Mx = 0;
    for (int i=0; i<=max_hst; i++)
    {
        norm[i] = double( hst[i] ) / hsum;
        Mx += Real(i + 0.5) * norm[i];
    }
    Mx *= h;
    // -----------------------------------
    
    Dx = 2 * sqr( Mx ) / PI;
    // ------------------------------------------------
}
