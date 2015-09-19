#include "add_user.h"
#include "ui_add_user.h"

add_user::add_user(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_user)
{
    ui->setupUi(this);
    ui->tb_ok->setDefaultAction(ui->a_add_user);
    connect(ui->a_add_user, SIGNAL(triggered()), this, SLOT(ok()));



}

add_user::~add_user()
{
    delete ui;
}


void add_user::ok(){
    fam1 = ui->le_fam1->text().trimmed();
    fam2 = ui->le_fam2->text().trimmed();
    fam3 = ui->le_fam3->text().trimmed();
    accept();
}
