#include "tratatexto.h"
#include <QTextStream>
#include <list>
#include <QFile>
#include <queue>
#include <set>
#include <locale.h>
#include <functional>
#include <ctype.h>
#include <cstdlib>
#include <unistd.h>
using namespace std;

trataTexto::trataTexto()
{
    QString tipoPunct = "Fp,Fs,Fd,Fx,Fg,Fit,Fat";
    QStringList specialPunct = tipoPunct.split(",");
    for (int i=0; i<specialPunct.size(); i++)
        this->pontuacao.insert(pair<string,int>(specialPunct[i].toStdString(),0));
    carregaFiltros();
}

bool trataTexto::abreArquivo(string nome)
{

    ifstream  arquivo;
    string token;
    QStringList list1;
    int nPal=0;
    arquivo.open(nome.c_str(), ios::in);

    this->normal.clear();
    this->canonica.clear();
    list1.clear();
    while (!arquivo.eof())
    {
        getline(arquivo, token);
        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo1 = list1[0].toLower().toStdString();
        string codigo2 = list1[1].toLower().toStdString();
        if (list1[2][0] == 'F')
            continue;

        this->normal.insert(pair<string,int>(codigo1,normal[codigo1]+1));
        normal[codigo1]=normal[codigo1]+1;
        this->canonica.insert(pair<string,int>(codigo2,canonica[codigo2]+1));
        canonica[codigo2]=canonica[codigo2]+1;
        nPal++;

    }

    arquivo.close();
    this->palDiffN = this->normal.size();
    this->palDiffC = this->canonica.size();
    this->palTot = nPal;
    return true;
}


bool trataTexto::abreArquivoTAG(string nome, QStringList tags)
{
    ifstream  arquivo;
    string token;
    QStringList list1;
    int nPalFuncional=0;
    int nPalContenido=0;
    arquivo.open(nome.c_str(), ios::in);
    this->vocabulario.clear();

    while (!arquivo.eof()) // && (nPalFuncional + nPalContenido <= 4000))
    {
        int i=0;
        getline(arquivo, token);

        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo = list1[2].toStdString();
        if (list1[2][0] == 'F')
            continue;
        string codigo2 = list1[1].toLower().toStdString();

        for (i=0;i<tags.size();i++)
        {
            string aux=codigo.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                this->vocabulario.insert(pair<string,int>(codigo2,vocabulario[codigo2]+1));
                vocabulario[codigo2]=vocabulario[codigo2]+1;
                nPalContenido++;
                break;
            }
        }

        if (i==tags.size())
            nPalFuncional++;
    }
    arquivo.close();
    this->palVocabularioT = this->vocabulario.size();
    this->palFuncionalT=nPalFuncional;
    this->palContenidoT=nPalContenido;
    return true;
}



bool trataTexto::abreArquivoCRE(string nome, QStringList tags,int passo, string arqCre)
{
    ifstream  arquivo;
    ofstream arquivoCre,arqResum;
    int totCanAn=0;
    int totNorAn=0;
    map<string,string>::iterator it;
    string token;
    vector <int> tagContNov;
    vector <int> tagCont;

    QStringList list1;
    int tamanho=passo;
    int nPal=0;
    if(passo==0) tamanho=99999999; // garante que apenas o ultimo valor é impresso.


    arquivo.open(nome.c_str(), ios::in);

    arquivoCre.open(arqCre.c_str(),ios::out);
    arquivoCre << "Tamanho\tWDN\tWDC\tNPN\tNPC\t";
    for(int i=0;i<tags.size();i++) arquivoCre << tags[i].toStdString() <<"_New\t"
            << tags[i].toStdString() <<"\t";
    arquivoCre << endl;

    this->normal.clear();
    this->canonica.clear();
    list1.clear();

    tagContNov.assign(tags.size(),0);
    tagCont.assign(tags.size(),0);

    while (!arquivo.eof())
    {
        getline(arquivo, token);
        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo1 = list1[0].toLower().toStdString();  // nomal
        string codigo2 = list1[1].toLower().toStdString();  // canonica
        string codigo3 = list1[2].toStdString();            // tag
        if (list1[2][0] == 'F')
            continue;

        this->normal.insert(pair<string,int>(codigo1,normal[codigo1]+1));
        normal[codigo1]=normal[codigo1]+1;
        this->canonica.insert(pair<string,int>(codigo2,canonica[codigo2]+1));

        canonica[codigo2]=canonica[codigo2]+1;
        for (int i=0;i<tags.size();i++)
        {
            string aux=codigo3.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                if (canonica[codigo2]==1)   // caso seja a primeira ocorrencia
                    tagContNov[i]++;
                tagCont[i]++;
            }
        }
        nPal++;
        if(nPal>=tamanho)
        {
            arquivoCre << nPal <<"\t" << this->normal.size() << "\t" <<
                    this->canonica.size()<<"\t"<<
                    this->normal.size()-totNorAn<<"\t"<<
                    this->canonica.size()-totCanAn<<"\t";
            totNorAn=normal.size();
            totCanAn=canonica.size();
            for(unsigned int i=0;i<tagCont.size();i++)
            {
                arquivoCre << tagContNov[i] << "\t" << tagCont[i] <<"\t";
            }
            arquivoCre << endl;

            tamanho+=passo;
        }
        if(passo==0 && nPal>=55000) // resumo a 55000 palavras
        {
            arqResum.open("resum55k.dat",ios::app);
            arqResum << arqCre << "\t" << nPal <<"\t" << this->normal.size() << "\t" <<
                    this->canonica.size()<<"\t"<<
                    this->normal.size()-totNorAn<<"\t"<<
                    this->canonica.size()-totCanAn<<"\t";
            for(unsigned int i=0;i<tagCont.size();i++)
                arqResum << tagContNov[i] << "\t" << tagCont[i] <<"\t";
            arqResum << endl;
            arqResum.close();
            passo=1;    //imprime soh uma vez

        }

    }

    // Imprime a última linha
    arquivoCre << nPal <<"\t" << this->normal.size() << "\t" <<
            this->canonica.size()<<"\t"<<
            this->normal.size()-totNorAn<<"\t"<<
            this->canonica.size()-totCanAn<<"\t";
    totNorAn=normal.size();
    totCanAn=canonica.size();
    for(unsigned int i=0;i<tagCont.size();i++)
    {
        arquivoCre << tagContNov[i] << "\t" << tagCont[i] <<"\t";
    }
    arquivoCre << endl;
    // ---------------------

    arquivo.close();
    arquivoCre.close();
    this->palDiffN = this->normal.size();
    this->palDiffC = this->canonica.size();
    this->palTot = nPal;
    arqResum.open("resum.dat",ios::app);
    arqResum << arqCre << "\t" << nPal <<"\t" << this->normal.size() << "\t" <<
            this->canonica.size()<<"\t"<<
            this->normal.size()-totNorAn<<"\t"<<
            this->canonica.size()-totCanAn<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
        arqResum << tagContNov[i] << "\t" << tagCont[i] <<"\t";
    arqResum << endl;
    arqResum.close();

    return true;
}


bool trataTexto::abreArquivoCREP(string nome, QStringList tags,int passo, string arqCre)
{
    ifstream  arquivo;
    ofstream arquivoCre,arqResum;
    int totCanAn=0;
    int totNorAn=0;
    map<string,string>::iterator it;
    string token;
    vector <int> tagContNov;
    vector <int> tagCont;

    QStringList list1;
    int tamanho=passo;
    int nPal=0;

    arquivo.open(nome.c_str(), ios::in);

    arquivoCre.open(arqCre.c_str(),ios::out);
    arquivoCre << "Tamanho\tWDN\tWDC\tNPN\tNPC\t";
    for(int i=0;i<tags.size();i++) arquivoCre << tags[i].toStdString() <<"\t";
    arquivoCre << endl;

    this->normal.clear();
    this->canonica.clear();
    list1.clear();
    if(passo==0) tamanho=99999999; // garante que apenas o ultimo valor é impresso.

    tagCont.assign(tags.size(),0);

    while (!arquivo.eof())
    {
        getline(arquivo, token);
        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo1 = list1[0].toLower().toStdString();  // nomal
        string codigo2 = list1[1].toLower().toStdString();  // canonica
        string codigo3 = list1[2].toStdString();            // tag
        if (list1[2][0] == 'F')
            continue;

        this->normal.insert(pair<string,int>(codigo1,normal[codigo1]+1));
        normal[codigo1]=normal[codigo1]+1;
        this->canonica.insert(pair<string,int>(codigo2,canonica[codigo2]+1));

        canonica[codigo2]=canonica[codigo2]+1;
        for (int i=0;i<tags.size();i++)
        {
            string aux=codigo2.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                tagCont[i]++;
            }
        }
        nPal++;
        if(nPal>=tamanho)
        {
            arquivoCre << nPal <<"\t" << this->normal.size() << "\t" <<
                    this->canonica.size()<<"\t"<<
                    this->normal.size()-totNorAn<<"\t"<<
                    this->canonica.size()-totCanAn<<"\t";
            totNorAn=normal.size();
            totCanAn=canonica.size();
            for(unsigned int i=0;i<tagCont.size();i++)
            {
                arquivoCre <<tagCont[i] <<"\t";
            }
            arquivoCre << endl;

            tamanho+=passo;
        }
        if(passo==0 && nPal>=55000) // resumo a 55000 palavras
        {
            arqResum.open("resumPal55k.dat",ios::app);
            arqResum << arqCre<< "\t" << nPal <<"\t" << this->normal.size() << "\t" <<
                    this->canonica.size()<<"\t"<<
                    this->normal.size()-totNorAn<<"\t"<<
                    this->canonica.size()-totCanAn<<"\t";
            for(unsigned int i=0;i<tagCont.size();i++)
                arqResum << tagCont[i]  <<"\t";
            arqResum << endl;
            arqResum.close();
            passo=1;    //imprime soh uma vez

        }


    }

    // Imprime a última linha
    arquivoCre << nPal <<"\t" << this->normal.size() << "\t" <<
            this->canonica.size()<<"\t"<<
            this->normal.size()-totNorAn<<"\t"<<
            this->canonica.size()-totCanAn<<"\t";
    totNorAn=normal.size();
    totCanAn=canonica.size();
    for(unsigned int i=0;i<tagCont.size();i++)
    {
        arquivoCre << tagCont[i]  <<"\t";
    }
    arquivoCre << endl;
    // ---------------------

    arquivo.close();
    arquivoCre.close();
    this->palDiffN = this->normal.size();
    this->palDiffC = this->canonica.size();
    this->palTot = nPal;
    arqResum.open("resumPal.dat",ios::app);
    arqResum << arqCre<< "\t" << nPal <<"\t" << this->normal.size() << "\t" <<
            this->canonica.size()<<"\t"<<
            this->normal.size()-totNorAn<<"\t"<<
            this->canonica.size()-totCanAn<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
        arqResum << tagCont[i]  <<"\t";
    arqResum << endl;
    arqResum.close();

    return true;
}


bool trataTexto::abreArquivoCRES(string nome, QStringList tags,int passo, string arqCre)
{
    ifstream  arquivo;
    ofstream arquivoCre,arqResum;
    int nSent=0;        // quantidade de sentenças
    int nPaus=0;        // Numero de Pausas
    int nDWPS=0;        // Acumulado da quantidade de palavras diferentes na sentença
    map<string,string>::iterator it;
    string token;
    vector <int> tagCont;

    QStringList list1;
    int tamanho=passo;
    int nPal=0;

    if(passo==0) tamanho=99999999; // garante que apenas o ultimo valor é impresso.

    arquivo.open(nome.c_str(), ios::in);

    arquivoCre.open(arqCre.c_str(),ios::out);
    arquivoCre << "Tamanho\tNum_Sent\tWPS\tDWPC\tPPS\t";
    for(int i=0;i<tags.size();i++) arquivoCre << tags[i].toStdString() <<"\t";
    arquivoCre << endl;

    this->normal.clear();
    this->canonica.clear();
    list1.clear();
    QStringList endSentences,endPauses;
    endSentences << "Fat" << "Fp" << "Fs" << "Fit";
    endPauses << "Fat" << "Fd" << "Fp" << "Fs" << "Fx" <<"Fit" << "Fg" <<"Fe" <<"Fc";

    tagCont.assign(tags.size(),0);

    while (!arquivo.eof())
    {
        bool fS,fP;
        getline(arquivo, token);
        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo1 = list1[0].toLower().toStdString();  // nomal
        string codigo2 = list1[1].toLower().toStdString();  // canonica
        string codigo3 = list1[2].toStdString();            // tag

        fS=endSentences.contains(list1[2],Qt::CaseInsensitive);
        fP=endPauses.contains(list1[2],Qt::CaseInsensitive);

        if (fS|| fP)
        {
            if(fS)nSent++;
            if(fP)nPaus++;
            nDWPS+=this->canonica.size();
            this->canonica.clear();
            this->normal.clear();
            if(nPal>=tamanho)
            {
                arquivoCre << nPal <<"\t" << nSent << "\t" <<
                        (double)nPal/nSent<<"\t"<<
                        (double)nDWPS/nSent<<"\t" <<
                        (double) nPal/nPaus<<"\t";
                for(unsigned int i=0;i<tagCont.size();i++)
                {
                    arquivoCre << (double)tagCont[i]/nSent <<"\t";
                }
                arquivoCre << endl;

                tamanho+=passo;
            }
            if(passo==0 && nPal>=55000) // resumo a 50000 palavras
            {
                arqResum.open("resumSentences55k.dat",ios::app);
                arqResum << arqCre << "\t" << nSent <<"\t" << (double)nPal/nSent << "\t" <<
                        (double)nDWPS/nSent<<"\t" << (double) nPal/nPaus<<"\t";
                for(unsigned int i=0;i<tagCont.size();i++)
                    arqResum << (double)tagCont[i]/nSent  <<"\t";
                arqResum << endl;
                arqResum.close();
                passo=1;    //imprime soh uma vez

            }

            continue;
        }
        else
        {
            nPal++;
        }
        this->normal.insert(pair<string,int>(codigo1,normal[codigo1]+1));
        this->canonica.insert(pair<string,int>(codigo2,canonica[codigo2]+1));

        for (int i=0;i<tags.size();i++)
        {
            string aux=codigo3.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                tagCont[i]++;
            }
        }
    }

    // Imprime a última linha
    arquivoCre << nPal <<"\t" << nSent << "\t" <<
            (double)nPal/nSent<<"\t"<<
            (double)nDWPS/nSent<<"\t"<< (double) nPal/nPaus<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
    {
        arquivoCre <<(double)tagCont[i]/nSent <<"\t";
    }
    arquivoCre << endl;
    // ---------------------

    arquivo.close();
    arquivoCre.close();
    this->palDiffN = this->normal.size();
    this->palDiffC = this->canonica.size();
    this->palTot = nPal;
    arqResum.open("resumSentences.dat",ios::app);
    arqResum << arqCre << "\t" << nSent <<"\t" << (double)nPal/nSent << "\t" <<
            (double)nDWPS/nSent<<"\t"<<(double) nPal/nPaus<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
        arqResum << (double)tagCont[i]/nSent  <<"\t";
    arqResum << endl;
    arqResum.close();

    return true;
}

bool trataTexto::abreArquivoCRED(string nome, QStringList tags,int passo, string arqCre)
{
    ifstream  arquivo;
    ofstream arquivoCre,arqResum;
    int nSent=0;        // quantidade de sentenças
    int nDWPS=0;        // Acumulado da quantidade de palavras diferentes na sentença
    map<string,string>::iterator it;
    string token;
    vector <int> tagCont;

    QStringList list1;
    int tamanho=passo;
    int nPal=0;

    if(passo==0) tamanho=99999999; // garante que apenas o ultimo valor é impresso.

    arquivo.open(nome.c_str(), ios::in);

    arquivoCre.open(arqCre.c_str(),ios::out);
    arquivoCre << "Tamanho\tNum_Sent\tWPS\tDWPC\t";
    for(int i=0;i<tags.size();i++) arquivoCre << tags[i].toStdString() <<"\t";
    arquivoCre << endl;

    this->normal.clear();
    this->canonica.clear();
    list1.clear();

    tagCont.assign(tags.size(),0);

    while (!arquivo.eof())
    {
        getline(arquivo, token);
        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo1 = list1[0].toLower().toStdString();  // nomal
        string codigo2 = list1[1].toLower().toStdString();  // canonica
        string codigo3 = list1[2].toStdString();            // tag
        if (list1[2][0] == 'F')
        {
            nSent++;
            nDWPS+=this->canonica.size();
            this->canonica.clear();
            this->normal.clear();
            if(nPal>=tamanho)
            {
                arquivoCre << nPal <<"\t" << nSent << "\t" <<
                        (double)nPal/nSent<<"\t"<<
                        (double)nDWPS/nSent<<"\t";
                for(unsigned int i=0;i<tagCont.size();i++)
                {
                    arquivoCre << (double)tagCont[i]/nSent <<"\t";
                }
                arquivoCre << endl;

                tamanho+=passo;
            }
            if(passo==0 && nPal>=55000) // resumo a 50000 palavras
            {
                arqResum.open("resumUtt55k.dat",ios::app);
                arqResum << arqCre << "\t" << nSent <<"\t" << (double)nPal/nSent << "\t" <<
                        (double)nDWPS/nSent<<"\t";
                for(unsigned int i=0;i<tagCont.size();i++)
                    arqResum << (double)tagCont[i]/nSent  <<"\t";
                arqResum << endl;
                arqResum.close();
                passo=1;    //imprime soh uma vez

            }

            continue;
        }
        this->normal.insert(pair<string,int>(codigo1,normal[codigo1]+1));
        this->canonica.insert(pair<string,int>(codigo2,canonica[codigo2]+1));

        for (int i=0;i<tags.size();i++)
        {
            string aux=codigo3.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                tagCont[i]++;
            }
        }
        nPal++;
    }

    // Imprime a última linha
    arquivoCre << nPal <<"\t" << nSent << "\t" <<
            (double)nPal/nSent<<"\t"<<
            (double)nDWPS/nSent<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
    {
        arquivoCre <<(double)tagCont[i]/nSent <<"\t";
    }
    arquivoCre << endl;
    // ---------------------

    arquivo.close();
    arquivoCre.close();
    this->palDiffN = this->normal.size();
    this->palDiffC = this->canonica.size();
    this->palTot = nPal;
    arqResum.open("resumUtt.dat",ios::app);
    arqResum << arqCre << "\t" << nSent <<"\t" << (double)nPal/nSent << "\t" <<
            (double)nDWPS/nSent<<"\t";
    for(unsigned int i=0;i<tagCont.size();i++)
        arqResum << (double)tagCont[i]/nSent  <<"\t";
    arqResum << endl;
    arqResum.close();

    return true;
}

void trataTexto::carregaFiltros()
{
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
        this->filtroGram.push_back(QString(maxLinha.c_str()));
    }

    while(!arqPl.eof())
    {
        getline(arqPl,maxLinha);
        this->filtroPala.push_back(QString(maxLinha.c_str()));
    }
    arqGr.close();
    arqPl.close();
}

bool trataTexto::filtro(QStringList token)
{
    bool flagGr,flagPl;
    flagGr=flagPl=false;
    for(int i=0;i<this->filtroGram.size();i++)
    {
        int tam=this->filtroGram[i].size();
        if(tam==0) continue;
        if(this->filtroGram[i].toUpper()==token[2].left(tam).toUpper())
        {
            flagGr=true;
            break;
        }
    }
    for(int i=0;i<this->filtroPala.size();i++)
    {
        int tam=this->filtroPala[i].size();
        if(tam==0) continue;
        if(this->filtroPala[i].toUpper()==token[1].toUpper())
        {
            flagPl=true;
            break;
        }
    }
    return flagGr||flagPl;
}



bool trataTexto::abreArquivoFRQ(string nome)
{
    ifstream  arquivo;
    string token;
    QStringList list1;
    list <int> frequencia;
    list <int>::iterator it;
    arquivo.open(nome.c_str(), ios::in);

    getline(arquivo, token);

    while (!arquivo.eof())
    {
        getline(arquivo, token);

        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split("\t");
        if (list1.size() < 2)
            return false;
        int freq = list1[1].toInt();
        frequencia.push_back(freq);
    }
    arquivo.close();

    frequencia.sort();
    int soma=0;
    int somaFreq=0;
    int i=0;
    for (it=frequencia.begin();it!=frequencia.end();++it)
    {
        soma+= (*it*(frequencia.size()-i));
        somaFreq+= *it;
        i++;

    }

    this->zipfpromedio=(double)soma/somaFreq;

    return true;
}


bool trataTexto::abreArquivoTAGFFR(string nome, QStringList tags, string arqFfr)
{
    ifstream  arqEntrada;
    ofstream arqSaida;
    string token;
    QStringList list1;
    vector <int> totPalavras;
    vector <int> palContenido;
    vector <int> tamFrase;
    vector <int> qtdFrases;
    arqEntrada.open(nome.c_str(), ios::in);
    arqSaida.open(arqFfr.c_str(), ios::out);

    totPalavras.push_back(0);
    palContenido.push_back(0);
    int maxTamSent=0;
    int is=0;

    while (!arqEntrada.eof())
    {
        int i=0;
        getline(arqEntrada, token);

        if(token.size() == 0)
            continue;
        QString qtoken;
        QTextStream meleca(token.c_str());
        qtoken = meleca.readLine(1000);
        list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;
        string codigo = list1[2].toStdString();
        if (!list1[2].compare("Fat")  || !list1[2].compare("Fd") || !list1[2].compare("Fit")
            || !list1[2].compare("Fp") || !list1[2].compare("Fs") || !list1[2].compare("Fx"))
            {
            totPalavras.push_back(0);
            palContenido.push_back(0);
            maxTamSent = (totPalavras[is]>maxTamSent)?totPalavras[is]:maxTamSent;
            is++;
            continue;
        }

        if (list1[2][0] == 'F' && list1[2].compare("Fw"))
            continue;

        totPalavras[is]++;

        for (i=0;i<tags.size();i++)
        {
            string aux=codigo.substr(0,tags[i].size());
            if (aux.compare(tags[i].toStdString())==0)
            {
                palContenido[is]++;
                break;
            }
        }
    }
    arqEntrada.close();
    tamFrase.assign(maxTamSent+1,0);
    qtdFrases.assign(maxTamSent+1,0);

    for (unsigned int i=0;i<totPalavras.size()-1;i++)
    {
        tamFrase[totPalavras[i]]+= palContenido[i];
        qtdFrases[totPalavras[i]]++;
    }

    arqSaida << "TamanhoSentencas \t TotalPalavrasContenido \t QtdSentenĂ§as \t MediaPalavrasContenido" << endl;

    for (unsigned int i=1;i<tamFrase.size();i++)
    {
        if(qtdFrases[i])
            arqSaida << i << "\t" << tamFrase[i] << "\t" << qtdFrases[i] << "\t" << (double)tamFrase[i]/qtdFrases[i] << "\t" << (double)qtdFrases[i]/(is-1) << "\t" << endl;

    }
    arqSaida.close();
    return true;
}

void trataTexto::preanalyze(QString baseName, char idioma, char tipoAnalise)
{
    //QString arqAnalyze;
    QString arqConfig, arqConfig2;

    if (idioma == 1)
        arqConfig = "en";
    else{
        arqConfig = "pt";
    }

    if (tipoAnalise == 1)
        arqConfig = arqConfig /*+ "Normal"*/;
    else if (tipoAnalise == 2)
        arqConfig = arqConfig /*+ "Canonica"*/;
    else
    {
        arqConfig = arqConfig /*+ "Canonica"*/;
        arqConfig2 = arqConfig /*+ "Normal"*/;
        QString comando2("analyze -f " + arqConfig2 + ".cfg  tagged <\"" +
                         baseName + ".pre\"> \"" + baseName + ".tag\"");
        system(comando2.toStdString().c_str());
    }

    QString comando("analyze -f " + arqConfig + ".cfg  tagged <\"" +
                    baseName + ".pre\"> \"" + baseName + ".tag\"");
    system(comando.toStdString().c_str());
}


bool trataTexto::redePalavras (string infArq, int janela)
{


    vector <string> janPalNormal, janPalCanonica;
    QStringList list1;
    string nomeArquivo = infArq + ".tag";
    QFile arquivo(nomeArquivo.c_str());
    arquivo.open(QIODevice::ReadOnly);
    QTextStream in(&arquivo);
    in.setCodec("UTF-8");


    this->normal.clear();
    this->canonica.clear();
    this->normalArestas.clear();
    this->canonicaArestas.clear();

    list1.clear();

    for (int i=0;i<janela;i++)
    {
        QString qtoken = in.readLine(1000);
        if(qtoken.size() == 0)
        {
            i--;
            continue;
        }
        list1 = qtoken.split(" ");

        if (list1.size() < 4)
            return false;
        if(filtro(list1))           // desconsidera as palavras e formas canonicas preselecionadas.
        {
            i--;
            continue;
        }
        string codigo1 = list1[0].toLower().toStdString(); // normal
        string codigo2 = list1[1].toLower().toStdString(); // canonica
        if (list1[2][0] == 'F')
        {
            i--;
            continue;
        }
        this->normal.insert(pair<string,int>(codigo1,normal.size()));
        this->canonica.insert(pair<string,int>(codigo2,canonica.size()));

        janPalNormal.push_back(codigo1);
        janPalCanonica.push_back(codigo2);
    }

    while(!in.atEnd())
    {
        for (unsigned i=0;i<janPalNormal.size()-1;i++)
        {
            for (unsigned j=i+1; j<janPalNormal.size(); j++)
            {
                string codigo1 = janPalNormal[i] + " " + janPalNormal[j];
                string codigo2 = janPalCanonica[i] + " " + janPalCanonica[j];
                string codigo3 = janPalNormal[j] + " " + janPalNormal[i];
                string codigo4 = janPalCanonica[j] + " " + janPalCanonica[i];
/*O map esta inserindo nas chaves codigo1,2 e 3 um contador para cada vez que eles aparecerem no texto
 * este contador parece ser o peso da aresta*/
                this->normalArestas.insert(pair<string,int>(codigo1,normalArestas[codigo1]++));
                this->normalArestas.insert(pair<string,int>(codigo3,normalArestas[codigo3]++));
                this->canonicaArestas.insert(pair<string,int>(codigo2,canonicaArestas[codigo2]++));
                this->canonicaArestas.insert(pair<string,int>(codigo4,canonicaArestas[codigo4]++));
            }
        }
        janPalNormal.erase(janPalNormal.begin());
        janPalCanonica.erase(janPalCanonica.begin());

        LER:
        QString qtoken = in.readLine(1000);
        if(qtoken.size() == 0)
        {
            if (in.atEnd()) continue;
            else goto LER;
        }
        list1 = qtoken.split(" ");

        //QString qtoken(token.c_str());
        //list1 = qtoken.split(" ");
        if (list1.size() < 4)
            return false;

        if(filtro(list1))           // desconsidera as palavras e formas canonicas preselecionadas.
        {
            if (in.atEnd()) continue;
            else goto LER;
        }
        if (list1[2][0] == 'F')
        {
            if (in.atEnd()) continue;
            else goto LER;
        }
        string codigo1 = list1[0].toLower().toStdString();
        string codigo2 = list1[1].toLower().toStdString();
        janPalNormal.push_back(codigo1);
        janPalCanonica.push_back(codigo2);
        this->normal.insert(pair<string,int>(codigo1,normal.size()));
        this->canonica.insert(pair<string,int>(codigo2,canonica.size()));
    }
    arquivo.close();

    this->imprimeNet(infArq);

    return true;

}

string trataTexto:: GetStdoutFromCommand(string cmd) {

    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
    while (!feof(stream))
    if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
}
return data;
}


void trataTexto::lowerWord (QString nomeArqE, QString nomeArqS)
{
    //fstream arquivoEntrada;
    //fstream arquivoSaida;
    //arquivoEntrada.open(nomeArqE);
    //arquivoSaida.open(nomeArqS+".pre");
    QFile arqEntrada(nomeArqE);
    QFile arqSaida(nomeArqS + ".pre");
    char maxLinha[50];
    arqEntrada.open(QIODevice::ReadOnly);
    arqSaida.open(QIODevice::WriteOnly);
    while(arqEntrada.readLine( maxLinha,sizeof(maxLinha))>0)
    {
        int cont=0;
        string aux;
        while(maxLinha[cont]!='\0'){
            if(maxLinha[cont]=='\0'){
                break;
            }
            if((maxLinha[cont]>='A'&& maxLinha[cont]<='Z')){
                maxLinha[cont]=maxLinha[cont]+32;
            }
           /* switch (maxLinha[cont]) {
            case 'É':maxLinha[cont]='é';
                break;
            case 'Ó': maxLinha[cont]='ó';
                break;
            case 'Ç': maxLinha[cont]='ç';
                        break;
            case 'Á': maxLinha[cont]='á';
                break;
             case 'À': maxLinha[cont]='à';
                break;
             case 'Ô': maxLinha[cont]='ô';
                break;
             case 'Ê': maxLinha[cont]='ê';
                break;
             case 'Í': maxLinha[cont]='í';
                break;
              case 'Ã': maxLinha[cont]='ã';
                break;
               case 'Õ': maxLinha[cont]='õ';
                break;
                case 'Ẽ': maxLinha[cont]='ẽ';
            default:
                break;
            }*/
            cont++;
        }

        QTextStream outStream(&arqSaida);
        outStream<<maxLinha;

       /* QString qlinha =maxLinha;
        qlinha=qlinha.toLower();
        qlinha=qlinha.toUtf8();
        arqSaida.write(qlinha.toStdString().c_str());
        */

    }
    arqEntrada.close();
    arqSaida.close();

}

void trataTexto::simpleSubs (QString nomeArqE, QString nomeArqS, QStringList regAlvo, QStringList regSubs)
{
    QFile arqEntrada(nomeArqE);
    QFile arqSaida(nomeArqS + "Sub.txt");
    char maxLinha[1000];

    arqEntrada.open(QIODevice::ReadOnly);
    arqSaida.open(QIODevice::WriteOnly);

    while(arqEntrada.readLine(maxLinha,1000)>0)
    {
        QString qlinha = maxLinha;
        for(unsigned i=0;i<regAlvo.size();i++)
            qlinha=qlinha.replace(QRegExp(regAlvo[i]), regSubs[i]);
        qlinha=qlinha.toUtf8();
        arqSaida.write(qlinha.toStdString().c_str());
    }
    arqEntrada.close();
    arqSaida.close();

}


void trataTexto::imprimeNet (string nomeArq)
{
    QFile arqSaidaCan(QString(nomeArq.c_str()) + "redPalCan.csv");//define os nomes dos arquivos de saida e sua terminação
    QFile arqSaidaNormal(QString(nomeArq.c_str()) + "redPalNor.csv");
    int i=0;
    //da permição de escrita para os arquivos criados
    arqSaidaCan.open(QIODevice::WriteOnly);
    arqSaidaNormal.open(QIODevice::WriteOnly);

    //Escreve a quantidade de vertices obtida através do tamanho da variavel canonica e normal
    QString qlinha="Source Target \n";
    arqSaidaCan.write(qlinha.toStdString().c_str());
    qlinha="Source Target \n";
    arqSaidaNormal.write(qlinha.toStdString().c_str());
    //aqui ele esta imprimindo o numero da palavra e a palavra
    i = 1;
    for (this->it = this->canonica.begin(); this->it != this->canonica.end(); this->it++)
    {
        qlinha = QString::number(i)+" \""+QString(this->it->first.c_str())+" \"\n";
        qlinha=qlinha.toLower();
        qlinha=qlinha.toLocal8Bit();
        //arqSaidaCan.write(qlinha.toStdString().c_str());
        this->it->second = i++; //Neste ponto aqui ele atribui a canonica.second o valor de i++
    }
    i = 1;

    for (this->it = this->normal.begin(); this->it != this->normal.end(); this->it++)
    {
        qlinha = QString::number(i)+" \""+QString(this->it->first.c_str())+" \"\n";
        qlinha=qlinha.toLower();
        qlinha=qlinha.toLocal8Bit();
       // arqSaidaNormal.write(qlinha.toStdString().c_str());
        this->it->second = i++; //atribui a normal.second o valor de i++
    }

    //imprime "*edges" nos dois arquivos para simbolizar que a partir dali começa a parte das arestas
   // arqSaidaCan.write("*edges\n");
    //arqSaidaNormal.write("*edges\n");

    //laço responsavel por imprimir as arestas entre os vertices e seus pesos
    for (this->it = this->canonicaArestas.begin(); this->it != this->canonicaArestas.end(); this->it++)
    {
        QString palavra(this->it->first.c_str()); //atribui a variavel palavra o valor de canonicaArestas.first que são pares de palavras em redeSentenças e individuais em rede de Palavras
        QStringList lPalavra = palavra.split(" ");//Caso sejam pares de palavras ele divide ela quando achar um espaço
        QString pal1=QString::number(this->canonica[lPalavra[0].toStdString()]);//pega o indice(rotulo do vertice) da primeira palavra
        QString pal2=QString::number(this->canonica[lPalavra[1].toStdString()]);//pega o indice(rotulo do vertice) da segunda palavra
       // QString qlinha = pal1 + " " + pal2 + " " +
           QString qlinha =lPalavra[0]+" "+lPalavra[1]+ " " +
                          QString::number(this->it->second)+ "\n";
        arqSaidaCan.write(qlinha.toStdString().c_str());//imprime no arquivo o formato "tagVetice1 tagVertice2 Peso"
    }
//Neste laço o mesmo processo é refeito, no entanto com as palavras normais
    for (this->it = this->normalArestas.begin(); this->it != this->normalArestas.end(); this->it++)
    {
        QString palavra(this->it->first.c_str());
        QStringList lPalavra = palavra.split(" ");
        QString pal1=QString::number(this->normal[lPalavra[0].toStdString()]);
        QString pal2=QString::number(this->normal[lPalavra[1].toStdString()]);
       // QString qlinha = pal1 + " " + pal2 + " " +
        QString qlinha= lPalavra[0]+" "+ lPalavra[1]+" "+
                         QString::number(this->it->second)+ "\n";
        arqSaidaNormal.write(qlinha.toStdString().c_str());
    }
    arqSaidaCan.close();
    arqSaidaNormal.close();
}

bool trataTexto::redeSentencas (string infArq)
{
    string token;//Não foi utilizado para nada
    vector <string> sentPalNormal, sentPalCanonica;//guarda as palavras da sentença
    string nomeArquivo = infArq + ".tag";
    QFile arquivo(nomeArquivo.c_str());
    arquivo.open(QIODevice::ReadOnly);
    QTextStream in(&arquivo);
    in.setCodec("UTF-8");
    //in.setCodec("LATIN");


    map<string,queue<string> > grafo;//Armazena em cada substantivo a sua fila de sentencas
    queue<string> substantivos;
    set <string> subConj;


    this->normal.clear();
    this->canonica.clear();
    this->normalArestas.clear();
    this->canonicaArestas.clear();
    this->redeSentencasImpressaoCanonica.clear();
    this->redeSentencasImpressaoNormal.clear();

    int quantidadeDeVertices = 0;
    while(!in.atEnd())
    {
        sentPalNormal.clear();
        sentPalCanonica.clear();

        string sentenca;
        int quantidadeDeSubstantivos = 0;
        bool tem_substantivo = false; // cond
        bool terminou = false;


        while(!in.atEnd())
        {
            QString linha = in.readLine(1000);
            if(linha.isEmpty())
                continue;

            QStringList tokens = linha.split(" ");
            if (tokens.size() < 4)
                return false;

           if(filtro(tokens))           // desconsidera as palavras e formas canonicas preselecionadas.
                continue;

            string tag = tokens[2].toStdString();
            string palavra = tokens[0].toLower().toStdString();
            string canonica = tokens[1].toLower().toStdString();
            if(tag[0]!='F' && !filtro(tokens)){
                if(sentenca.size()>0)
                    sentenca = sentenca + "_" + canonica;
                else
                    sentenca=sentenca+canonica;

            }
            if (/*eFimSentenca(tokens[2]) ||*/ tag[0] == 'F' && (tag[1]=='a' ||(tag[1]=='i')|| tag[1]=='p'||tag[1]=='s'))
            {
                terminou = true;

                if (tem_substantivo)
                {
                    quantidadeDeVertices = quantidadeDeVertices+1;
                    for (int r = 0; r < quantidadeDeSubstantivos; ++r)
                    {
                        grafo[substantivos.front()].push(sentenca);
                        substantivos.pop();
                    }
                }
                break;

            }
            else
            {
                if (tag[0]=='N'){
                    tem_substantivo=true;
                    ++quantidadeDeSubstantivos;
                    //aux.push(primeiraPalavraDaLinha);
                    subConj.insert(canonica);
                    substantivos.push(canonica);

                }
            }
        }
    }

    arquivo.close();

    nomeArquivo = infArq + ".redeSent.csv";
    QFile arquivoNet(nomeArquivo.c_str());

    if (!arquivoNet.open(QIODevice::WriteOnly))
        cerr << "erro ao abrir!\n";




    int contar=0,anteriorCond=0;
    hash<string> calcula_hash;

    //QString qlinha = "*vertices " + QString::number(quantidadeDeVertices) + "\n";
   // arquivoNet.write(qlinha.toUtf8());

    int quantidadeDePares=0;
    //queue<pair<size_t,size_t> > arestas;	//Armazena a lista com os pares de arestas para liga-los no grafo
    queue<pair<string,string> > arestas;
    for(set<string>::iterator it= subConj.begin();it!=subConj.end();it++){
        ++contar;
        bool primeiro = true;
        //size_t anterior = 0;
        string sentAnterior;
        while (!grafo[*it].empty())
        {
            size_t id = calcula_hash(grafo[*it].front());
            //arquivoNet.write(QString::number(id).toUtf8());
            //arquivoNet.write(" \"");
            //arquivoNet.write(grafo[*it].front().c_str());
           // arquivoNet.write("\"");

            if(anteriorCond==contar-1 && !primeiro)
            {
                ++quantidadeDePares;
                arestas.push(make_pair(sentAnterior,grafo[*it].front()));
            }
            //arquivoNet.write("\n");
            anteriorCond=contar;
           // anterior=id;
            string zera;
            sentAnterior=zera;
            sentAnterior=sentAnterior+grafo[*it].front();
           if (!grafo[*it].empty())
                grafo[*it].pop();
            primeiro=false;
            ++contar;
        }
    }

    arquivoNet.write("source target\n");
    while(!arestas.empty()){
        //pair<string,string> front = arestas.front();
        arquivoNet.write((arestas.front().first).c_str());
        arquivoNet.write(" ");
        arquivoNet.write((arestas.front().second).c_str());
        arquivoNet.write("\n");
        //cout<<arestas.front().first<<" "<<arestas.front().second<<endl;
        arestas.pop();
    }


    arquivoNet.close();



    return true;

}

bool trataTexto::eFimSentenca(QString classe)
{
    return this->pontuacao.count(classe.toStdString()) == 1;
}

void trataTexto::frequenciaPalavra(QString nomeArq)
{
    map<string,int>::iterator it;
    ofstream  arquivo;
    arquivo.open(nomeArq.toStdString().c_str(), ios::out);

    arquivo <<"Palavras" <<"Frequencia" <<endl;

    for (it=this->canonica.begin();it!=this->canonica.end();it++)
    {
        arquivo <<(*it).first <<"\t" <<(*it).second <<endl;

    }
    arquivo.close();
}





bool trataTexto:: redeFrases(string infArq){
    string token;//Não foi utilizado para nada
    vector <string> sentPalNormal, sentPalCanonica;//guarda as palavras da sentença
    string nomeArquivo = infArq + ".tag";
    QFile arquivo(nomeArquivo.c_str());
    arquivo.open(QIODevice::ReadOnly);
    QTextStream in(&arquivo);
    in.setCodec("UTF-8");
    //in.setCodec("LATIN");


    map<string,queue<string> > grafo;//Armazena em cada substantivo a sua fila de sentencas
    queue<string> substantivos;
    set <string> subConj;


    this->normal.clear();
    this->canonica.clear();
    this->normalArestas.clear();
    this->canonicaArestas.clear();
    this->redeSentencasImpressaoCanonica.clear();
    this->redeSentencasImpressaoNormal.clear();

    int quantidadeDeVertices = 0;
    while(!in.atEnd())
    {
        sentPalNormal.clear();
        sentPalCanonica.clear();

        string sentenca;
        int quantidadeDeSubstantivos = 0;
        bool tem_substantivo = false; // cond
        bool terminou = false;


        while(!in.atEnd())
        {
            QString linha = in.readLine(1000);
            if(linha.isEmpty())
                continue;

            QStringList tokens = linha.split(" ");
            if (tokens.size() < 4)
                return false;

           if(filtro(tokens))           // desconsidera as palavras e formas canonicas preselecionadas.
                continue;

            string tag = tokens[2].toStdString();
            string palavra = tokens[0].toLower().toStdString();
            string canonica = tokens[1].toLower().toStdString();
            if(tag[0]!='F' && !filtro(tokens)){
                if(sentenca.size()>0){
                    sentenca = sentenca + "_" + canonica;
                    cout<<sentenca<<" "<<tag<<endl;
                }else
                    sentenca=sentenca+canonica;

            }
            if (eFimSentenca(tokens[2]) || tag[0] == 'F' )
            {
                terminou = true;

                if (tem_substantivo)
                {
                    quantidadeDeVertices = quantidadeDeVertices+1;
                    for (int r = 0; r < quantidadeDeSubstantivos; ++r)
                    {
                        grafo[substantivos.front()].push(sentenca);
                        substantivos.pop();
                    }
                }
                break;

            }
            else
            {
                if (tag[0]=='N'){
                    tem_substantivo=true;
                    ++quantidadeDeSubstantivos;
                    //aux.push(primeiraPalavraDaLinha);
                    subConj.insert(canonica);
                    substantivos.push(canonica);

                }
            }
        }
    }

    arquivo.close();

    nomeArquivo = infArq + ".redeFras.csv";
    QFile arquivoNet(nomeArquivo.c_str());

    if (!arquivoNet.open(QIODevice::WriteOnly))
        cerr << "erro ao abrir!\n";




    int contar=0,anteriorCond=0;
    hash<string> calcula_hash;

    //QString qlinha = "*vertices " + QString::number(quantidadeDeVertices) + "\n";
   // arquivoNet.write(qlinha.toUtf8());

    int quantidadeDePares=0;
    //queue<pair<size_t,size_t> > arestas;	//Armazena a lista com os pares de arestas para liga-los no grafo
    queue<pair<string,string> > arestas;
    for(set<string>::iterator it= subConj.begin();it!=subConj.end();it++){
        ++contar;
        bool primeiro = true;
        //size_t anterior = 0;
        string sentAnterior;
        while (!grafo[*it].empty())
        {
            size_t id = calcula_hash(grafo[*it].front());
            //arquivoNet.write(QString::number(id).toUtf8());
            //arquivoNet.write(" \"");
            //arquivoNet.write(grafo[*it].front().c_str());
           // arquivoNet.write("\"");

            if(anteriorCond==contar-1 && !primeiro)
            {
                ++quantidadeDePares;
                arestas.push(make_pair(sentAnterior,grafo[*it].front()));
            }
            //arquivoNet.write("\n");
            anteriorCond=contar;
           // anterior=id;
            string zera;
            sentAnterior=zera;
            sentAnterior=sentAnterior+grafo[*it].front();
           if (!grafo[*it].empty())
                grafo[*it].pop();
            primeiro=false;
            ++contar;
        }
    }

    arquivoNet.write("source target\n");
    while(!arestas.empty()){
        //pair<string,string> front = arestas.front();
        arquivoNet.write((arestas.front().first).c_str());
        arquivoNet.write(" ");
        arquivoNet.write((arestas.front().second).c_str());
        arquivoNet.write("\n");
        //cout<<arestas.front().first<<" "<<arestas.front().second<<endl;
        arestas.pop();
    }


    arquivoNet.close();



    return true;


}






/*
 *
void trataTexto::imprimeREdeSentenca(string nomeArq){

        QFile arqSaidaCan(QString(nomeArq.c_str()) + "sentCan.net");//define os nomes dos arquivos de saida e sua terminação
        QFile arqSaidaNormal(QString(nomeArq.c_str()) + "sentNormal.net");
        int i=0;
        //da permição de escrita para os arquivos criados
        arqSaidaCan.open(QIODevice::WriteOnly);
        arqSaidaNormal.open(QIODevice::WriteOnly);

        //Escreve a quantidade de vertices obtida através do tamanho da variavel canonica e normal
        QString qlinha="*vertices " + QString::number(this->redeSentencasImpressaoCanonica.size())+ "\n";
        arqSaidaCan.write(qlinha.toStdString().c_str());
        qlinha="*vertices " + QString::number(this->redeSentencasImpressaoNormal.size())+ "\n";
        arqSaidaNormal.write(qlinha.toStdString().c_str());
        //aqui ele esta imprimindo o numero da palavra e a palavra
        i = 1;
        for (this->it = this->redeSentencasImpressaoCanonica.begin(); this->it != this->redeSentencasImpressaoCanonica.end(); this->it++)
        {
            qlinha = QString::number(i)+" \""+QString(this->it->first.c_str())+" \"\n";
            qlinha=qlinha.toLower();
            qlinha=qlinha.toLocal8Bit();
            arqSaidaCan.write(qlinha.toStdString().c_str());
            this->it->second = i++; //Neste ponto aqui ele atribui a canonica.second o valor de i++
        }
        i = 1;

        for (this->it = this->redeSentencasImpressaoNormal.begin(); this->it != this->redeSentencasImpressaoNormal.end(); this->it++)
        {
            qlinha = QString::number(i)+" \""+QString(this->it->first.c_str())+" \"\n";
            qlinha=qlinha.toLower();
            qlinha=qlinha.toLocal8Bit();
            arqSaidaNormal.write(qlinha.toStdString().c_str());
            this->it->second = i++; //atribui a normal.second o valor de i++
        }

        //imprime "*edges" nos dois arquivos para simbolizar que a partir dali começa a parte das arestas
        arqSaidaCan.write("*edges\n");
        arqSaidaNormal.write("*edges\n");

        //laço responsavel por imprimir as arestas entre os vertices e seus pesos
        for (this->it = this->canonicaArestas.begin(); this->it != this->canonicaArestas.end(); this->it++)
        {
            QString palavra(this->it->first.c_str()); //atribui a variavel palavra o valor de canonicaArestas.first que são pares de palavras em redeSentenças e individuais em rede de Palavras
            QStringList lPalavra = palavra.split(" ");//Caso sejam pares de palavras ele divide ela quando achar um espaço
            QString pal1=QString::number(this->redeSentencasImpressaoCanonica[palavra]);//pega o indice(rotulo do vertice) da primeira palavra
            QString pal2=QString::number(this->redeSentencasImpressaoCanonica[palavra]);//pega o indice(rotulo do vertice) da segunda palavra
            QString qlinha = pal1 + " " + pal2 + " " +
                             QString::number(this->it->second)+ "\n";
            arqSaidaCan.write(qlinha.toStdString().c_str());//imprime no arquivo o formato "tagVetice1 tagVertice2 Peso"
        }
    //Neste laço o mesmo processo é refeito, no entanto com as palavras normais
        for (this->it = this->normalArestas.begin(); this->it != this->normalArestas.end(); this->it++)
        {
            QString palavra(this->it->first.c_str());
            QStringList lPalavra = palavra.split(" ");
            QString pal1=QString::number(this->normal[lPalavra[0].toStdString()]);
            QString pal2=QString::number(this->normal[lPalavra[1].toStdString()]);
            QString qlinha = pal1 + " " + pal2 + " " +
                             QString::number(this->it->second)+ "\n";
            arqSaidaNormal.write(qlinha.toStdString().c_str());
        }
        arqSaidaCan.close();
        arqSaidaNormal.close();

}
*/
