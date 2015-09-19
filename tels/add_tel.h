#ifndef ADD_TEL_H
#define ADD_TEL_H

#include <QDialog>

namespace Ui {
class add_tel;
}

class add_tel : public QDialog
{
    Q_OBJECT

public:
    explicit add_tel(QWidget *parent = 0);
    ~add_tel();
    QString tel;
    int tp;
    void add_type(int, QString);

public slots:
    void ok();

private:
    Ui::add_tel *ui;
};

#endif // ADD_TEL_H
