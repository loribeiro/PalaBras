#include "dialogcrece.h"
#include "ui_dialogcrece.h"

DialogCrece::DialogCrece(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCrece)
{
    ui->setupUi(this);
}

DialogCrece::~DialogCrece()
{
    delete ui;
}

QString DialogCrece::getClasses()
{
    return ui->ClassesLineEdit->text();
}
int DialogCrece::getPasso()
{
    return this->ui->PassoSpinBox->value();
}
