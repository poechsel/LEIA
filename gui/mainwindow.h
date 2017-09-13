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
#include "sdlwidget.h"
#include "../simulateur/simulateur.h"
#include <QtConcurrent/QtConcurrent>
#include <QGroupBox>
#include <QPushButton>

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
private:
    QStringList _code;
    QListWidget *_list_view;
    CodeView *_code_view;
    MemoryView *_memory_view;
    SDLWidget *_screen_view;

private slots:
   void open_file();
};

#endif // MAINWINDOW_H
