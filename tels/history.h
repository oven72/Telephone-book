#ifndef HISTORY_H
#define HISTORY_H

#include <QListWidget>
#include <QKeyEvent>

namespace Ui {
class history;
}

class history : public QListWidget
{
    Q_OBJECT

protected:
   void keyPressEvent(QKeyEvent *event);
   void focusOutEvent(QFocusEvent *event);

public:
    explicit history(QWidget *parent = 0);
    ~history();

private:
    Ui::history *ui;
};

#endif // HISTORY_H
