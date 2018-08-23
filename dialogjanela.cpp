#include "dialogjanela.h"
#include "ui_dialogjanela.h"

DialogJanela::DialogJanela(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogJanela)
{
    ui->setupUi(this);
}

DialogJanela::~DialogJanela()
{
    delete ui;
}

void DialogJanela::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

char DialogJanela::getIdioma()
{
    if (this->ui->ingles->isChecked())
        return 1;

    if (this->ui->portugues->isChecked())
        return 2;

    return 0;
}

char DialogJanela::getAnalise()
{
    if (this->ui->normal->isChecked())
        return 1;

    if (this->ui->canonico->isChecked())
        return 2;

    if (this->ui->completo->isChecked())
        return 3;

    return 0;
}



void DialogJanela::on_cancelar_clicked()
{
    this->done(0);
}

void DialogJanela::on_inicio_clicked()
{
    opIdioma = getIdioma();
    opAnalise = getAnalise();
    if (opIdioma == 0 || opAnalise == 0)
        this->done(0);
    else
        this->done(1);
}
