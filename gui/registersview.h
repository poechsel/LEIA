#ifndef REGISTERSVIEW_H
#define REGISTERSVIEW_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QMenu>
#include "../simulateur/structs.h"
#include "dataview.h"

class RegistersView : public DataView
{
    Q_OBJECT
public:
    RegistersView(QWidget *parent = 0);

public slots:
void showContextMenu(const QPoint &pos);
private slots:
void switchToMemorySelected();

signals:
    void sendSelected(int row);
private:
    int _selected_row;
};

#endif // REGISTERSVIEW_H
