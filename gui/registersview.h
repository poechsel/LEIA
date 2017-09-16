#ifndef REGISTERSVIEW_H
#define REGISTERSVIEW_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QMenu>
#include "../simulateur/structs.h"

class RegistersView : public QTableWidget
{
    Q_OBJECT
public:
    RegistersView(QWidget *parent = 0);
    void update();
    void updateOptimize(QVector<int> indices);
    void setMachine(Machine &machine);


public slots:
    void editCell(int row, int column);
void showContextMenu(const QPoint &pos);
private:
    Machine *_machine;
    void _updateRow(int row, int value);
};

#endif // REGISTERSVIEW_H
