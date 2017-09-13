#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include "../simulateur/structs.h"
#include <QDebug>

class MemoryView : public QTableWidget
{
    Q_OBJECT
public:
    MemoryView(QWidget *parent = 0);
    void update();
    void setMachine(Machine &machine);

public slots:
    void editCell(int row, int column);

private:
    Machine *_machine;
    void _updateRow(int row, int value);

signals:
    void memoryChanged(int row);
};

#endif // MEMORYVIEW_H
