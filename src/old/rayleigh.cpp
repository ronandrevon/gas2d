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
#include <QCheckBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <cmath>
#include "rayleigh.h"

Rayleigh::Rayleigh( const double* histogram, int n, QWidget *parent ) 
	: QWidget( parent )
{
	QCheckBox* rayleighCheck = new QCheckBox( "Rayleigh" );
	
	QLabel* scaleLabel = new QLabel( "scale:" );
	QLabel* expectedLabel = new QLabel( "expected value:" );
	QLabel* varLabel = new QLabel( "variance:" );
	
	QSpinBox* scaleSpin = new QSpinBox;
	expectedEdit = new QLineEdit;
	varEdit = new QLineEdit( "1.0" );
	
	plotter = new Plotter;
	
	// -----------------------------------------------
	// настройка политики изменения размеров элементов
	expectedLabel->setSizePolicy( QSizePolicy::Fixed, 
				      QSizePolicy::Fixed );
	varLabel->setSizePolicy( QSizePolicy::Fixed, 
				 QSizePolicy::Fixed );
	scaleLabel->setSizePolicy( QSizePolicy::Fixed, 
				   QSizePolicy::Fixed );
	expectedEdit->setSizePolicy( QSizePolicy::Expanding,
				     QSizePolicy::Fixed );
	varEdit->setSizePolicy( QSizePolicy::Expanding,
				QSizePolicy::Fixed );
	plotter->setSizePolicy( QSizePolicy::Expanding,
				QSizePolicy::Expanding );
	// -----------------------------------------------
	
	// настройка начальных состояний элементов
	rayleighCheck->setCheckState( Qt::Unchecked );
	scaleSpin->setRange( 1, 80 );
	scaleSpin->setValue( 1 );
	plotter->setHistogram( histogram, n );
	plotter->setScale( double(scaleSpin->value( )) );
	plotter->setFunc( rayleighFunc );
	plotter->setShowDistr( false );
	expectedEdit->setReadOnly( true );
	varEdit->setReadOnly( true );
	// -----------------------------------------------
    
    // подключение сигналов
    connect( rayleighCheck, SIGNAL( stateChanged( int ) ),
             this, SLOT( rayleighShow( int ) ) );
    connect( scaleSpin, SIGNAL( valueChanged( int ) ),
             this, SLOT( scaleChanged( int ) ) );
	
	// компоновка элементов управления
	QVBoxLayout* rightLabels = new QVBoxLayout;
	rightLabels->addWidget( expectedLabel );
	rightLabels->addWidget( varLabel );
	
	QVBoxLayout* rightEdits = new QVBoxLayout;
	rightEdits->addWidget( expectedEdit );
	rightEdits->addWidget( varEdit );
	
	QGridLayout* leftLayout = new QGridLayout;
	leftLayout->addWidget( rayleighCheck, 0, 0, 1, 2 );
	leftLayout->addWidget( scaleLabel, 1, 0 );
	leftLayout->addWidget( scaleSpin, 1, 1 );
	
	QHBoxLayout* bottomLayout = new QHBoxLayout;
	bottomLayout->addLayout( leftLayout );
	bottomLayout->addStretch( );
	bottomLayout->addLayout( rightLabels );
	bottomLayout->addLayout( rightEdits );
	
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget( plotter );
	mainLayout->addLayout( bottomLayout );
	
	setLayout( mainLayout );
}

void Rayleigh::setVariance( double new_var )
{
    plotter->setVar( new_var );
    varEdit->setText( QString::number( new_var, 'F', 15 ) );
}

void Rayleigh::setExpectedValue( double expected_val )
{
    expectedEdit->setText( QString::number( expected_val ) );
}

void Rayleigh::setStep( double h )
{
    plotter->setStep( h );
}

void Rayleigh::rayleighShow( int state )
{
    plotter->setShowDistr( bool(state) );
    plotter->repaint( );
}

void Rayleigh::scaleChanged( int val )
{
    plotter->setScale( val );
    plotter->repaint( );
}

double Rayleigh::rayleighFunc( double x, double Dx )
{
	return (x * std::exp( -(x*x)/(2*Dx) )) / Dx;
}