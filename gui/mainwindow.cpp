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

 CodeModel *a = new CodeModel;


    QHBoxLayout *layout = new QHBoxLayout;

    QStringList list;
    list << "a" << "b" << "c";

//    http://www.walletfox.com/course/qtcheckablelist.php
    _list_view = new QListWidget;
    _list_view->addItems(list);

     QListWidgetItem* item = 0;
    for(int i = 0; i < _list_view->count(); ++i){
        item = _list_view->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    layout->addWidget(nom);

    layout->addWidget(_list_view);

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
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        line.trimmed();
        _code.append(line);
    }

    _list_view->clear();
    _list_view->addItems(_code);
     QListWidgetItem* item = 0;
    for(int i = 0; i < _list_view->count(); ++i){
        item = _list_view->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

    item->setTextAlignment(Qt::AlignVCenter);
    }
}

MainWindow::~MainWindow()
{
}
