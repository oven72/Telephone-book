#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlerror.h>
#include <QFile>
#include <QMessageBox>
#include <QMenu>
#include <QListWidget>
#include <QPoint>
#include <QKeyEvent>
#include <QTreeWidgetItem>

#include <history.h>
#include <add_user.h>
#include <add_tel.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
   //void keyPressEvent(QKeyEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void fnd(void);
    void del(void);
    void del_rel(void);
    void add_top_user(void);
    void add_top_tel(void);
    void add_rel(void);
    void edited(void);
    void history_view(QString);
    void history_to_find(QListWidgetItem*);
    void itm_change(QTreeWidgetItem*, QTreeWidgetItem*);
    void about(void);

private:
    Ui::MainWindow *ui;
    history *lw;
    int fnd_on_fio(void);
    int fnd_on_num(void);
    void add_history(QString);
    QString css_str;
    QMenu edit_menu;

};

#endif // MAINWINDOW_H
