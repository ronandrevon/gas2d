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

#ifndef _VISUALIZER_H_
#define _VISUALIZER_H_

#include <QGLWidget>
#include <GL/glu.h>
#include <vector>
#include "particle.h"

class Visualizer : public QGLWidget
{
public:
    Visualizer( const std::vector<Particle>& pv, int nx, int ny, 
                const Real& time, QWidget *parent = 0 );
    ~Visualizer( );
    void setShowGrid( bool is_show );
    
protected:
    void initializeGL( );
    void resizeGL( int width, int height );
    void paintGL( );
    
private:
    enum { BORDER = 10 };
    int np; // количество частиц
    
    // массив указателей на частицы, которые изображаются
    const std::vector<Particle>& particle;
    
    // размеры системы
    int max_x;
    int max_y;
    
    const Real& t;
    
    double k; // коэффициент растяжения/сжатия изображения
    bool is_show_grid;
    
    GLUquadricObj **qobj;
};

#endif // _VISUALIZER_H_
