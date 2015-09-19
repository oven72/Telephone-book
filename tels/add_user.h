#ifndef ADD_USER_H
#define ADD_USER_H

#include <QDialog>

namespace Ui {
class add_user;
}

class add_user : public QDialog
{
    Q_OBJECT

public:
    explicit add_user(QWidget *parent = 0);
    ~add_user();
    QString fam1;
    QString fam2;
    QString fam3;

public slots:
    void ok();

private:
    Ui::add_user *ui;
};

#endif // ADD_USER_H
