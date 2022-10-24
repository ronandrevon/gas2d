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

#include <QWidget>
#include <QTimer>

#include "initialize_dialog.h"
#include "parameters.h"
#include "rayleigh.h"
#include "processing.h"
#include "statistic.h"
#include "gas_simulator.h"
#include "visualizer_tab.h"

class Simulator : public QWidget
{
    Q_OBJECT
public:
    Simulator( QWidget *parent = 0 );
    ~Simulator( );
    
private slots:
    void startProcessing( );
    void onTimer( );
    void zero( );
//    void newParams( );
    
private:
    QTimer *timer;
    Initialize_dialog *init_dialog;
    Parameters *params;
    Rayleigh *rayleigh;
    Process_tab *process;
    Statistic *statistic;
    Gas_simulator *gas;
    Visualizer_tab* visualizer;
    
    double dt;
};