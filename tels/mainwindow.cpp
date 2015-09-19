#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"


extern QSqlDatabase *db;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lw = new history(this);
    connect(lw, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(history_to_find(QListWidgetItem*)));


    QFile file(QApplication::applicationDirPath()+"/form.css");
    if (file.open(QIODevice::ReadOnly)) {
        css_str = file.readAll();
        file.close();
    }

    setStyleSheet(css_str);

    edit_menu.addAction(ui->a_add_user);
    edit_menu.addAction(ui->a_add_telephone);
    edit_menu.addAction(ui->a_add_and_relation);
    edit_menu.addSeparator();
    edit_menu.addAction(ui->a_del);
    edit_menu.addAction(ui->a_delete_relation);

    ui->tw_result->addActions(edit_menu.actions());
    ui->tw_result->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->tb_find->setDefaultAction(ui->a_find);
    ui->tb_del->setDefaultAction(ui->a_del);
    ui->tb_add_top_u->setDefaultAction(ui->a_add_user);
    ui->tb_add_top_t->setDefaultAction(ui->a_add_telephone);
    ui->tb_del_rel->setDefaultAction(ui->a_delete_relation);
    ui->tb_add_rel->setDefaultAction(ui->a_add_and_relation);

    connect(ui->tw_result, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itm_change(QTreeWidgetItem*, QTreeWidgetItem*)));
    connect(ui->le_search_str, SIGNAL(returnPressed()), this, SLOT(fnd()));
    connect(ui->a_find, SIGNAL(triggered()), this, SLOT(fnd()));
    connect(ui->a_del, SIGNAL(triggered()), this, SLOT(del()));
    connect(ui->a_add_user, SIGNAL(triggered()), this, SLOT(add_top_user()));
    connect(ui->a_add_telephone, SIGNAL(triggered()), this, SLOT(add_top_tel()));
    connect(ui->a_delete_relation, SIGNAL(triggered()), this, SLOT(del_rel()));
    connect(ui->a_add_and_relation, SIGNAL(triggered()), this, SLOT(add_rel()));
    connect(ui->history, SIGNAL(linkActivated(QString)), this, SLOT(history_view(QString)));
    connect(ui->a_edit_information, SIGNAL(triggered()), this, SLOT(edited()));
    connect(ui->a_about, SIGNAL(triggered()), this, SLOT(about()));

}

MainWindow::~MainWindow()
{
    delete lw;
    delete ui;
}

int MainWindow::fnd_on_fio(void){
    //сначала осуществляем поиск по фамилии
    QSqlQuery sq_get_fio(*db);
    if (!sq_get_fio.prepare("select * from find_user(:I_SEARCH_STR)")) {
        qDebug() << sq_get_fio.lastError().text();
        QMessageBox::information(this, tr("Error database"), sq_get_fio.lastError().text());
        return 0;
    }
    sq_get_fio.bindValue(":I_SEARCH_STR", ui->le_search_str->text().trimmed());
    if (!sq_get_fio.exec())  {
        QMessageBox::information(this, tr("Error database"), sq_get_fio.lastError().text());
        qDebug() << sq_get_fio.lastError().text();
        return 0;
    }
    ui->tw_result->clear();
    bool is_data = false;

    //заполняем дерево по фамилиям
    QTreeWidgetItem *item_fio;
    int cnt = 0;
    while (sq_get_fio.next()) {
        cnt++;
        is_data = true;

        item_fio = new QTreeWidgetItem(ui->tw_result);
        item_fio->setData(0, Qt::UserRole, "0"+sq_get_fio.record().value("O_ID").toString().rightJustified(20, '0'));
        item_fio->setText(0, sq_get_fio.record().value("O_FAM1").toString()+
                          " "+sq_get_fio.record().value("O_FAM2").toString()+
                          " "+sq_get_fio.record().value("O_FAM3").toString());

        QSqlQuery sq_get_num(*db);
        if (!sq_get_num.prepare("select * from INFO_BY_USER_ID(:I_ID)")) {
            qDebug() << sq_get_num.lastError().text();
            QMessageBox::information(this, tr("Error database"), sq_get_num.lastError().text());
            return 0;
        }
        sq_get_num.bindValue(":I_ID", sq_get_fio.record().value("O_ID").toInt());
        if (!sq_get_num.exec())  {
            QMessageBox::information(this, tr("Error database"), sq_get_num.lastError().text());
            qDebug() << sq_get_num.lastError().text();
            return 0;
        }
        QTreeWidgetItem *item_num;
        while (sq_get_num.next()) { //добавляем номера
            if (!sq_get_num.record().value("O_NUM").isNull()){
                item_num = new QTreeWidgetItem(item_fio);
                item_num->setData(0, Qt::UserRole, "1"+sq_get_num.record().value("O_TID").toString().rightJustified(20, '0')+
                                  sq_get_fio.record().value("O_ID").toString().rightJustified(20, '0'));
                item_num->setText(0, sq_get_num.record().value("O_NUM").toString()+
                                  " ("+sq_get_num.record().value("O_MEMO").toString()+")");

            }
        }

    }
    add_history(ui->le_search_str->text().trimmed());
    if (cnt>50) {
        //Много данных. Конкретизируйте запрос
        QMessageBox::information(this, tr("Search"), tr("It is a lot of data. Concretize inquiry"));
    }
    if (!is_data) {
        //если поиск по фамилиям не удался
        return 1;
    } else {
        //описк удался
        return 2;
    }
    return 0;
}


int MainWindow::fnd_on_num(void){
    //сначала осуществляем поиск по фамилии
    QSqlQuery sq_get_num(*db);
    if (!sq_get_num.prepare("select * from find_tels(:I_SEARCH_STR)")) {
        qDebug() << sq_get_num.lastError().text();
        QMessageBox::information(this, tr("Error database"), sq_get_num.lastError().text());
        return 0;
    }
    sq_get_num.bindValue(":I_SEARCH_STR", ui->le_search_str->text().trimmed());
    if (!sq_get_num.exec())  {
        QMessageBox::information(this, tr("Error database"), sq_get_num.lastError().text());
        qDebug() << sq_get_num.lastError().text();
        return 0;
    }
    ui->tw_result->clear();
    bool is_data = false;

    //заполняем дерево по номерам
    QTreeWidgetItem *item_num;
    int cnt = 0;
    while (sq_get_num.next()) {
        cnt++;
        is_data = true;

        item_num = new QTreeWidgetItem(ui->tw_result);
        item_num->setData(0, Qt::UserRole, "2"+sq_get_num.record().value("O_ID").toString().rightJustified(20, '0'));
        item_num->setText(0, sq_get_num.record().value("O_NUM").toString()+
                          " ("+sq_get_num.record().value("O_MEMO").toString()+") ");

        QSqlQuery sq_get_fio(*db);
        if (!sq_get_fio.prepare("select * from INFO_BY_TEL_ID(:I_ID)")) {
            qDebug() << sq_get_fio.lastError().text();
            QMessageBox::information(this, tr("Error database"), sq_get_fio.lastError().text());
            return 0;
        }
        sq_get_fio.bindValue(":I_ID", sq_get_num.record().value("O_ID").toInt());
        if (!sq_get_fio.exec())  {
            QMessageBox::information(this, tr("Error database"), sq_get_fio.lastError().text());
            qDebug() << sq_get_fio.lastError().text();
            return 0;
        }
        QTreeWidgetItem *item_fio;
        while (sq_get_fio.next()) {//добавляем фамилии
            if (!sq_get_fio.record().value("O_FAM1").isNull()||
                !sq_get_fio.record().value("O_FAM2").isNull()||
                !sq_get_fio.record().value("O_FAM3").isNull()){
                item_fio = new QTreeWidgetItem(item_num);
                item_fio->setData(0, Qt::UserRole, "3"+sq_get_fio.record().value("O_UID").toString().rightJustified(20, '0')+
                                  sq_get_num.record().value("O_ID").toString().rightJustified(20, '0'));
                item_fio->setText(0, sq_get_fio.record().value("O_FAM1").toString()+
                                  " "+sq_get_fio.record().value("O_FAM2").toString()+
                                  " "+sq_get_fio.record().value("O_FAM3").toString());
            }
        }
    }
    add_history(ui->le_search_str->text().trimmed());
    if (cnt>50) {
        //Много данных. Конкретизируйте запрос
        QMessageBox::information(this, tr("Search"), tr("It is a lot of data. Concretize inquiry"));
    }
    if (!is_data) {
        //если поиск по телефонам не удался
        return 1;
    } else {
        //описк удался
        return 2;
    }
    return 0;
}

void MainWindow::fnd(void){
    //все зависит от приоритета поиска х=
    //0-фио родитель
    //1-фио наследник
    //2-номер родитель
    //3-номер наследник
    //хyyyyyyyyyyyyyyyyyyyyzzzzzzzzzzzzzzzzzzzz
    //    текущий элемент   предок(если есть)
    if (ui->a_priority_number->isChecked()){
        if (fnd_on_num()==1){
            fnd_on_fio();
        }
    } else {
        if (fnd_on_fio()==1){
            fnd_on_num();
        }
    }
}

void MainWindow::add_history(QString str){
    QSqlQuery sq_h(*db);
    if (!sq_h.prepare("execute procedure add_history(:I_STR)")) {
        qDebug() << sq_h.lastError().text();
        QMessageBox::information(this, tr("Error database"), sq_h.lastError().text());
        return;
    }
    sq_h.bindValue(":I_STR", str);
    if (!sq_h.exec())  {
        QMessageBox::information(this, tr("Error database"), sq_h.lastError().text());
        qDebug() << sq_h.lastError().text();
        return;
    }
}

void MainWindow::history_view(QString){
    qDebug() << "history";
    QSqlQuery sq_h(*db);
    if (!sq_h.prepare("select * from get_history")) {
        qDebug() << sq_h.lastError().text();
        QMessageBox::information(this, tr("Error database"), sq_h.lastError().text());
        return;
    }
    if (!sq_h.exec())  {
        QMessageBox::information(this, tr("Error database"), sq_h.lastError().text());
        qDebug() << sq_h.lastError().text();
        return;
    }

    lw->setStyleSheet(css_str);
    lw->clear();
    while (sq_h.next()){
        new QListWidgetItem(sq_h.record().value("O_FND_STR").toString(), lw);
    }
    lw->move(cursor().pos().x(), cursor().pos().y());
    lw->show();
    lw->setFocus();

}

void MainWindow::history_to_find(QListWidgetItem *itm){
    ui->le_search_str->setText(itm->text());
    lw->hide();
}

//void MainWindow::keyPressEvent(QKeyEvent *event){
//    if (!lw->isHidden()&&event->key()==Qt::Key_Escape) lw->hide();
//}


void MainWindow::edited(void){
    if (ui->a_edit_information->isChecked()){
        ui->a_add_telephone->setEnabled(true);
        ui->a_add_user->setEnabled(true);
        ui->a_del->setEnabled(true);
        ui->a_delete_relation->setEnabled(true);
        ui->a_add_and_relation->setEnabled(true);
    } else {
        ui->a_add_telephone->setEnabled(false);
        ui->a_add_user->setEnabled(false);
        ui->a_del->setEnabled(false);
        ui->a_delete_relation->setEnabled(false);
        ui->a_add_and_relation->setEnabled(false);
    }
    qDebug() << "edited";
}


void MainWindow::del(void){
    qDebug() << "del";
    if (ui->tw_result->currentItem()!=NULL) {
        QString dat = ui->tw_result->currentItem()->data(0, Qt::UserRole).toString();
        int tp = dat.mid(0,1).toInt();
        int curid = dat.mid(1,20).toInt();
        int parid = dat.mid(21,20).toInt();
        qDebug() << tp <<curid <<parid;
        bool isdel = false;
        switch (tp) {
            case 0:
            case 3:
                //это фамилия
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete user<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим фамилию
                    QSqlQuery sq_du(*db);
                    if (!sq_du.prepare("execute procedure del_user(:I_ID)")) {
                        qDebug() << sq_du.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_du.lastError().text());
                        return;
                    }
                    sq_du.bindValue(":I_ID", curid);
                    if (!sq_du.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_du.lastError().text());
                        qDebug() << sq_du.lastError().text();
                        return;
                    }
                    isdel = true;
                }

                break;
            case 1:
            case 2:
                //это номер
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete number<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим номер
                    QSqlQuery sq_dn(*db);
                    if (!sq_dn.prepare("execute procedure del_tel(:I_ID)")) {
                        qDebug() << sq_dn.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_dn.lastError().text());
                        return;
                    }
                    sq_dn.bindValue(":I_ID", curid);
                    if (!sq_dn.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_dn.lastError().text());
                        qDebug() << sq_dn.lastError().text();
                        return;
                    }
                    isdel = true;
                }
                break;
            default:
                break;
        }
        if (isdel) fnd();
    } else {
        QMessageBox::information(this, "delete", tr("need select user or number"));
    }
}

void MainWindow::add_top_user(void){
    qDebug() << "add_top_user";
    add_user *au = new add_user(this);
    if (au->exec()){
        qDebug() << "add user";
        QSqlQuery *sq_au = new QSqlQuery(*db);
        if (!sq_au->prepare("execute procedure add_user(:fam1, :fam2, :fam3)")) {
            qDebug() << sq_au->lastError().text();
            QMessageBox::information(this, tr("Error database"), sq_au->lastError().text());
            delete sq_au;
            delete au;
            return;
        }
        sq_au->bindValue(":fam1", au->fam1);
        sq_au->bindValue(":fam2", au->fam2);
        sq_au->bindValue(":fam3", au->fam3);
        if (!sq_au->exec())  {
            QMessageBox::information(this, tr("Error database"), sq_au->lastError().text());
            qDebug() << sq_au->lastError().text();
            delete sq_au;
            delete au;
            return;
        }
        sq_au->seek(0);
        QMessageBox::information(this, tr("User added"), tr("Add user ID=")+sq_au->record().value("O_ID").toString());

        delete sq_au;
    }
    delete au;
}

void MainWindow::add_top_tel(void){
    qDebug() << "add_top_tel";
    add_tel *at = new add_tel(this);
    //заполним типы телефонов
    QSqlQuery sq_get(*db);
    if (!sq_get.prepare("select * from get_tel_types")) {
        qDebug() << sq_get.lastError().text();
        QMessageBox::information(this, tr("Error database"), sq_get.lastError().text());
        delete at;
        return;
    }
    if (!sq_get.exec())  {
        QMessageBox::information(this, tr("Error database"), sq_get.lastError().text());
        qDebug() << sq_get.lastError().text();
        delete at;
        return;
    }
    while (sq_get.next()) {
        at->add_type(sq_get.record().value("O_ID").toInt(), sq_get.record().value("O_MEMO").toString());
    }

    if (at->exec()){
        qDebug() << "add tel" << at->tel;
        QSqlQuery *sq_at = new QSqlQuery(*db);
        if (!sq_at->prepare("execute procedure add_tel(:str, :tp)")) {
            qDebug() << sq_at->lastError().text();
            QMessageBox::information(this, tr("Error database"), sq_at->lastError().text());
            delete sq_at;
            delete at;
            return;
        }
        sq_at->bindValue(":str", at->tel);
        sq_at->bindValue(":tp", at->tp);
        if (!sq_at->exec())  {
            QMessageBox::information(this, tr("Error database"), sq_at->lastError().text());
            qDebug() << sq_at->lastError().text();
            delete sq_at;
            delete at;
            return;
        }
        sq_at->seek(0);
        QMessageBox::information(this, tr("Telephone added"), tr("Add telephone ID=")+sq_at->record().value("O_ID").toString());

        delete sq_at;
    }
    qDebug() << "add tel finish";
    delete at;
}

void MainWindow::add_rel(void){
    qDebug() << "add_rel";
    if (ui->tw_result->currentItem()!=NULL) {
        QString dat = ui->tw_result->currentItem()->data(0, Qt::UserRole).toString();
        int tp = dat.mid(0,1).toInt();
        int curid = dat.mid(1,20).toInt();
        int parid = dat.mid(21,20).toInt();
        qDebug() << tp <<curid <<parid;
        bool isset = false;
        switch (tp) {
            case 0:
            case 1:{
                //можно добавить только номер привязав сразу к текущей фамилии
                qDebug() << "add user start";
                add_tel *at = new add_tel(this);
                //заполним типы телефонов
                QSqlQuery sq_get(*db);
                if (!sq_get.prepare("select * from get_tel_types")) {
                    qDebug() << sq_get.lastError().text();
                    QMessageBox::information(this, tr("Error database"), sq_get.lastError().text());
                    delete at;
                    return;
                }
                if (!sq_get.exec())  {
                    QMessageBox::information(this, tr("Error database"), sq_get.lastError().text());
                    qDebug() << sq_get.lastError().text();
                    delete at;
                    return;
                }
                while (sq_get.next()) {
                    at->add_type(sq_get.record().value("O_ID").toInt(), sq_get.record().value("O_MEMO").toString());
                }


                if (at->exec()){
                    qDebug() << "add tel";
                    QSqlQuery *sq_at = new QSqlQuery(*db);
                    if (!sq_at->prepare("execute procedure add_tel(:str, :tp)")) {
                        qDebug() << sq_at->lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_at->lastError().text());
                        delete at;
                        return;
                    }
                    sq_at->bindValue(":str", at->tel);
                    sq_at->bindValue(":tp", at->tp);
                    if (!sq_at->exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_at->lastError().text());
                        qDebug() << sq_at->lastError().text();
                        delete at;
                        return;
                    }
                    sq_at->seek(0);
                    //получили код вновь добавленного телефона (или код старого, если он уже был)
                    int u_id = sq_at->record().value("O_ID").toInt();
                    qDebug() << "uid="<<u_id;
                    delete sq_at;

                    QSqlQuery *sq_rel = new QSqlQuery(*db);
                    if (!sq_rel->prepare("execute procedure add_rel(:i_user_id, :i_tel_id)")) {
                        qDebug() << sq_rel->lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_rel->lastError().text());
                        delete at;
                        delete sq_rel;
                        return;
                    }
                    sq_rel->bindValue(":i_tel_id", u_id);
                    switch (tp){
                        case 0:
                            sq_rel->bindValue(":i_user_id", curid);
                            break;
                        case 1:
                            sq_rel->bindValue(":i_user_id", parid);
                            break;
                    }
                    if (!sq_rel->exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_rel->lastError().text());
                        qDebug() << sq_rel->lastError().text();
                        delete at;
                        delete sq_rel;
                        return;
                    }
                    delete sq_rel;
                    isset = true;
                }
                delete at;
                break;
            }
            case 2:
            case 3:{
                //можно добавить только фио привязав сразу к текущему номеру телефона
                qDebug() << "add user start";
                add_user *au = new add_user(this);
                if (au->exec()){
                    qDebug() << "add user";
                    QSqlQuery *sq_au = new QSqlQuery(*db);
                    if (!sq_au->prepare("execute procedure add_user(:fam1, :fam2, :fam3)")) {
                        qDebug() << sq_au->lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_au->lastError().text());
                        delete au;
                        return;
                    }
                    sq_au->bindValue(":fam1", au->fam1);
                    sq_au->bindValue(":fam2", au->fam2);
                    sq_au->bindValue(":fam3", au->fam3);
                    if (!sq_au->exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_au->lastError().text());
                        qDebug() << sq_au->lastError().text();
                        delete au;
                        return;
                    }
                    sq_au->seek(0);
                    //получили код вновь добавленного человека (или код старого, если он уже был)
                    int u_id = sq_au->record().value("O_ID").toInt();
                    qDebug() << "uid="<<u_id;
                    delete sq_au;

                    QSqlQuery *sq_rel = new QSqlQuery(*db);
                    if (!sq_rel->prepare("execute procedure add_rel(:i_user_id, :i_tel_id)")) {
                        qDebug() << sq_rel->lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_rel->lastError().text());
                        delete au;
                        delete sq_rel;
                        return;
                    }
                    sq_rel->bindValue(":i_user_id", u_id);
                    switch (tp){
                        case 2:
                            sq_rel->bindValue(":i_tel_id", curid);
                            break;
                        case 3:
                            sq_rel->bindValue(":i_tel_id", parid);
                            break;
                    }
                    if (!sq_rel->exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_rel->lastError().text());
                        qDebug() << sq_rel->lastError().text();
                        delete au;
                        delete sq_rel;
                        return;
                    }
                    delete sq_rel;
                    isset = true;
                }
                delete au;
                break;
            }
        }
        if (isset) fnd();
    }
}

void MainWindow::del_rel(void){
    qDebug() << "del_rel";
    if (ui->tw_result->currentItem()!=NULL) {
        QString dat = ui->tw_result->currentItem()->data(0, Qt::UserRole).toString();
        int tp = dat.mid(0,1).toInt();
        int curid = dat.mid(1,20).toInt();
        int parid = dat.mid(21,20).toInt();
        qDebug() << tp <<curid <<parid;
        bool isdel = false;
        switch (tp) {
            case 0:
                //это фамилия (все связи рвем)
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete all user relations for<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим все связи фамилии
                    QSqlQuery sq_dur(*db);
                    if (!sq_dur.prepare("execute procedure del_user_rel(:I_ID)")) {
                        qDebug() << sq_dur.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_dur.lastError().text());
                        return;
                    }
                    sq_dur.bindValue(":I_ID", curid);
                    if (!sq_dur.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_dur.lastError().text());
                        qDebug() << sq_dur.lastError().text();
                        return;
                    }
                    isdel = true;
                }
                break;
            case 3:
                //это фамилия (рвем только связь с предком)
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete tel user relation<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим связи фамилии с телефоном предком
                    QSqlQuery sq_dtur(*db);
                    if (!sq_dtur.prepare("execute procedure del_tel_user_rel(:I_TEL_ID, :I_USER_ID)")) {
                        qDebug() << sq_dtur.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_dtur.lastError().text());
                        return;
                    }
                    sq_dtur.bindValue(":I_TEL_ID", parid);
                    sq_dtur.bindValue(":I_USER_ID", curid);
                    if (!sq_dtur.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_dtur.lastError().text());
                        qDebug() << sq_dtur.lastError().text();
                        return;
                    }
                    isdel = true;
                }

                break;
            case 1:
                //это номер (рвем только связь с предком)
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete tel user relation<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим связи телефона с фамилией предком
                    QSqlQuery sq_dtur(*db);
                    if (!sq_dtur.prepare("execute procedure del_tel_user_rel(:I_TEL_ID, :I_USER_ID)")) {
                        qDebug() << sq_dtur.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_dtur.lastError().text());
                        return;
                    }
                    sq_dtur.bindValue(":I_TEL_ID", curid);
                    sq_dtur.bindValue(":I_USER_ID", parid);
                    if (!sq_dtur.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_dtur.lastError().text());
                        qDebug() << sq_dtur.lastError().text();
                        return;
                    }
                    isdel = true;
                }
                break;
            case 2:
                //это номер (все связи рвем)
                if (QMessageBox::Yes==QMessageBox::question(this, trUtf8("ahtung!"), tr("delete all number relations for<br> <b>\"")+
                                                            ui->tw_result->currentItem()->text(0)+
                                                            " </b> \"?",
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No)) {
                    //удалим все связи телефона
                    QSqlQuery sq_dtr(*db);
                    if (!sq_dtr.prepare("execute procedure del_tel_rel(:I_ID)")) {
                        qDebug() << sq_dtr.lastError().text();
                        QMessageBox::information(this, tr("Error database"), sq_dtr.lastError().text());
                        return;
                    }
                    sq_dtr.bindValue(":I_ID", curid);
                    if (!sq_dtr.exec())  {
                        QMessageBox::information(this, tr("Error database"), sq_dtr.lastError().text());
                        qDebug() << sq_dtr.lastError().text();
                        return;
                    }
                    isdel = true;
                }
                break;
            default:
                break;
        }
        if (isdel) fnd();

    }
}

void MainWindow::itm_change(QTreeWidgetItem* w1, QTreeWidgetItem*){
    //на самом деле не понадобилось (оставлю для истории)
    if (ui->a_edit_information->isChecked()){
        if (w1!=NULL){
            QString dat = w1->data(0, Qt::UserRole).toString();
            int tp = dat.mid(0,1).toInt();
            switch (tp) {
                case 0:
                    ui->a_add_telephone->setEnabled(true);
                    ui->a_add_user->setEnabled(true);
                    ui->a_del->setEnabled(true);
                    ui->a_delete_relation->setEnabled(true);
                    ui->a_add_and_relation->setEnabled(true);
                    break;
                case 1:
                    ui->a_add_telephone->setEnabled(true);
                    ui->a_add_user->setEnabled(true);
                    ui->a_del->setEnabled(true);
                    ui->a_delete_relation->setEnabled(true);
                    ui->a_add_and_relation->setEnabled(true);
                    break;
                case 2:
                    ui->a_add_telephone->setEnabled(true);
                    ui->a_add_user->setEnabled(true);
                    ui->a_del->setEnabled(true);
                    ui->a_delete_relation->setEnabled(true);
                    ui->a_add_and_relation->setEnabled(true);
                    break;
                case 3:
                    ui->a_add_telephone->setEnabled(true);
                    ui->a_add_user->setEnabled(true);
                    ui->a_del->setEnabled(true);
                    ui->a_delete_relation->setEnabled(true);
                    ui->a_add_and_relation->setEnabled(true);
                    break;
                default:
                    break;
            }
        } else {
            ui->a_add_telephone->setEnabled(true);
            ui->a_add_user->setEnabled(true);
            ui->a_del->setEnabled(false);
            ui->a_delete_relation->setEnabled(false);
            ui->a_add_and_relation->setEnabled(false);
        }
    }
}
void MainWindow::about(void){
    QMessageBox::information(this, tr("about"), tr("Is programm is telepfone catalog.<p align=\"right\">2015(c)</p>"));
}
