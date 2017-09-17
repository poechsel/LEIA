#ifndef CODEVIEW_H
#define CODEVIEW_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include "../simulateur/structs.h"
#include "../simulateur/debug.h"
#include <QMenu>
#include <QTableWidget>
#include <QHeaderView>

class CodeView: public QTableWidget
{
    Q_OBJECT
public:
    CodeView(QWidget *parent = 0);
    void setMachine(Machine &machine);
    void update();
    void updateOptimize(QVector<int> indices);
    bool isBreakpoint(int row);
    void setLabels(QString *labels) {_labels = labels;}

protected:
    QString rowToString(int row);
    void refresh();
    Machine *_machine;
    QString *_labels;

signals:
    void switchToMemory(int);

public slots:
    void updateCode(int row);
    void setPosition(int);
    void showContextMenu(const QPoint &);
    void setPositionToCurrent();
    void requestSwitchToMemory();
    void setPositionToNext();
};

#endif // CODEVIEW_H
