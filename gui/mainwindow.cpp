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

    layout->addWidget(age);


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
}

MainWindow::~MainWindow()
{
}
