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

//    http://www.walletfox.com/course/qtcheckablelist.php
    layout->addWidget(nom);

    _code_view = new CodeView;
    layout->addWidget(_code_view);

    layout->addWidget(age);


    zoneCentrale->setLayout(layout);
    setCentralWidget(zoneCentrale);
}

void MainWindow::open_file() {
    QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open code"), "/home/", "");

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    _code.clear();
    Machine machine;
    readFromStr(file_name.toStdString(), machine);
    loadCodeToMemory(machine);

    _code_view->update(machine);
}

MainWindow::~MainWindow()
{
}
