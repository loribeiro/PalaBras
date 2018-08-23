#include "guipalabras.h"
#include "ui_guipalabras.h"
#include <QString>
#include "tratatexto.h"
#include <QFileInfo>
#include <QProgressDialog>
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include "dialogcrece.h"
#include "config.h"
#include <string>
using namespace std;

guiPalabras::guiPalabras(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::guiPalabras)
{
    ui->setupUi(this);
    this->ui->menuAnalises->setEnabled(true);
    this->ui->labelStatus->setText("Nenhum arquivo aberto");
}

guiPalabras::~guiPalabras()
{
    delete ui;
}

void guiPalabras::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void guiPalabras::on_actionAbrir_triggered()
{

    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) para abrir"),
                QString::null,
                QString::null);
    if(this->listaArquivosEntrada.size()>0)
    {
        this->ui->labelStatus->clear();
        this->ui->menuAnalises->setEnabled(true);
    }
    else
    {
        this->ui->menuAnalises->setEnabled(false);
    }
}

void guiPalabras::on_actionPre_tratamento_triggered()
{
    /* Fiz diversas modificações nesta funçao com a inteção de resolver o problema da codificação de letras latinas
        as modificações estão marcadas com comentarios.
*/

   //Quando a caixa de dialogo abre ele marca 0 caso não seja selecionado nada
    int meleca = opEscolha.exec();
    if (meleca == 0)
        return;
    //Recebem as opçes de escolha de idioma e o tipo de analise
    char idioma = opEscolha.opIdioma;
    char tipoanalise = opEscolha.opAnalise;

    //
    trataTexto arqIndividual;
    QString path;
    QProgressDialog progress("Tratando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    foreach(path, this->listaArquivosEntrada)
    {
        progress.setValue(i++);
        QFileInfo arqInfo(path);
        QString nomePrincipal = arqInfo.path()+"/"+arqInfo.baseName();
        string localArquivo=arqInfo.path().toStdString();
        string nomeArquivo=arqInfo.baseName().toStdString();

        if(idioma==1){
            system(("cd "+localArquivo +"&& cp "+nomeArquivo+".txt "+nomeArquivo+".txtConvertido.txt").c_str());
            arqIndividual.lowerWord(path+"Convertido.txt", nomePrincipal+"Convertido");
            arqIndividual.preanalyze(nomePrincipal+"Convertido", idioma, tipoanalise);
        }else{

            string tipoDoArquivo= arqIndividual.GetStdoutFromCommand(("cd "+arqInfo.path().toStdString()+" && file -i "+arqInfo.baseName().toStdString()+".txt").c_str());// esta linha recupera o tipo de arquivo de texto está sendo tratado
            bool v=false;
            QString auxiliar;
            string s;
            //Este for está buscando apenas a codificação do arquivo, que vem precedida de algumas outras informações inuteis para o nosso interesse
            for(int i=0;i<tipoDoArquivo.size();i++){
                cout<<tipoDoArquivo[i];
                if(tipoDoArquivo[i]=='\n'|| tipoDoArquivo[i]=='\0'){
                    cout<<"sai do laço"<<endl;
                    break;
                }
                if(tipoDoArquivo[i]=='='){
                        ++i;
                        v=true;
                    }
                    if(v==true){
                        s=s+tipoDoArquivo[i];
                    }


                }
            cout<<"esse é o tipo do arquivo: "<<tipoDoArquivo<<endl;
            cout<<s<<" string s"<<endl;
            system(("cd "+localArquivo+ " && iconv -f "+s+" -t iso-8859-1//TRANSLIT "+ nomeArquivo +".txt -o "+nomeArquivo +".txtConvertido.txt ").c_str());// Converte o arquivo para o padrao latino

            arqIndividual.lowerWord(path+"Convertido.txt", nomePrincipal+"Convertido");
            arqIndividual.preanalyze(nomePrincipal+"Convertido", idioma, tipoanalise);
        }
        if (progress.wasCanceled())
            break;
    }
}

void guiPalabras::on_actionPalavras_triggered()
{
    trataTexto arqIndividual;
    ofstream arquivoSaida;
    QString path;
    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    arquivoSaida.open("resultados.txt", ios::out);
    arquivoSaida << "Arquivo\tWordTot\tWordDiffN\tWordDiffCan\n";

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqTag = arqInfo.baseName() + ".tag";
        QString arqFrq = arqInfo.baseName() + ".frq";
        if (!arqIndividual.abreArquivo(arqTag.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + arqInfo.baseName());
            msgBox.exec();

        }
        // saving contents:
        arqIndividual.frequenciaPalavra(arqFrq);
        arquivoSaida << arqTag.toStdString().c_str() << "\t"
                     << arqIndividual.palTot << "\t" << arqIndividual.palDiffN << "\t"
                     << arqIndividual.palDiffC << endl;
        
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
    arquivoSaida.close();

}


void guiPalabras::on_actionPalavras_2_triggered()
{
    trataTexto arqIndividual;
    QString path;
    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        if (!arqIndividual.redePalavras((arqInfo.path()+"/"+arqInfo.baseName()+"Convertido").toStdString(), 2))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + arqInfo.baseName());
            msgBox.exec();

        }

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
}

void guiPalabras::on_actionSentencas_2_triggered()
{
    trataTexto arqIndividual;
    QString path;
    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    arqIndividual.carregaFiltros();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);

        if (!arqIndividual.redeSentencas((arqInfo.path()+"/"+arqInfo.baseName()+"Convertido").toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + arqInfo.baseName());
            msgBox.exec();

        }

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
}

void guiPalabras::on_actionVisualizacao_triggered()
{
    system ("wine ~/.wine/drive_c/pajek/Pajek/PAJEK.exe");
}

void guiPalabras::on_actionCalculos_triggered()
{
    QString path;
    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    //int i=0;
    progress.show();
    string comand/*="netall > resultadosNetAll.txt"*/;
    string co2;
    //system(comand.c_str());

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString nomeArqSai = arqInfo.baseName();
        QString nomeArqEnt = arqInfo.baseName();
        QString endereco= arqInfo.path();
        cout<<nomeArqEnt.toStdString()<<endl;
        cout<<endereco.toStdString()<<endl;
        /*comand="netall " + nomeArqSai.toStdString() + " -arq " +
                nomeArqEnt.toStdString() + " -bin >> resultadosNetAll.txt";*/
        comand="python analyzer.py "+endereco.toStdString()+
                " "+nomeArqEnt.toStdString();
        co2="nano config.ui";
        system(comand.c_str());
    }
}

void guiPalabras::on_actionMinusculas_triggered()
{
    trataTexto arqIndividual;
    QString path;
    QProgressDialog progress("Tratando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    foreach(path, this->listaArquivosEntrada)
    {
        progress.setValue(i++);
        QFileInfo arqInfo(path);
        //QString nomePrincipal = arqInfo.baseName();

        arqIndividual.lowerWord(path, arqInfo.baseName());
        //arqIndividual.preanalyze(nomePrincipal, idioma, tipoanalise);
        if (progress.wasCanceled())
            break;
    }
}
//Esta função procura no arquivo 
void guiPalabras::on_actionFuncionais_triggered()
{
    trataTexto arqIndividual;
    ofstream arquivoSaida;
    QString path;
    QString tipoNFunc = "AQ,R,N,VM,JJ,RB,VB,WRB";//Tinha um FW como Tag
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;

    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    arquivoSaida.open("estatisticasfuncionais.txt", ios::out);
    arquivoSaida << "Arquivo\tpalContenido\tpalFuncionais\tpalVocabulario\n";

    foreach(path, this->listaArquivosEntrada)
    {
           //QFileInfo arqInfo(path);
           //QString arqTag = arqInfo.baseName() + ".tag";
        if (!arqIndividual.abreArquivoTAG(path.toStdString(),specialFunc))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();

        }
        // saving contents:
        arquivoSaida << path.toStdString().c_str() << "\t"
                     << arqIndividual.palFuncionalT << "\t" << arqIndividual.palContenidoT <<
                        "\t" << arqIndividual.palVocabularioT << endl;

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
    arquivoSaida.close();

}

void guiPalabras::on_actionZipfPromedio_triggered()
{
    trataTexto arqIndividual;
    ofstream arquivoSaida;
    QString path;
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) FRQ para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;

    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    arquivoSaida.open("zipfpromedio.txt", ios::out);// Acrescentei isso arqInfo.path()+"/"+
    arquivoSaida << "Arquivo\tzipfpromedio\n";

    foreach(path, this->listaArquivosEntrada)
    {
        if (!arqIndividual.abreArquivoFRQ(path.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();

        }
        // saving contents:
        arquivoSaida << path.toStdString().c_str() << "\t"
                     << arqIndividual.zipfpromedio << endl;

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
    arquivoSaida.close();

}


void guiPalabras::on_actionTamSentencas_triggered()
{
    trataTexto arqIndividual;
    QString path;
    QString tipoNFunc = "AQ,R,N,VM,FW,JJ,RB,VB,WRB";
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;

    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqFfr = arqInfo.baseName() + ".ffr";
        if (!arqIndividual.abreArquivoTAGFFR(path.toStdString(),specialFunc,arqFfr.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();

        }

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
}


void guiPalabras::on_actionTags_triggered()
{
    DialogCrece para;
    trataTexto arqIndividual;
    ofstream arquivoCre,arqResum,arqResum50;
    QString path;
    if(para.exec()==0) return;
    QString tipoNFunc = para.getClasses();
    int passo=para.getPasso();
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;
    arqResum.open("resum.dat",ios::out);
    arqResum50.open("resum50k.dat",ios::out);
    arqResum << "arquivo\tTamanho\tWDN\tWDC\tNPN\tNPC\t";
    arqResum50 << "arquivo\tTamanho\tWDN\tWDC\tNPN\tNPC\t";
    for(int i=0;i<specialFunc.size();i++)
    {
        arqResum<< specialFunc.at(i).toStdString()<<"\t";
        arqResum50<< specialFunc.at(i).toStdString()<<"\t";
    }
    arqResum << endl;
    arqResum50 << endl;

    arqResum50.close();
    arqResum.close();



    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqCre = arqInfo.baseName() + ".cret";
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
        if (!arqIndividual.abreArquivoCRE(path.toStdString(),specialFunc,passo,arqCre.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();

        }
        arquivoCre.close();
    }
}

void guiPalabras::on_actionCrescPalabras_triggered()
{
    DialogCrece para;
    trataTexto arqIndividual;
    ofstream arquivoCre,arqResum,arqResum50;
    QString path;
    if(para.exec()==0) return;
    QString tipoNFunc = para.getClasses();
    int passo=para.getPasso();
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;

    arqResum.open("resumPal.dat",ios::out);
    arqResum50.open("resumPal55k.dat",ios::out);
    arqResum << "arquivo\tTamanho\tWDN\tWDC\tNPN\tNPC\t";
    arqResum50 << "arquivo\tTamanho\tWDN\tWDC\tNPN\tNPC\t";

    for(int i=0;i<specialFunc.size();i++)
    {
        arqResum<< specialFunc.at(i).toStdString()<<"\t";
        arqResum50<< specialFunc.at(i).toStdString()<<"\t";
    }
    arqResum << endl;
    arqResum50 << endl;
    arqResum50.close();
    arqResum.close();


    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqCre = arqInfo.baseName() + ".crep";
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
        if (!arqIndividual.abreArquivoCREP(path.toStdString(),specialFunc,passo,arqCre.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();

        }
        arquivoCre.close();
    }
}

void guiPalabras::on_actionCresSentencas_triggered()
{
    DialogCrece para;
    trataTexto arqIndividual;
    ofstream arquivoCre,arqResum,arqResum50;
    QString path;
    if(para.exec()==0) return;
    QString tipoNFunc = para.getClasses();
    int passo=para.getPasso();
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;
    arqResum.open("resumSentences.dat",ios::out);
    arqResum50.open("resumSentences55k.dat",ios::out);
    arqResum << "Tamanho\tNum_Sent\tWPS\tDWPC\tPPS\t";
    arqResum50 << "Tamanho\tNum_Sent\tWPS\tDWPC\tPPS\t";
    for(int i=0;i<specialFunc.size();i++)
    {
        arqResum<< specialFunc.at(i).toStdString()<<"\t";
        arqResum50<< specialFunc.at(i).toStdString()<<"\t";
    }
    arqResum << endl;
    arqResum50 << endl;

    arqResum50.close();
    arqResum.close();

    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqCre = arqInfo.baseName() + ".creS";
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
        if (!arqIndividual.abreArquivoCRES(path.toStdString(),specialFunc,passo,arqCre.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();
            break;

        }
        arquivoCre.close();
    }
}

void guiPalabras::on_actionDeclara_es_utterances_triggered()
{
    DialogCrece para;
    trataTexto arqIndividual;
    ofstream arquivoCre,arqResum,arqResum50;
    QString path;
    if(para.exec()==0) return;
    QString tipoNFunc = para.getClasses();
    int passo=para.getPasso();
    QStringList specialFunc = tipoNFunc.split(",");
    this->listaArquivosEntrada = QFileDialog::getOpenFileNames(
                this,
                tr("Escolha arquivo(s) TAG para abrir"),
                QString::null,
                QString::null);

    if(this->listaArquivosEntrada.size()==0)
        return;
    arqResum.open("resumUtt.dat",ios::out);
    arqResum50.open("resumUtt55k.dat",ios::out);
    arqResum << "Tamanho\tNum_Sent\tWPS\tDWPC\t";
    arqResum50 << "Tamanho\tNum_Sent\tWPS\tDWPC\t";
    for(int i=0;i<specialFunc.size();i++)
    {
        arqResum<< specialFunc.at(i).toStdString()<<"\t";
        arqResum50<< specialFunc.at(i).toStdString()<<"\t";
    }
    arqResum << endl;
    arqResum50 << endl;

    arqResum50.close();
    arqResum.close();

    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);
        QString arqCre = arqInfo.baseName() + ".creD";
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
        if (!arqIndividual.abreArquivoCRED(path.toStdString(),specialFunc,passo,arqCre.toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + path);
            msgBox.exec();
            break;

        }
        arquivoCre.close();
    }
}

void guiPalabras::on_actionSubs_Simples_triggered()
{
    trataTexto arqIndividual;
    QString path;
    QString nomeArqSubs;
    string alvo, subs;
    QStringList regrasAlvo;
    QStringList regrasSubs;
    ifstream arqSubs;
    nomeArqSubs = QFileDialog::getOpenFileName(
                this,
                tr("Escolha arquivo(s) de regras para abrir"),
                QString::null,
                QString::null);
    arqSubs.open(nomeArqSubs.toStdString().c_str());
    do{
        arqSubs >> alvo >> subs;
        QString Qalvo=alvo.c_str();
        QString Qsubs=subs.c_str();
        Qsubs.replace("\\s"," ");
        Qsubs.replace("\\d","");

        regrasAlvo.push_back(Qalvo);
        regrasSubs.push_back(Qsubs);
    }while(!arqSubs.eof());



    QProgressDialog progress("Aplicando Regras...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    foreach(path, this->listaArquivosEntrada)
    {
        cout<<"passei aqui"<<endl;
        progress.setValue(i++);
        QFileInfo arqInfo(path);
        //QString nomePrincipal = arqInfo.baseName();

        arqIndividual.simpleSubs(path, arqInfo.baseName(),regrasAlvo,regrasSubs);
        if (progress.wasCanceled())
            break;
    }

}
void guiPalabras::on_actionFrases_triggered(){
    trataTexto arqIndividual;
    QString path;
    QProgressDialog progress("Calculando...Aguarde", "Cancelar", 0,
                             this->listaArquivosEntrada.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    progress.show();
    arqIndividual.carregaFiltros();

    foreach(path, this->listaArquivosEntrada)
    {
        QFileInfo arqInfo(path);

        if (!arqIndividual.redeFrases((arqInfo.path()+"/"+arqInfo.baseName()+"Convertido").toStdString()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Existe um erro em seu arquivo" + arqInfo.baseName());
            msgBox.exec();

        }

        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
}
void guiPalabras::on_actionOp_es_de_rede_triggered()
{
    config op;
    op.exec();

}
void guiPalabras:: on_actionAjuda_triggered(){

}
