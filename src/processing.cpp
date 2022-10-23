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

#include "processing.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <limits.h>

Process_tab::Process_tab( QWidget *parent ) : QGroupBox( "Process", parent )
{
    QLabel* deltaLabel = new QLabel( "delta:" );
    deltaLineEdit = new QLineEdit( "1000" );
    startButton = new QPushButton( "START" );
    
    QIntValidator* intValidator = new QIntValidator( 1, INT_MAX, this );
    deltaLineEdit->setValidator( intValidator );
    
    startButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    connect( startButton, SIGNAL( clicked( ) ),
             this, SIGNAL( startClicked( ) ) );
    connect( deltaLineEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( deltaChanged( const QString& ) ) );
    
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget( deltaLabel );
    topLayout->addWidget( deltaLineEdit );
    
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout( topLayout );
    mainLayout->addStretch( );
    mainLayout->addWidget( startButton );
    mainLayout->setAlignment( startButton, Qt::AlignHCenter );
    
    setLayout( mainLayout );
    resize( sizeHint( ) );
}

void Process_tab::setText( const QString& text )
{
    startButton->setText( text );
}

int Process_tab::delta( ) const
{
    return deltaLineEdit->text( ).toInt( );
}

void Process_tab::deltaChanged( const QString& text )
{
    startButton->setEnabled( !(text.isEmpty( )));
}
