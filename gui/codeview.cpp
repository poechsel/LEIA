#include "codeview.h"

CodeView::CodeView(QWidget *parent):
    QListWidget(parent)
{
    this->setUniformItemSizes(true);
}


void CodeView::update(Machine &machine) {
    QStringList code;
    for (int i = 0; i < 0x10000; ++i) {
        code.append(QString::fromStdString(dissassemble(machine.memory[i])));
    }
    this->clear();
    this->addItems(code);
    this->refresh();
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
