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

#include <QCheckBox>
#include <QVBoxLayout>
#include "visualizer_tab.h"

Visualizer_tab::Visualizer_tab( const std::vector<Particle>& pv, 
                                int nx, int ny, const Real& time, 
                                QWidget *parent )
    : QWidget( parent )
{
    visualizer = new Visualizer( pv, nx, ny, time, this );
    QCheckBox* grid = new QCheckBox( "show grid", this );
    
    connect( grid, SIGNAL( stateChanged( int ) ),
             this, SLOT( showGrid( int ) ) );
    
    visualizer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    grid->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    QVBoxLayout* box = new QVBoxLayout;
    box->addWidget( visualizer );
    box->addWidget( grid );
    
    setLayout( box );
}

void Visualizer_tab::paintEvent( QPaintEvent *event )
{
    visualizer->repaint( );
    event->accept( );
}

void Visualizer_tab::showGrid( int is_show )
{
    visualizer->setShowGrid( bool(is_show) );
    visualizer->repaint( );
}
