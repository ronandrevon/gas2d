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
#include <QSpinBox>
#include <QGridLayout>

#include "initialize_dialog.h"

Initialize_dialog::Initialize_dialog( QWidget* parent ) : QDialog( parent )
{
    QLabel* amountLabel = new QLabel( tr( "Particles amount:" ) );
    QLabel* widthLabel = new QLabel( "Width (X):" );
    QLabel* heightLabel = new QLabel( "Height (Y):" );
    QLabel* radiusLabel = new QLabel( "Radius:" );
    QLabel* massLabel = new QLabel( "Mass:" );
    
    amountLineEdit = new QLineEdit( "10" );
    widthSpin = new QSpinBox;
    heightSpin = new QSpinBox;
    radiusSpin = new QDoubleSpinBox;
    massSpin = new QDoubleSpinBox;
    
    okButton = new QPushButton( "OK" );
    QPushButton* cancelButton = new QPushButton( "cancel" );
    amountValidator = new QIntValidator( 1, 100, this );
    
    // -------------------------------------------
    // настройка элементов управления
    // ------------------------------
    widthSpin->setRange( 10, 1000 );
    widthSpin->setAccelerated( true );
    
    heightSpin->setRange( 10, 1000 );
    heightSpin->setAccelerated( true );
    
    radiusSpin->setRange( 0.01, 0.5 );
    radiusSpin->setSingleStep( 0.01 );
    radiusSpin->setAccelerated( true );
    radiusSpin->setValue( 0.4 );
    
    massSpin->setRange( 0.1, 5.0 );
    massSpin->setSingleStep( 0.01 );
    massSpin->setAccelerated( true );
    massSpin->setValue( 1.0 );
    // -------------------------------------------
    
    // -------------------------------------------
    // связывание сигналов и слотов
    // ----------------------------
    connect( okButton, SIGNAL( clicked( ) ), this, SLOT( accept( ) ) );
    connect( cancelButton, SIGNAL( clicked( ) ), this, SLOT( reject( ) ) );
    connect( amountLineEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( on_amount_changed( const QString& ) ) );
    connect( widthSpin, SIGNAL( valueChanged( int ) ),
             this, SLOT( on_width_changed( int ) ) );
    connect( heightSpin, SIGNAL( valueChanged( int ) ),
             this, SLOT( on_height_changed( int ) ) );
    // -------------------------------------------
    
    // -------------------------------------------
    // компоновка элементов управления
    // -------------------------------
    QGridLayout* topLayout = new QGridLayout;
    topLayout->addWidget( amountLabel, 0, 0 );
    topLayout->addWidget( amountLineEdit, 0, 1, 1, 2 );
    
    topLayout->addWidget( widthLabel, 2, 0 );
    topLayout->addWidget( widthSpin, 2, 1 );
    topLayout->addWidget( heightLabel, 3, 0 );
    topLayout->addWidget( heightSpin, 3, 1 );
    
    topLayout->addWidget( radiusLabel, 5, 0 );
    topLayout->addWidget( radiusSpin, 5, 1 );
    topLayout->addWidget( massLabel, 6, 0 );
    topLayout->addWidget( massSpin, 6, 1 );
    
    topLayout->addWidget( okButton, 8, 0 );
    topLayout->addWidget( cancelButton, 8, 2 );
    // -------------------------------------------
    
    setLayout( topLayout );
    okButton->setMaximumSize( okButton->sizeHint( ) );
    setMaximumSize( sizeHint( ) );
    setWindowTitle( tr("Initialize") );
}

// -------------------------------------------------------------
// Слот проверяет корректность текста в поле ввода количества
// частиц. Вызывается при изменении текста в поле.
// -------------------------------------------------------------
void Initialize_dialog::on_amount_changed( const QString& text )
{
    int pos = 0;
    QString str( text );
    okButton->setEnabled( amountValidator->validate( str, pos ) == 
            QValidator::Acceptable );
}

void Initialize_dialog::on_width_changed( int new_width )
{
    amountValidator->setTop( new_width * heightSpin->value( ) );
    on_amount_changed( amountLineEdit->text( ) );
}

void Initialize_dialog::on_height_changed( int new_height )
{
    amountValidator->setTop( widthSpin->value( ) * new_height );
    on_amount_changed( amountLineEdit->text( ) );
}
