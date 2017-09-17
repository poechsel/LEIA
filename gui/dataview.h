#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QWidget>
#include <QObject>
#include <QTableWidget>
#include "../simulateur/structs.h"

class DataView: public QTableWidget
{
    Q_OBJECT
public:
    DataView(int size, QString label_formatting = "%1", QWidget *parent = 0);
    void setData(uword *data);
    void update();
    void updateOptimize(QVector<int> indices);

protected:
    void refresh();
    uword *_data;
    int _size;

    void _updateRow(int row, int value);

public slots:
    void editCell(int row, int column);
    void focusOn(int row);


signals:
    void memoryChanged(int row);
};

#endif // DATAVIEW_H
