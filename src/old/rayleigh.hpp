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

#ifndef _RAYLEIGH_H_
#define _RAYLEIGH_H_

#include <QWidget>
#include <QLineEdit>
#include "plotter.h"

class Rayleigh : public QWidget
{
	Q_OBJECT
public:
	Rayleigh( const double* histogram=0, int n=100, QWidget *parent = 0 );
	
	void setVariance( double new_var );
	void setExpectedValue( double expected_val );
	void setStep( double h );
	
private slots:
	void rayleighShow( int state );
    void scaleChanged( int val );
    
private:
	static double rayleighFunc( double x, double Dx );
    
private:
    Plotter* plotter;
    QLineEdit* expectedEdit;
    QLineEdit* varEdit;
};

#endif // _RAYLEIGH_H_
