#include "config.h"
#include "ui_config.h"
#include <fstream>
#include <iostream>
using namespace std;

config::config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::config)
{
    ui->setupUi(this);

    string maxLinha;

    fstream arqGr,arqPl;
    arqGr.open("gramat.ftl",ios_base::in);
    if(!arqGr.is_open())
        arqGr.open("gramat.ftl",ios_base::out);
    arqPl.open("palav.ftl",ios_base::in);
    if(!arqPl.is_open())
        arqPl.open("palav.ftl",ios_base::out);
    while(!arqGr.eof())
    {
        getline(arqGr,maxLinha);
        this->ui->gramat->append(QString(maxLinha.c_str()));
    }

    while(!arqPl.eof())
    {
        getline(arqPl,maxLinha);
        this->ui->palavr->append(QString(maxLinha.c_str()));
    }
    arqGr.close();
    arqPl.close();
}

config::~config()
{
    ofstream arqGr,arqPl;
    arqGr.open("gramat.ftl");
    arqGr << this->ui->gramat->toPlainText().toStdString();
    arqGr.close();
    arqPl.open("palav.ftl");
    arqPl << this->ui->palavr->toPlainText().toStdString();
    arqPl.close();

    delete ui;
}
