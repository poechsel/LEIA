#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QFormLayout>
#include <QListWidget>
#include <QStringListModel>
#include <QFileDialog>
#include "codeview.h"
#include "../simulateur/utilities.h"
#include "../simulateur/debug.h"
#include "memoryview.h"
#include "qtscreen.h"
#include "../simulateur/simulateur.h"
#include <QtConcurrent/QtConcurrent>
#include <QGroupBox>
#include <QPushButton>
#include "registersview.h"

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void simulate(Param &param, Machine &machine);

public slots:
    void simulateSingleStep();
    void simulateSingleStepJumpCall();
    void simulateNextBreakpoint();
    void simulationStart();
    void simulationStop();

    void activateSimulateControls();
    void deactivateSimulateControls();

signals:
    void workingEnd();

private:
    int evaluateAndMem();
void _simulateNextBreakpoint_worker();


    QStringList _code;
    QListWidget *_list_view;
    CodeView *_code_view;
    MemoryView *_memory_view;
    RegistersView *_registers_view;
    QtScreen *_screen_view;



        QPushButton* _button_single_step;
        QPushButton* _button_next_step;
        QPushButton* _button_breakpoint;
        QPushButton* _button_play;
        QPushButton* _button_stop;


        bool _use_breakpoints;
        bool _stop_simulation;

    Machine _machine;
    Param _param;

private slots:
   void open_file();
};

#endif // MAINWINDOW_H
