#ifndef DIALOGJANELA_H
#define DIALOGJANELA_H

#include <QDialog>

namespace Ui {
    class DialogJanela;
}

class DialogJanela : public QDialog {
    Q_OBJECT
public:
    DialogJanela(QWidget *parent = 0);
    ~DialogJanela();
    char getIdioma();
    char getAnalise();
    char opIdioma, opAnalise;


protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogJanela *ui;

private slots:
    void on_cancelar_clicked();
    void on_inicio_clicked();
};

#endif // DIALOGJANELA_H
