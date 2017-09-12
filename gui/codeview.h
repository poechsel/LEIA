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
    void update(Machine &machine);

protected:
    void refresh();
};

#endif // CODEVIEW_H
