#include "history.h"
#include "ui_history.h"

history::history(QWidget *parent) :
    QListWidget(parent),
    ui(new Ui::history)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
    hide();
}


history::~history()
{
    delete ui;
}

void history::keyPressEvent(QKeyEvent *event){
    if (event->key()==Qt::Key_Escape){
        hide();
    }
    QListWidget::keyPressEvent(event);
}

void history::focusOutEvent(QFocusEvent*){
    hide();
}
