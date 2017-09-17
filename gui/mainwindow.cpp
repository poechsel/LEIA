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

    QVBoxLayout *layout_views = new QVBoxLayout;
    _memory_view = new DataView(0x10000);
    layout_views->addWidget(_memory_view);
    _registers_view = new RegistersView;
    layout_views->addWidget(_registers_view);
    layout->addLayout(layout_views);

    _code_view = new CodeView;
    layout->addWidget(_code_view);


        _screen_view = new QtScreen;

        QVBoxLayout *layout_w_controls = new QVBoxLayout;
        layout->addLayout(layout_w_controls);

        QGroupBox* panel_control = new QGroupBox("Contrôles");
        panel_control->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        layout_w_controls->addWidget(panel_control);
        QGridLayout* grid_control_layout = new QGridLayout;
        grid_control_layout->setSizeConstraint(QLayout::SetMinimumSize);

        _button_single_step = new QPushButton("Single");
        _button_next_step = new QPushButton("Next");
        _button_breakpoint = new QPushButton("Break");
        _button_play = new QPushButton("Play");
        _button_stop = new QPushButton("Stop");
        grid_control_layout->addWidget(_button_single_step, 0, 0);
        grid_control_layout->addWidget(_button_next_step, 0, 1);
        grid_control_layout->addWidget(_button_breakpoint, 0, 2);
        grid_control_layout->addWidget(_button_play, 1, 0);
        grid_control_layout->addWidget(_button_stop, 1, 1);

        panel_control->setLayout(grid_control_layout);
        layout_w_controls->addWidget(_screen_view);

    _screen_view->resize(120, 340);


    zoneCentrale->setLayout(layout);
    setCentralWidget(zoneCentrale);

    connect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));
    connect(_registers_view, SIGNAL(cellChanged(int,int)), _registers_view, SLOT(editCell(int,int)));
    connect(_memory_view, SIGNAL(memoryChanged(int)), _code_view, SLOT(updateCode(int)));

    connect(_button_single_step, SIGNAL(released()), this, SLOT(simulateSingleStep()));
    connect(_button_next_step, SIGNAL(released()), this, SLOT(simulateSingleStepJumpCall()));
    connect(_button_breakpoint, SIGNAL(released()), this, SLOT(simulateNextBreakpoint()));
    connect(_button_play, SIGNAL(released()), this, SLOT(simulationStart()));
    connect(_button_stop, SIGNAL(released()), this, SLOT(simulationStop()));
    connect(this, SIGNAL(workingEnd()), this, SLOT(activateSimulateControls()));

    connect(_registers_view, SIGNAL(sendSelected(int)), _memory_view, SLOT(focusOn(int)));

    activateSimulateControls();
}

void MainWindow::simulateSingleStep() {
    int w = this->evaluateAndMem();
    QVector<int> indices;
    if (w >= 0) {
        indices.push_back(w);
    }
    _code_view->updateOptimize(indices);
    _memory_view->updateOptimize(indices);
    _registers_view->update();
}

void MainWindow::simulateSingleStepJumpCall() {
    uword previous_pc = -1;
    _param.skip_call = true;
    _machine.in_call = false;
    QVector<int> indices;
    do {
        previous_pc = _machine.pc;
        int w = this->evaluateAndMem();
        if (w >= 0)
            indices.push_back(w);
    } while (_machine.in_call && previous_pc != _machine.pc);
    _param.skip_call = false;
    _code_view->updateOptimize(indices);
    _memory_view->updateOptimize(indices);
    _registers_view->update();
}
void MainWindow::simulateNextBreakpoint() {
    this->deactivateSimulateControls();
    this->_use_breakpoints = true;
    this->_stop_simulation = false;
    QtConcurrent::run(this, &MainWindow::_simulateNextBreakpoint_worker);

}

void MainWindow::deactivateSimulateControls() {
    this->_button_breakpoint->setEnabled(false);
    this->_button_single_step->setEnabled(false);
    this->_button_next_step->setEnabled(false);
    this->_button_stop->setEnabled(true);
    this->_button_play->setEnabled(false);
}
void MainWindow::activateSimulateControls() {
    this->_button_breakpoint->setEnabled(true);
    this->_button_single_step->setEnabled(true);
    this->_button_next_step->setEnabled(true);
    this->_button_stop->setEnabled(false);
    this->_button_play->setEnabled(true);
}

void MainWindow::_simulateNextBreakpoint_worker() {
    uword previous_pc = -1;
    QVector<int> indices;
    bool indices_cache[0x10000];
    for (int i = 0; i < 0x10000; ++i)
        indices_cache[i] = false;
    do {
        previous_pc = _machine.pc;
        int w = this->evaluateAndMem();
        if (w >= 0 && !indices_cache[w]) {
            indices.push_back(w);
            indices_cache[w] = true;
        }
    } while (!(
             (this->_use_breakpoints && this->_code_view->item(_machine.pc)->checkState() == Qt::Checked)
         || previous_pc == _machine.pc
         || this->_stop_simulation
             ));
    _code_view->updateOptimize(indices);
    _memory_view->updateOptimize(indices);
    _registers_view->update();
    this->activateSimulateControls();
}

void MainWindow::simulationStart() {
    qDebug()<<"starting\n";
    this->deactivateSimulateControls();
    this->_use_breakpoints = false;
    this->_stop_simulation = false;
    QtConcurrent::run(this, &MainWindow::_simulateNextBreakpoint_worker);

}

void MainWindow::simulationStop() {
    this->_stop_simulation = true;
}

int MainWindow::evaluateAndMem() {
    auto opcode = _machine.memory[_machine.pc];
    evaluate(_machine.memory[_machine.pc], _machine, _param, (Screen*) _screen_view);
    if ((opcode >> 12) == 0b0000) {
       return _machine.registers[toUWord(opcode)];
    }
    return -1;
}

void MainWindow::open_file() {
    disconnect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));
    QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open code"), "/home/pierre/Demo-ASR/demo/", "");

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _code.clear();
    readFromStr(file_name.toStdString(), _machine);
    loadCodeToMemory(_machine);

    _code_view->setMachine(_machine);
    _memory_view->setData(_machine.memory);
    _registers_view->setData(_machine.registers);
    _code_view->update();
    _memory_view->update();
    _registers_view->update();
    connect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));

    ClockTicks ct = clockticks_new();
        _param.step_by_step = false;
        _param.debug_output = false;
        _param.fast_mode = true;
        _param.full_debug = false;
        _param.skip_call = false;
        _machine.clock_ticks = ct;

      // QtConcurrent::run(this, &MainWindow::simulate, _param, _machine);
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
