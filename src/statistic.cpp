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

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "statistic.h"

Statistic::Statistic( QWidget *parent ) : QGroupBox( "Statistic", parent )
{
    QLabel* eventsLabel = new QLabel( "events:" );
    QLabel* crossingsLabel = new QLabel( "crossings:" );
    QLabel* collisionsLabel = new QLabel( "collisions:" );
    QLabel* energyLabel = new QLabel( "average energy:" );
    QLabel* speedLabel = new QLabel( "average speed:" );
    
    events = new QLineEdit( "0" );
    crossings = new QLineEdit( "0" );
    collisions = new QLineEdit( "0" );
    energy = new QLineEdit( "0.0" );
    speed = new QLineEdit( "0.0" );
    
    events->setReadOnly( true );
    events->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    eventsLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    crossings->setReadOnly( true );
    crossings->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    crossingsLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    collisions->setReadOnly( true );
    collisions->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    collisionsLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    energy->setReadOnly( true );
    energy->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    energyLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    speed->setReadOnly( true );
    speed->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    speedLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    QVBoxLayout* leftLabels = new QVBoxLayout;
    leftLabels->addWidget( eventsLabel );
    leftLabels->addWidget( crossingsLabel );
    leftLabels->addWidget( collisionsLabel );
    
    QVBoxLayout* leftEdits = new QVBoxLayout;
    leftEdits->addWidget( events );
    leftEdits->addWidget( crossings);
    leftEdits->addWidget( collisions );
    
    QVBoxLayout* rightLabels = new QVBoxLayout;
    rightLabels->addWidget( energyLabel );
    rightLabels->addWidget( speedLabel );
    
    QVBoxLayout* rightEdits = new QVBoxLayout;
    rightEdits->addWidget( energy );
    rightEdits->addWidget( speed );
    
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout( leftLabels );
    mainLayout->addLayout( leftEdits );
    mainLayout->addStretch( );
    mainLayout->addLayout( rightLabels );
    mainLayout->addLayout( rightEdits );
    
    setLayout( mainLayout );
    resize( sizeHint( ) );
}

void Statistic::setEvents( int new_events )
{
    events->setText( QString::number( new_events ) );
}

void Statistic::setCrossings( int new_crossings )
{
    crossings->setText( QString::number( new_crossings ) );
}

void Statistic::setCollisions( int new_collisions )
{
    collisions->setText( QString::number( new_collisions ) );
}

void Statistic::setEnergy( double new_energy )
{
    energy->setText( QString::number( new_energy, 'f', 15 ) );
}

void Statistic::setSpeed( double new_speed )
{
    speed->setText( QString::number( new_speed, 'f', 15 ) );
}
