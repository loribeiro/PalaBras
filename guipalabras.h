#ifndef GUIPALABRAS_H
#define GUIPALABRAS_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QString>
#include "dialogjanela.h"
#include "dialogjanela.h"
#include "ui_dialogjanela.h"

namespace Ui {
    class guiPalabras;
}

class guiPalabras : public QMainWindow {
    Q_OBJECT
public:
    guiPalabras(QWidget *parent = 0);
    ~guiPalabras();

protected:
    void changeEvent(QEvent *e);


private:
    Ui::guiPalabras *ui;
    QStringList listaArquivosEntrada;
    DialogJanela opEscolha;

private slots:


private slots:
    void on_actionTamSentencas_triggered();
    void on_actionZipfPromedio_triggered();
    void on_actionFuncionais_triggered();
    void on_actionMinusculas_triggered();
    void on_actionCalculos_triggered();
    void on_actionVisualizacao_triggered();
    void on_actionSentencas_2_triggered();
    void on_actionPalavras_2_triggered();
    void on_actionPalavras_triggered();
    void on_actionPre_tratamento_triggered();
    void on_actionAbrir_triggered();
    void on_actionTags_triggered();
    void on_actionCrescPalabras_triggered();
    void on_actionCresSentencas_triggered();
    void on_actionDeclara_es_utterances_triggered();
    void on_actionSubs_Simples_triggered();
    void on_actionOp_es_de_rede_triggered();
    void on_actionFrases_triggered();
    void on_actionAjuda_triggered();
};

#endif // GUIPALABRAS_H
