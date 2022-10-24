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

#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include <QGLWidget>

class Plotter : public QGLWidget
{
private:
    typedef double (*FUNC)( double, double );
    
public:
    Plotter( QWidget *parent = 0 );
    
    void setHistogram( const double* new_hst, int n );
    void setFunc( FUNC func );
    void setShowDistr( bool is_show );
    void setScale( double scale );
    void setStep( double step );
    void setVar( double new_var );
    
protected:
    void initializeGL( );
    void resizeGL( int width, int height );
    void paintGL( );
    
private:
    bool is_show_distr; // показ графика функции
    int nh; // размер гистограммы
    const double* hst; // гистограмма
    FUNC distr; // функция распределения
    double k; // коэффициент масштабирования
    double h;
    double var; // дисперсия
};

#endif // _PLOTTER_H_
