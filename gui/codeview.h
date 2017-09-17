#ifndef CODEVIEW_H
#define CODEVIEW_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include "../simulateur/structs.h"
#include "../simulateur/debug.h"

class CodeView: public QListWidget
{
    Q_OBJECT
public:
    CodeView(QWidget *parent = 0);
    void setMachine(Machine &machine);
    void update();
    void updateOptimize(QVector<int> indices);
    void setLabels(QString *labels) {_labels = labels;}

protected:
    QString rowToString(int row);
    void refresh();
    Machine *_machine;
    QString *_labels;

public slots:
    void updateCode(int row);
};

#endif // CODEVIEW_H
