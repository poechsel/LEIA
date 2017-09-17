#include "codeview.h"

CodeView::CodeView(QWidget *parent):
    QTableWidget(parent), _machine(0)
{
    //this->setUniformItemSizes(true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));

    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setShowGrid(false);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
  //  this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setRowCount(0x10000);
    this->setColumnCount(4);
    this->horizontalHeader()->setStretchLastSection(false);
    this->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}
void CodeView::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Voir l'instruction en cours d'execution", this);
   QAction action2("Voir l'instruction suivante", this);
   QAction action3("Voir dans la mémoire", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(setPositionToCurrent()));
   connect(&action2, SIGNAL(triggered()), this, SLOT(setPositionToNext()));
   connect(&action3, SIGNAL(triggered()), this, SLOT(requestSwitchToMemory()));
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
   contextMenu.addAction(&action3);
   contextMenu.exec(mapToGlobal(pos));
}

void CodeView::setPosition(int r) {
    this->setCurrentCell(r, 0);
}

void CodeView::requestSwitchToMemory() {
    emit switchToMemory(this->currentRow());
}

void CodeView::setPositionToCurrent() {
    if (this->_machine) {
        this->setCurrentCell(this->_machine->pc, 0);
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
                this->setCurrentCell(next_index, 0);
        }
    }
}

void CodeView::update() {
    QStringList code;
    for (int i = 0; i < 0x10000; ++i) {
        this->setItem(i, 0, new QTableWidgetItem(""));
        this->setItem(i, 1, new QTableWidgetItem(""));
        this->setItem(i, 2, new QTableWidgetItem(""));
        this->setItem(i, 3, new QTableWidgetItem(""));
        this->rowToString(i);
        //code.append(rowToString(i));
    }
    this->refresh();
    if (this->_machine)
        this->setCurrentCell(this->_machine->pc, 0);
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
        this->item(row, 1)->setText(QString::number(row));
        this->item(row, 2)->setText(_labels[row]);
        this->item(row, 3)->setText(dis);
        return QString("[") + QString::number(row) + QString("]") + _labels[row] + ": " + dis;
    }
}

bool CodeView::isBreakpoint(int row) {
    return this->item(row, 0)->checkState() == Qt::Checked;
}

void CodeView::updateOptimize(QVector<int> indices) {
    if (this->_machine) {
        for (int t : indices) {
            this->rowToString(t);
            //this->item(t, 3)->setText(rowToString(t));
        }
        this->setCurrentCell(this->_machine->pc, 0);
    }
}

void CodeView::updateCode(int row) {
    if (!_machine) return;
    if (row < this->rowCount())
        this->rowToString(row);
        //this->item(row, 3)->setText(rowToString(row));
}

void CodeView::setMachine(Machine &machine) {
    this->_machine = &machine;
}

void CodeView::refresh() {
    for(int i = 0; i < this->rowCount(); ++i){
        auto item = this->item(i, 0);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignVCenter);
    }
    this->resizeColumnToContents(0);
}
