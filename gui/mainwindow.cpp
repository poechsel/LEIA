#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("LEIA debugger");

    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
    QAction *action_open_file = new QAction(tr("&Open"), this);
    QAction *action_close = new QAction(tr("&Close"), this);
    connect(action_open_file, &QAction::triggered, this, &MainWindow::open_file);
    connect(action_close, SIGNAL(triggered(bool)), this, SLOT(closeCustom(bool)));
    menuFichier->addAction(action_open_file);
    menuFichier->addAction(action_close);



    QSplitter *layout = new QSplitter(Qt::Horizontal);

    QSplitter *layout_views = new QSplitter(Qt::Vertical);
    _memory_view = new MemoryView;
    layout_views->addWidget(_memory_view);
    _registers_view = new RegistersView;
    layout_views->addWidget(_registers_view);
    layout->addWidget(layout_views);

    _code_view = new CodeView;
    _code_view->setLabels(_labels);
    layout->addWidget(_code_view);


        _screen_view = new QtScreen;

        QVBoxLayout *layout_w_controls = new QVBoxLayout;
        QWidget *widget_controls = new QWidget;
        widget_controls->setLayout(layout_w_controls);
        layout->addWidget(widget_controls);

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

        AspectRatioSingleItemLayout *screen_layout = new AspectRatioSingleItemLayout(WIDTH/HEIGHT);
        screen_layout->addWidget(_screen_view);
        layout_w_controls->addLayout(screen_layout);
        _console = new QTextEdit;
        _console->setReadOnly(true);
        layout_w_controls->addWidget(_console);


    _screen_view->resize(120, 340);


    setCentralWidget(layout);

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
    connect(_code_view, SIGNAL(switchToMemory(int)), _memory_view, SLOT(focusOn(int)));
    connect(_memory_view, SIGNAL(switchToCode(int)), _code_view, SLOT(setPosition(int)));

    connect(this, SIGNAL(consoleUpdate(QString)), _console, SLOT(insertPlainText(QString)));

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
             (this->_use_breakpoints && this->_code_view->isBreakpoint(_machine.pc)/*&& this->_code_view->item(_machine.pc)->checkState() == Qt::Checked*/)
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
    std::string out = evaluate(_machine.memory[_machine.pc], _machine, _param, (Screen*) _screen_view);
    QString out_q = QString::fromStdString(out);
    if (out_q != "")
        emit consoleUpdate(out_q);
    if ((opcode >> 12) == 0b0000) {
       return _machine.registers[toUWord(opcode)];
    }
    return -1;
}


void MainWindow::openDebugInformations(QFile &file) {
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&file);
    QString label;
    int position;

    QString previous_label = "";
    int index = 0;
    while (!in.atEnd()) {
        in >> label >> position;
        if (label == "")
            position = 0x10000;
        for (; index < position && index < 0x10000; index++) {
            _labels[index] = previous_label;
        }
        previous_label = label;
        qDebug()<<label<<" "<<position<<"\n";
    }
    for (; index < position && index < 0x10000; index++) {
        _labels[index] = previous_label;
    }
}

void MainWindow::open_file() {
    disconnect(_memory_view, SIGNAL(cellChanged(int,int)), _memory_view, SLOT(editCell(int,int)));
    QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open code"), _last_path, "");

    int extension_pos = file_name.lastIndexOf('.');
    QString debug_path = file_name.leftRef(extension_pos) + ".debug";
    QFile debug_file(debug_path);
    if (debug_file.exists()) {
        auto reply = QMessageBox::question(this, "Fichier de débug", "Un fichier contenant des informations de débug à été détecté. Voulez vous l'utiliser?",
                                QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            this->openDebugInformations(debug_file);
        } else {
            for (int i = 0; i < 0x10000; ++i)
                _labels[i] = "";
        }
    }

    QFile file(file_name);
    _last_path = QDir(file_name).absolutePath();
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _code.clear();
    readFromStr(file_name.toStdString(), _machine);
    _machine.pc = 0;
    _console->setText("");
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
