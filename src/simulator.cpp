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

#include <QApplication>
#include <QListWidget>
#include <QGridLayout>
#include <QStackedLayout>
#include <cstdlib>

#include "simulator.h"

Simulator::Simulator( QWidget *parent ) : QWidget( parent )
{
    init_dialog = new Initialize_dialog( this );
    if (init_dialog->exec( ) == QDialog::Rejected)
    {
        gas = 0;
        std::exit( 1 );
    }
    timer = new QTimer(this);
    gas = new Gas_simulator( init_dialog->amount( ), init_dialog->radius( ),
                             init_dialog->max_x( ), init_dialog->max_y( ),
                                     init_dialog->mass( ) );
    params = new Parameters( this );
    statistic = new Statistic( this );
    process = new Process_tab( this );
    rayleigh = new Rayleigh(gas->speed_histogram( ), gas->histogram_size( ),
                            this );
    visualizer = new Visualizer_tab( gas->vparticles( ),
                                             init_dialog->max_x( ),
                                             init_dialog->max_y( ), 
                                     gas->time( ), this );
    
    QListWidget *listWidget = new QListWidget;
    QStackedLayout *stackedLayout = new QStackedLayout;
    
    listWidget->addItem( "Parameters" );
    listWidget->addItem( "Rayleigh" );
    listWidget->addItem( "Visualize" );
    
    stackedLayout->addWidget( params );
    stackedLayout->addWidget( rayleigh );
    stackedLayout->addWidget( visualizer );
    
    connect( listWidget, SIGNAL( currentRowChanged( int ) ),
             stackedLayout, SLOT( setCurrentIndex( int ) ) );
    connect( process, SIGNAL( startClicked( ) ),
             this, SLOT( startProcessing( ) ) );
    connect( timer, SIGNAL( timeout( ) ), this, SLOT( onTimer( ) ) );
    connect( params, SIGNAL( onNewParameters( ) ),
                this, SLOT( zero( ) ) );
    
    listWidget->setCurrentRow( 0 );
    listWidget->setMaximumWidth( listWidget->sizeHint( ).width( ) );
    process->resize( process->sizeHint( ) );
    process->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    statistic->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    
    rayleigh->setStep( gas->h( ) );

    params->setAmount( init_dialog->amount( ) );
    params->setMax_x( init_dialog->max_x( ) );
    params->setMax_y( init_dialog->max_y( ) );
    params->setRadius( init_dialog->radius( ) );
    params->setMass( init_dialog->mass( ) );
    
    // ------------------------------------------
    // компоновка элементов управления
    // ------------------------------------------
    QGridLayout* grid = new QGridLayout;
    grid->addWidget( listWidget, 0, 0 );
    grid->addLayout( stackedLayout, 0, 1 );
    grid->addWidget( process, 1, 0 );
    grid->addWidget( statistic, 1, 1 );
    
    setLayout( grid );
    // ------------------------------------------
    gas->step_by( 1000000 );
}

Simulator::~Simulator( )
{
    if (gas) delete gas;
}

void Simulator::startProcessing( )
{
    if (timer->isActive( ))
    {
        process->setText( "START" );
        timer->stop( );
        gas->gather_statistic( );
        statistic->setEvents( gas->crossings( ) + gas->collisions( ) );
        statistic->setCrossings( gas->crossings( ) );
        statistic->setCollisions( gas->collisions( ) );
        statistic->setSpeed( gas->average_speed( ) );
        statistic->setEnergy( gas->k_energy( ) );
        statistic->repaint( );
        visualizer->update( );
    }
    else
    {
        process->setText( "STOP" );
        timer->start( 1 );
        dt = process->delta( );
    }
}

void Simulator::onTimer( )
{
    gas->step_by( dt );
    gas->gather_statistic( );
    statistic->setEvents( gas->crossings( ) + gas->collisions( ) );
    statistic->setCrossings( gas->crossings( ) );
    statistic->setCollisions( gas->collisions( ) );
    statistic->setSpeed( gas->average_speed( ) );
    statistic->setEnergy( gas->k_energy( ) );
    statistic->repaint( );
    visualizer->update( );
        
    rayleigh->setExpectedValue( gas->expected_value( ) );
    rayleigh->setVariance( gas->var( ) );
}

void Simulator::zero( )
{
    gas->reset_all( );
}
