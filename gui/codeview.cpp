#include "codeview.h"

CodeView::CodeView(QWidget *parent):
    QListWidget(parent), _machine(0)
{
    this->setUniformItemSizes(true);
}


void CodeView::update() {
    QStringList code;
    for (int i = 0; i < 0x10000; ++i) {
        code.append(QString("[") + QString::number(i) + QString("]") + QString::fromStdString(dissassemble(_machine->memory[i])));
    }
    this->clear();
    this->addItems(code);
    this->refresh();
    if (this->_machine)
    this->setCurrentRow(this->_machine->pc);
}

void CodeView::updateOptimize(QVector<int> indices) {
    if (this->_machine) {
        for (int t : indices) {
            QString txt = QString("[") + QString::number(t) + QString("]") + QString::fromStdString(dissassemble(_machine->memory[t]));
            this->item(t)->setText(txt);
        }
        this->setCurrentRow(this->_machine->pc);
    }
}

void CodeView::updateCode(int row) {
    if (!_machine) return;
    QString foo = QString::fromStdString(dissassemble(_machine->memory[row]));
    if (row < this->count())
        this->item(row)->setText(QString("[") + QString::number(row) + QString("]") + foo);
}

void CodeView::setMachine(Machine &machine) {
    this->_machine = &machine;
}

void CodeView::refresh() {
    QListWidgetItem* item = 0;
    for(int i = 0; i < this->count(); ++i){
        item = this->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignVCenter);
    }
}
