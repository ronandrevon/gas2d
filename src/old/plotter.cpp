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

#include "plotter.h"
#include <GL/glu.h>

Plotter::Plotter( QWidget *parent ) 
    : QGLWidget( parent ), hst( 0 ), distr( 0 )
{
    is_show_distr = false;
    k = 1.0;
    h = 1.0;
    nh = 0;
    var = 1.0;
    
    setMinimumSize( 400, 200 );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void Plotter::initializeGL( )
{
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glShadeModel( GL_FLAT );
}

void Plotter::resizeGL( int width, int height )
{
    glViewport( 0, 0, GLsizei(width), GLsizei(height) );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluOrtho2D( 0.0, GLdouble(width), 0.0, GLdouble(height) );
}

void Plotter::paintGL( )
{
    const int step = (width( )-18) / 5;
    
    glClear( GL_COLOR_BUFFER_BIT );
    
    qglColor( Qt::yellow );
    glBegin( GL_LINES );
    	// горизонтальная ось
        glVertex2d( 9, 9 );
        glVertex2d( width( )-9, 9  );	
	
        // вертикальная ось
        glVertex2d( 9, 9 );
        glVertex2d( 9, height( )-9 );
	
	// отметки на горизонтальной оси
        for (int i=step; i<(width( )-9); i+=step )
        {
            glVertex2d( i, 4 );
            glVertex2d( i, 14 );
        }
    glEnd( );
    
    // ----------------------------------------------------------------
    // рисование гистограммы
    const double dx = double(width() - 20) / (nh);
    const double y1 = 10.0;
    
    if (hst && nh > 0)
    {
        qglColor( Qt::blue );
        glBegin( GL_QUADS );
        for (int i=0; i<=nh; i++)
        {
            double x1 = 10.0 + dx * i;
            double x2 = x1 + dx;
            double y2 = 10.0 + hst[i]*(height( )-20) * k;
            
            glVertex2d( x1, y1 );
            glVertex2d( x2, y1 );
            glVertex2d( x2, y2 );
            glVertex2d( x1, y2 );
        }
        glEnd( );
    }
    
    if (is_show_distr)
    {
        double *r = (double*)malloc( (nh+1) * sizeof(double) );
        double norm = 0.0;
        for (int i=0; i<=nh; i++)
        {
            r[i] = distr( i*h, var );
            norm += r[i];
        }
        
        for (int i=0; i<=nh; i++)
        {
            r[i] /= norm;
        }
        
        qglColor( Qt::red );
        glBegin( GL_LINE_STRIP );
            for (int i=0; i<=nh; i++)
            {
                double y = 10.0 +  r[i] * (height( )-20)*k;
                double x = 10.0 + dx * i;
                glVertex2d( x, y );
            }
        glEnd( );
        
        free(r);
    }
}

void Plotter::setHistogram( const double* new_hst, int n )
{
    hst = new_hst;
    nh = n;
}

void Plotter::setFunc( FUNC func )
{
    distr = func;
}

void Plotter::setShowDistr( bool is_show )
{
    is_show_distr = is_show;
    repaint( );
}

void Plotter::setScale( double scale )
{
    k = scale;
}

void Plotter::setStep( double step )
{
    h = step;
    repaint( );
}

void Plotter::setVar( double new_var )
{
    var = new_var;
    repaint( );
}
