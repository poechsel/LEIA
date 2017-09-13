#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{


    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
    QAction *action_open_file = new QAction(tr("&Open"), this);
    connect(action_open_file, &QAction::triggered, this, &MainWindow::open_file);
    menuFichier->addAction(action_open_file);

    QMenu *menuEdition = menuBar()->addMenu("&Edition");

    QMenu *menuAffichage = menuBar()->addMenu("&Affichage");
    QWidget *zoneCentrale = new QWidget;
    QLineEdit *nom = new QLineEdit;

    QLineEdit *prenom = new QLineEdit;
    QLineEdit *age = new QLineEdit;



    QHBoxLayout *layout = new QHBoxLayout;

    _memory_view = new MemoryView;
//    http://www.walletfox.com/course/qtcheckablelist.php
    layout->addWidget(_memory_view);

    _code_view = new CodeView;
    layout->addWidget(_code_view);


        _screen_view = new SDLWidget;

        QVBoxLayout *layout_w_controls = new QVBoxLayout;
        layout->addLayout(layout_w_controls);

        QGroupBox* panel_control = new QGroupBox("Contrôles");
        layout_w_controls->addWidget(panel_control);
        QGridLayout* grid_control_layout = new QGridLayout;

        QPushButton* button_single_step = new QPushButton("Single");
        QPushButton* button_next_step = new QPushButton("Next");
        QPushButton* button_breakpoint = new QPushButton("Break");
        QPushButton* button_play = new QPushButton("Play");
        QPushButton* button_stop = new QPushButton("Stop");
        grid_control_layout->addWidget(button_single_step, 0, 0);
        grid_control_layout->addWidget(button_next_step, 0, 1);
        grid_control_layout->addWidget(button_breakpoint, 0, 2);
        grid_control_layout->addWidget(button_play, 1, 0);
        grid_control_layout->addWidget(button_stop, 1, 1);

        panel_control->setLayout(grid_control_layout);

        layout_w_controls->addWidget(_screen_view);

    _screen_view->resize(120, 340);


    zoneCentrale->setLayout(layout);
    setCentralWidget(zoneCentrale);

    connect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));
    connect(_memory_view, SIGNAL(memoryChanged(int)), _code_view, SLOT(updateCode(int)));
}

void MainWindow::open_file() {
    disconnect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));
    QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open code"), "/home/pierre/Demo-ASR/demo/", "");

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _code.clear();
    Machine machine;
    readFromStr(file_name.toStdString(), machine);
    loadCodeToMemory(machine);

    _code_view->setMachine(machine);
    _memory_view->setMachine(machine);
    _code_view->update();
    _memory_view->update();
    connect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));

    ClockTicks ct = clockticks_new();
    Param param;
        param.step_by_step = false;
        param.debug_output = false;
        param.fast_mode = true;
        param.full_debug = false;
        param.skip_call = false;
        machine.clock_ticks = ct;

       QtConcurrent::run(this, &MainWindow::simulate,param, machine);
}

void MainWindow::simulate(Param &param, Machine &machine) {
    machine.pc = 0;
    uword previous_pc = -1;
    while (machine.pc != previous_pc) {
        previous_pc = machine.pc;
            evaluate(machine.memory[machine.pc], machine, param, (Screen*) _screen_view);
    }
    qDebug()<<"quit\n";
}

MainWindow::~MainWindow()
{
}
