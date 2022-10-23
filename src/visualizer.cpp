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

#include "visualizer.h"

Visualizer::Visualizer( const std::vector<Particle>& pv, 
                        int nx, int ny, const Real& time, QWidget *parent ) 
    : QGLWidget( parent ), particle( pv ), t( time )
{
    np = int(particle.size( ));
    max_x = nx;
    max_y = ny;
    is_show_grid = false;
    
    qobj = new GLUquadricObj* [np];
    for (int i=0; i<np; i++)
    {
        qobj[i] = gluNewQuadric( );
        gluQuadricDrawStyle( qobj[i], GLU_FILL );
    }
    
    setFormat( QGLFormat( QGL::DoubleBuffer ) );
    setMinimumSize( max_x+BORDER, max_y+BORDER );
}

Visualizer::~Visualizer( )
{
    for (int i=0; i<np; i++)
    {
        gluDeleteQuadric( qobj[i] );
    }
    delete [] qobj;
}

void Visualizer::initializeGL( )
{
    qglClearColor( Qt::black );
    glShadeModel( GL_FLAT );
}

void Visualizer::resizeGL( int width, int height )
{
    double ww = double(width - 2.0*BORDER);
    double hh = double(height - 2.0*BORDER);
    
    k = qMin( ww / max_x, hh / max_y );
    
    double left = (ww - k*max_x) / 2.0 + BORDER;
    double right = left + k*max_x;
    
    double bottom = (hh - k*max_y) / 2.0 + BORDER;
    double top = bottom + k*max_y;
    
    glViewport( 0, 0, GLsizei(width), GLsizei(height) );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluOrtho2D( -left, right, -bottom, top );
}

void Visualizer::paintGL( )
{
    glClear( GL_COLOR_BUFFER_BIT );
    qglColor( Qt::lightGray );
    glRectd( 0.0, 0.0, max_x*k, max_y*k );
    
    qglColor( Qt::blue );
    for (int i=0; i<np; i++)
    {
        glPushMatrix( );
        Vector_2D pos = particle[i].pos( ) + (particle[i].v( ) * (t - particle[i].t( ) ) );
        glTranslated( double(pos.x( ))*k,
                      double(pos.y( ))*k,
                      0.0 );
        gluDisk( qobj[i], 0.0, double(particle[i].R( ))*k, 64, 1 );
        glPopMatrix( );
    }
    
    // ------------------------------------------
    // рисование сетки
    if (is_show_grid)
    {
        qglColor( Qt::darkGray );        
        glBegin( GL_LINES );
            // горизонтальные линии
            for (int i=0; i<=max_x; i++)
            {
                glVertex2d( i*k, 0.0 );
                glVertex2d( i*k, max_y*k );
            }
            
            // вертикальные линии
            for (int i=0; i<=max_y; i++)
            {
                glVertex2d( 0.0, i*k );
                glVertex2d( k*max_x, i*k );
            }
        glEnd( );
    }
    // ------------------------------------------
}

void Visualizer::setShowGrid( bool is_show )
{
    is_show_grid = is_show;
}
