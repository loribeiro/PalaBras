#ifndef DIALOGCRECE_H
#define DIALOGCRECE_H

#include <QDialog>

namespace Ui {
    class DialogCrece;
}

class DialogCrece : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCrece(QWidget *parent = 0);
    QString getClasses();
    int getPasso();
    ~DialogCrece();

private:
    Ui::DialogCrece *ui;
};

#endif // DIALOGCRECE_H
