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

#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <vector>
#include "global_types.hpp"
#include "particle.hpp"

typedef long long int Longint;
using std::vector;

class Histogram
{
public:
    Histogram( const vector<Particle>& vparticle, int max_histo=100 );
    ~Histogram( );

    const double* histogram( ) const; // указатель на норм. гистограмму
    Real expected_value( ) const; // математическое ожидание
    Real variance( ) const; // дисперсия
    int size_hst( ) const;

    void reset_histogram( ); // сброс показаний гистограммы
    void update_histogram( ); // обновить гистограмму

private:
    const vector<Particle>& vp;

    const int max_hst; // количество элементов гистограммы
    const Real h; // шаг гистограммы
    Longint *hst; // гистограмма скоростей
    double *norm; // нормализованная гистограмма скоростей

    Real Mx; // математическое ожидание
    Real Dx; // дисперсия
};

#endif // _HISTOGRAM_H_
