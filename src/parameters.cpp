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
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "parameters.h"

Parameters::Parameters( QWidget* parent ) : QWidget( parent )
{
    QLabel* amountLabel = new QLabel( tr( "Particles amount:" ) );
    QLabel* widthLabel = new QLabel( "Width (X):" );
    QLabel* heightLabel = new QLabel( "Height (Y):" );
    QLabel* radiusLabel = new QLabel( "Radius:" );
    QLabel* massLabel = new QLabel( "Mass:" );
    
    amountEdit = new QLineEdit;
    widthEdit = new QLineEdit;
    heightEdit = new QLineEdit;
    radiusEdit = new QLineEdit;
    massEdit = new QLineEdit;
    
    QPushButton* newButton = new QPushButton( "New parameters" );

    amountEdit->setReadOnly( true );
    amountEdit->setMaximumWidth( 100 );
    amountEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    amountLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    widthEdit->setReadOnly( true );
    widthEdit->setMaximumWidth( 100 );
    widthEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    widthLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    heightEdit->setReadOnly( true );
    heightEdit->setMaximumWidth( 100 );
    heightEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    heightLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    radiusEdit->setReadOnly( true );
    radiusEdit->setMaximumWidth( 50 );
    radiusEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    radiusLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    massEdit->setReadOnly( true );
    massEdit->setMaximumWidth( 50 );
    massEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    massLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    newButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    // -------------------------------------------
    
    connect( newButton, SIGNAL( clicked( ) ), 
             this, SIGNAL( onNewParameters( ) ) );
    
    // -------------------------------------------
    // компоновка элементов управления
    // -------------------------------
    QVBoxLayout* left = new QVBoxLayout;
    left->addWidget( amountLabel );
    left->addWidget( widthLabel );
    left->addWidget( heightLabel );
    left->addWidget( radiusLabel );
    left->addWidget( massLabel );
    
    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget( amountEdit );
    right->addWidget( widthEdit );
    right->addWidget( heightEdit );
    right->addWidget( radiusEdit );
    right->addWidget( massEdit );
    
    QHBoxLayout* top = new QHBoxLayout;
    top->addStretch( );
    top->addLayout( left );
    top->addLayout( right );
    top->addStretch( );
    top->setSpacing( 5 );
    
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout( top );
    mainLayout->addStretch( );
    mainLayout->addWidget( newButton );
    
    setLayout( mainLayout );
}

void Parameters::setAmount( int amt )
{
    amountEdit->setText( QString::number( amt ) );
}

void Parameters::setMax_x( int max_x )
{
    widthEdit->setText( QString::number( max_x ) );
}

void Parameters::setMax_y( int max_y )
{
    heightEdit->setText( QString::number( max_y ) );
}

void Parameters::setRadius( double r )
{
    radiusEdit->setText( QString::number( r, 'F', 2 ) );
}

void Parameters::setMass( double m )
{
    massEdit->setText( QString::number( m, 'F', 2 ) );
}
