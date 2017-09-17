#include "codeview.h"

CodeView::CodeView(QWidget *parent):
    QListWidget(parent), _machine(0)
{
    this->setUniformItemSizes(true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));
}
void CodeView::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Voir l'instruction en cours d'execution", this);
   QAction action2("Voir l'instruction suivante", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(setPositionToCurrent()));
   connect(&action2, SIGNAL(triggered()), this, SLOT(setPositionToNext()));
   contextMenu.addAction(&action1);
   if (this->_machine) {
        int index = this->currentRow();
        if (0 <= index && index < 0x10000) {
            uword opcode = this->_machine->memory[index];
            int next_index = index + 1;
            if ((opcode >> 12) == 0b0011 || (opcode == 0xb001)) {
            } else {
                contextMenu.addAction(&action2);
            }
        }
   }
   contextMenu.exec(mapToGlobal(pos));
}

void CodeView::setPositionToCurrent() {
    if (this->_machine) {
        this->setCurrentRow(this->_machine->pc);
    }
}

void CodeView::setPositionToNext() {
    if (this->_machine) {
        int index = this->currentRow();
        if (0 <= index && index < 0x10000) {
            uword opcode = this->_machine->memory[index];
            int next_index = index + 1;
            if ((opcode >> 12) == 0b1010) {
                next_index = (opcode & 0xfff) * 16;
            } else if ((opcode >> 12) == 0b1011) {
                word second_part = (opcode & 0xfff);
                if (second_part == 1) {
                    next_index = _machine->registers[15];
                } else {
                    next_index = index + second_part;
                }
            } else if ((opcode >> 12) == 0b0011) {
                next_index = index;
            }

            if (0 <= index && index < 0x10000)
                this->setCurrentRow(next_index);
        }
    }
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
