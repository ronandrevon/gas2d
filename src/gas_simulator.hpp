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

#ifndef _GAS_SIMULATOR_H_
#define _GAS_SIMULATOR_H_

#include <vector>

#include "global_types.hpp"
#include "vector_algebra.hpp"
#include "particle.hpp"
#include "event_manager.hpp"
#include "event.hpp"
#include "cell.hpp"
#include "histogram.hpp"

using std::vector;

class Gas_simulator
{
public:
    Gas_simulator( int n, Real r, int max_x, int max_y, Real m, double* xv );
    void step_by( int ticks );
    void step_until_collision(double *info);
    void step_until_dt(double dt, double *xv, int dy);
    void reset_all( );

    // void set_distribution(double * dist);

    int crossings( ) const;
    int collisions( ) const;
    int bottom_collisions( ) const;
    int top_collisions( ) const;
    double dv(int i) const;
    Real k_energy( ) const;
    Real average_speed( ) const;
    const double* speed_histogram( ) const;
    void gather_statistic( );
    int histogram_size( ) const;
    double h( ) const;
    double var( ) const;
    double expected_value( ) const;
    const vector<Particle>& vparticles( ) const;
    const Real& time( ) const;
    void print_dist() const;
    void distibution(double *seq) const;

private:
    enum Border { X_LEFT = 1, X_RIGHT, Y_BOTTOM, Y_TOP };

    // расчет момента времени следующего события
    // для частицы ic и занесение его в очередь
    void min_time( int ic );

    Event crossing_time( int ic );
    Real collision_time( int ic, int jc );

    void step( );
    void step_crossing( int ic, const Event& ev );
    void step_collision( int ic, const Event& ev );
    void reflexion( int ic, int jc );

    // вспомогательные методы
    Vector_2D init_pos(  double x, double y,bool random);
    Vector_2D init_speed(double x, double y,bool random);

private:
    int np; // количество частиц
    int nx, ny; // размеры системы
    Real m_time;

    vector <Particle> m_particle; // массив частиц
    vector <vector <Cell> > m_cell; // подсистемы

    Event_manager <Event> m_event; // очередь с приоритетами

    // статистические данные
    int m_crossings;
    int m_collisions;
    int y;

    Real last_event_time;
    int last_ic;
    int last_jc;
    int m_bottom_col;
    int m_top_col;

    double *pressure;
    Real m_average_speed; // средняя скорость
    Histogram hst;
};

#endif // _GAS_SIMULATOR_H_
