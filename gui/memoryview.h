#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QObject>
#include <QWidget>
#include "dataview.h"
#include <QMenu>

class MemoryView : public DataView
{
    Q_OBJECT
public:
    MemoryView(QWidget *parent = 0);
signals:
    void switchToCode(int);
private slots:
    void showContextMenu(const QPoint &pos);
    void requestSwitchToCode();
private:
    int _selected_row;
};

#endif // MEMORYVIEW_H
