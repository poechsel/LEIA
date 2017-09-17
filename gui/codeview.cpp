#include "codeview.h"

CodeView::CodeView(QWidget *parent):
    QListWidget(parent), _machine(0)
{
    this->setUniformItemSizes(true);
}


void CodeView::update() {
    QStringList code;
    for (int i = 0; i < 0x10000; ++i) {
        code.append(rowToString(i));
    }
    this->clear();
    this->addItems(code);
    this->refresh();
    if (this->_machine)
    this->setCurrentRow(this->_machine->pc);
}

QString CodeView::rowToString(int row) {
    if (0 <= row && row < 0x10000) {
        QString dis = QString::fromStdString(dissassemble(_machine->memory[row]));
        uword operation = _machine->memory[row];
        if ((operation >> 12) == 0b1010) {
            int index = (operation & 0b111111111111) * 16;
            if (_labels[index] != "") {
                dis = "call " + _labels[index];
            }
        }
        return QString("[") + QString::number(row) + QString("]") + _labels[row] + ": " + dis;
    }
}

void CodeView::updateOptimize(QVector<int> indices) {
    if (this->_machine) {
        for (int t : indices) {
            this->item(t)->setText(rowToString(t));
        }
        this->setCurrentRow(this->_machine->pc);
    }
}

void CodeView::updateCode(int row) {
    if (!_machine) return;
    if (row < this->count())
        this->item(row)->setText(rowToString(row));
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
