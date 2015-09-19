#include "add_tel.h"
#include "ui_add_tel.h"

add_tel::add_tel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_tel)
{
    ui->setupUi(this);
    ui->tb_ok->setDefaultAction(ui->a_add_tel);
    connect(ui->a_add_tel, SIGNAL(triggered()), this, SLOT(ok()));

}

add_tel::~add_tel()
{
    delete ui;
}

void add_tel::ok(){
    tel = ui->le_tel->text().trimmed();
    tp = ui->cb_type->currentData(Qt::UserRole).toInt();
    accept();
}

void add_tel::add_type(int tp, QString str){
  ui->cb_type->addItem(str, tp);
}
