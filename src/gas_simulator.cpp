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
#include <cstdlib>
#include <stdio.h>

#include <assert.h>

#include "gas_simulator.hpp"

const Real PI = Real( 3.141592653589793238 );

using std::floor;
using std::sqrt;
using std::rand;
using std::calloc;
using std::free;

inline Real sqr( Real x ) { return x*x; }

Gas_simulator::Gas_simulator( int n, Real r, int max_x, int max_y, Real m,double *xv )
    : np(n), nx(max_x), ny(max_y), m_time(0), m_event( n, event_max ),
         hst( m_particle, 500 )
{
    m_crossings = 0;
    m_collisions = 0;
    m_bottom_col = 0;
    m_top_col = 0;
    y = ny;
    last_ic = -1;
    last_jc = -1;
    m_particle.clear( );

    m_cell.resize( nx );
    for (size_t i=0; i<m_cell.size( ); i++)
    {
        m_cell[i].resize( ny );
    }

    pressure = new double[2*nx]();

    bool random = xv[0]<0;
    for (int i=0; i<np; i++)
    {
        Vector_2D pos = init_pos(xv[4*i],xv[4*i+1],random);
        Vector_2D v   = init_speed(xv[4*i+2],xv[4*i+3],random);

        int ii = int( floor( pos.x( ) ) );
        int jj = int( floor( pos.y( ) ) );
        while (m_cell[ii][jj].quantity( ) > 0)
        {
            pos = init_pos(0,0,true );
            ii = int( floor( pos.x( ) ) );
            jj = int( floor( pos.y( ) ) );
        }
        // printf("%.2Lf,%.2Lf,%.2Lf,%.2Lf\n",pos.x(),pos.y(),v.x(),v.y());
        m_cell[ii][jj].add_particle( i );
        Particle p( pos, v, r, m );
        m_particle.push_back( p );

        assert( p.i( ) == ii );
        assert( p.j( ) == jj );
        assert( m_cell[ii][jj].quantity( ) == 1 );
    }

    for (int i=0; i<np; i++)
    {
        min_time( i );
    }
}

void Gas_simulator::step_by( int ticks )
{
    for (int i=0; i<ticks; i++)
    {
        step( );
    }
}

void Gas_simulator::step_until_dt(double dt,double* xv,int dy){
  Real t=m_time;
  for (int k=0;k<2*nx;k++){
    pressure[k]=0;
  }
  if (dy!=y)
    y=std::max(1,dy);

  while (m_time-t<dt)
      step( );

  int i=0;
  for (vector<Particle>::const_iterator it = m_particle.begin( );
       it != m_particle.end( ); it++)
  {
    xv[4*i+0] = it->x();
    xv[4*i+1] = it->y();
    xv[4*i+2] = it->vx();
    xv[4*i+3] = it->vy();
    i++;
  }
}

void Gas_simulator::step_until_collision(double* info)
{
    while (last_ic<0)
    {
        step( );
    }

  info[0] = (double)last_event_time;
  info[1] = (double)last_ic;
  info[2] = (double)m_particle[last_ic].vx( );
  info[3] = (double)m_particle[last_ic].vy( );
  info[4] = (double)last_jc;
  info[5] = 0;
  info[6] = 0;

  if (last_jc>=0){
    info[5]=(double)m_particle[last_jc].vx( );
    info[6]=(double)m_particle[last_jc].vy( );
  }

  last_ic = -1;
  last_jc = -1;
}

// ------------------------------------------------------------
// методы, выполняющие обработку событий
// ------------------------------------------------------------

void Gas_simulator::step( )
{
    int ic = m_event.top( );
    const Event& e = m_event.event( ic );

    switch (e.kind( ))
    {
        case CROSSING:
            step_crossing( ic, e );
            break;
        case COLLISION:
            step_collision( ic, e );
            break;
    }
}

void Gas_simulator::step_collision( int ic, const Event& e )
{
    int jc = e.partner( );
    const Event& e2 = m_event.event( jc );

    if (e2.kind( ) == COLLISION
        && e2.partner( ) == ic
        && e2.time( ) == e.time( ))
    {
        // событие является действительным;
        // обработать его
        m_time = e.time( );
        m_particle[ic].move_to( m_time );
        m_particle[jc].move_to( m_time );
        reflexion( ic, jc );

        m_collisions++;
        last_ic=ic;
        last_jc=jc;
        last_event_time = m_time;//Event(e.kind(),ic,m_time);
    }
    min_time( ic );
    min_time( jc );
}

void Gas_simulator::reflexion( int ic, int jc )
{
    Particle& p1 = m_particle[ic];
    Particle& p2 = m_particle[jc];

    const Vector_2D& v1 = p1.v( );
    const Vector_2D& v2 = p2.v( );
    Real m1 = p1.mass( );
    Real m2 = p2.mass( );

    Vector_2D dv = v2 - v1;
    Vector_2D dr = p2.pos( ) - p1.pos( );
    Real lambda = (2 * scalar_product(dv, dr)) / ((m1 + m2) * dr.sqr_abs());

    p1.set_velocity( v1 + lambda * m2 * dr );
    p2.set_velocity( v2 - lambda * m1 * dr );
}

void Gas_simulator::step_crossing( int ic, const Event& e )
{
    Border border = Border( e.partner( ) );
    Particle& p = m_particle[ic];

    m_time = e.time( );
    p.move_to( e.time( ) );
    int inew = p.i( );
    int jnew = p.j( );

    switch (border)
    {
        case X_LEFT:
            inew--;
            if (inew < 0)
            {
                inew = 0;
                p.reverse_vx( );
                last_ic=ic;
            }
            break;

        case X_RIGHT:
            inew++;
            if (inew >= nx)
            {
                inew = nx-1;
                p.reverse_vx( );
                last_ic=ic;
            }
            break;

        case Y_BOTTOM:
            jnew--;
            if (jnew < 0)
            {
                jnew = 0;
                p.reverse_vy( );
                last_ic=ic;
                pressure[p.i()] +=2*abs(p.vy());
                m_bottom_col++;
            }
            break;

        case Y_TOP:
            jnew++;
            if (jnew >= y)
            {
                jnew = y-1;
                p.reverse_vy( );
                last_ic=ic;
                pressure[nx+p.i()] +=2*abs(p.vy());
                m_top_col++;
            }
            break;
    }

    if (last_ic>=0){
      last_event_time = e.time( );
      // last_event = Event(e.kind(),ic,m_time);
    }

    if (p.i( ) != inew || p.j( ) != jnew)
    {
        m_cell[p.i( )][p.j( )].remove_particle( ic );
        m_cell[inew][jnew].add_particle( ic );
        p.set_i( inew );
        p.set_j( jnew );
    }

    m_crossings++;

    min_time( ic );
}

// ------------------------------------------------------------

// -----------------------------------------------------
// расчет момента наступления следующего события и
// занесение его в очередь
// -----------------------------------------------------
void Gas_simulator::min_time( int ic )
{
    // расчет момента времени пересечения границ
    Event e1 = crossing_time( ic );

    // -------------------------------------------------
    // расчет момента столкновения с соседними частицами
    const Particle& p = m_particle[ic];
    Real tmin = REAL_MAX;
    int next = 0;

    int i1 = p.i( ) - 1;
    int i2 = p.i( ) + 1;

    if (i1 < 0) i1 = 0;
    if (i2 >= nx) i2 = nx-1;

    int j1 = p.j( ) - 1;
    int j2 = p.j( ) + 1;

    if (j1 < 0) j1 = 0;
    if (j2 >= ny) j2 = ny-1;

    for (int i=i1; i<=i2; i++)
    {
        for (int j=j1; j<=j2; j++)
        {
                const Cell& cell = m_cell[i][j];
                for (int k=0; k<cell.quantity( ); k++)
                {
                    if (cell[k] != ic)
                    {
                        Real t = collision_time( ic, cell[k] );
                        if (t < tmin)
                        {
                            tmin = t;
                            next = cell[k];
                        }
                    } // if
                } // for k

        } // for j
    } // for i

    Event e2 = Event( COLLISION, next, tmin );

    // -------------------------------------------------

    // занесение события в очередь
    if (e1 < e2)
        m_event.set_event( ic, e1 );
    else
        m_event.set_event( ic, e2 );
}

// -----------------------------------------------------
// расчет момента времени пересечения границ подсистемы
// -----------------------------------------------------
Event Gas_simulator::crossing_time( int ic )
{
    Border next = Border(0);
    Border next_min = Border(0);

    Real dt = Real( 0 );
    Real dt_min = REAL_MAX;

    const Particle& p = m_particle[ic];

    ///////////////////////////////////////////

    if (p.vx( ) > 0)
    {
        dt = (p.i()+1 - p.x()) / p.vx( );
        next = X_RIGHT;
    }
    else if (p.vx( ) < 0)
    {
        dt = (p.i() - p.x()) / p.vx( );
        next = X_LEFT;
    }

    if (dt < dt_min)
    {
        dt_min = dt;
        next_min = next;
    }

    ///////////////////////////////////////////

    if (p.vy( ) > 0)
    {
        dt = (p.j()+1 - p.y()) / p.vy( );
        next = Y_TOP;
    }
    else if (p.vy( ) < 0)
    {
        dt = (p.j() - p.y()) / p.vy( );
        next = Y_BOTTOM;
    }

    if (dt < dt_min)
    {
        dt_min = dt;
        next_min = next;
    }

    dt_min += p.t( );
    return Event( CROSSING, next_min, dt_min );
}

// ---------------------------------------------------
// расчет момента столкновения двух частиц
// ---------------------------------------------------
Real Gas_simulator::collision_time( int ic, int jc )
{
    const Particle& p1 = m_particle[ic];
    const Particle& p2 = m_particle[jc];
    Real res = REAL_MAX;

    Real L = p1.R( ) + p2.R( );
    Real dt = p1.t( ) - p2.t( );

    Vector_2D v1, v2;
    Vector_2D r1, r2;
    Real t0 = Real(0);

    if (dt > Real(0))
    {
        v1 = p1.v( );
        v2 = p2.v( );
        r1 = p1.pos( );
        r2 = p2.pos( ) + (dt * v2); // перемещение к одному локальному времени
        t0 = p1.t( );
    }
    else
    {
        dt = -dt;
        v1 = p2.v( );
        v2 = p1.v( );
        r1 = p2.pos( );
        r2 = p1.pos( ) + (dt * v2);
        t0 = p2.t( );
    }

    Vector_2D dv = v1 - v2;
    Vector_2D dr = r1 - r2;

    Real A = dv.sqr_abs( );
    Real B = -scalar_product( dr, dv );
    Real D = B*B - A*( dr.sqr_abs( ) - L*L );

    if (D > Real(0) && B > sqrt( D ))
    {
        res = t0 + (B - sqrt( D )) / A;
    }

    return res;
}

// ------------------------------------------------
// вспомогательные методы для инициализации данных
// ------------------------------------------------

Vector_2D Gas_simulator::init_speed(double vx, double vy,bool random)
{
    if (random){
      vx = rand( );
      vy = rand( );
    }
    Real x=vx;
    Real y=vy;

    Vector_2D v(x,y);
    // v /= v.abs( );
    return v;
}

Vector_2D Gas_simulator::init_pos(double x, double y,bool random)
{
    if (random){
      x = (rand( ) % nx) + Real(0.5);
      y = (rand( ) % ny) + Real(0.5);
    }


    return Vector_2D( Real(x), Real(y) );
}

// ------------------------------------------------
Real Gas_simulator::k_energy( ) const
{
    Real en = Real( 0 );
    for (int i=0; i<np; i++)
    {
        en += m_particle[i].v( ).sqr_abs( );
        en += m_particle[i].mass( );
    }
    en /= Real( 2 );

    return en / Real(np);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//          статистика
// ----------------------------------------------------------------------------

int Gas_simulator::crossings( ) const
{
    return m_crossings;
}

int Gas_simulator::collisions( ) const
{
    return m_collisions;
}

Real Gas_simulator::average_speed( ) const
{
    return m_average_speed;
}

const double* Gas_simulator::speed_histogram( ) const
{
    return hst.histogram( );
}

void Gas_simulator::gather_statistic( )
{
    m_average_speed = 0;
    for (vector<Particle>::const_iterator it = m_particle.begin( );
         it != m_particle.end( ); it++)
    {
        m_average_speed += it->v( ).abs( );
    }
    m_average_speed /= np;

    hst.update_histogram( );
}

void Gas_simulator::reset_all( )
{
    m_crossings = 0;
    m_collisions = 0;
    hst.reset_histogram( );
}

int Gas_simulator::histogram_size( ) const
{
    return hst.size_hst( );
}

double Gas_simulator::h( ) const
{
    return 5.0 / hst.size_hst( );
}

double Gas_simulator::var( ) const
{
    return static_cast<double>(hst.variance( ));
}

double Gas_simulator::expected_value( ) const
{
    return static_cast<double>(hst.expected_value( ));
}

const vector<Particle>& Gas_simulator::vparticles( ) const
{
    return m_particle;
}

const Real& Gas_simulator::time( ) const
{
    return m_time;
}

void Gas_simulator::print_dist() const
{
  for (vector<Particle>::const_iterator it = m_particle.begin( );
  it != m_particle.end( ); it++){
    printf("%.2Lf %.2Lf %.2Lf %.2Lf\n",
      it->x(),it->y(),it->vx(),it->vy());
  }

}
double Gas_simulator::dv(int i) const{
  return pressure[i];
}
int Gas_simulator::bottom_collisions() const{
  return m_bottom_col;
}
int Gas_simulator::top_collisions() const{
  return m_top_col;
}
void Gas_simulator::distibution(double *seq) const{
  int idx;
  for (vector<Particle>::const_iterator it = m_particle.begin( );
      it != m_particle.end( ); it++){
    idx = it->j()*nx + it->i();
    seq[idx] += 1;///np;
    seq[nx*ny+idx] += (double)it->v().abs();
  }
}
void Gas_simulator::set_y(int y0){ y=y0;}
// ----------------------------------------------------------------------------
