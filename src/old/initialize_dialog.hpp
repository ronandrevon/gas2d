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

#ifndef _INITIALIZE_DIALOG_H_
#define _INITIALIZE_DIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QIntValidator>

class Initialize_dialog : public QDialog
{
    Q_OBJECT
private:
    // основные элементы управления
    QLineEdit* amountLineEdit;
    QSpinBox* widthSpin;
    QSpinBox* heightSpin;
    QDoubleSpinBox* radiusSpin;
    QDoubleSpinBox* massSpin;
    
public:
    Initialize_dialog( QWidget* parent = 0 );
    
    int amount( ) const { return amountLineEdit->text( ).toInt( ); }
    int max_x( ) const { return widthSpin->value( ); }
    int max_y( ) const { return heightSpin->value( ); }
    double radius( ) const { return radiusSpin->value( ); }
    double mass( ) const { return massSpin->value( ); }
    
private slots:
    void on_amount_changed( const QString& text );
    void on_width_changed( int new_width );
    void on_height_changed( int new_height );

private:
    QPushButton* okButton;
    QIntValidator* amountValidator;
};

#endif // _INITIALIZE_DIALOG_H_
