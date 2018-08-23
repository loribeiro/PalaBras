#ifndef TRATATEXTO_H
#define TRATATEXTO_H

#include <vector>
#include <QString>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QFile>
#include <cstdlib>
#include <QStringList>

using namespace std;

typedef struct
{
string palavra1;
string palavra2;//
string classe;
double prob;
} linha;


class trataTexto
{
public:
    trataTexto();
    int palTot, palDiffN, palDiffC;
    map <string, int> normal;
    map <string, int> canonica;
    map <string, int> vocabulario;
    map <string, int> normalArestas;
    map <string, int> canonicaArestas;
    map<string,int>::iterator it;
    map<string,int> pontuacao;
    map<string,int>redeSentencasImpressaoCanonica;
    map<string,int>redeSentencasImpressaoNormal;
    bool abreArquivo(string);
    bool abreArquivoTAG(string arquivo, QStringList tags);
    bool abreArquivoFRQ(string arquivo);
    bool abreArquivoCRE(string nome, QStringList tags,int passo,string arqCre);
    bool abreArquivoCREP(string nome, QStringList tags,int passo,string arqCre);
    bool abreArquivoCRES(string nome, QStringList tags,int passo,string arqCre);
    bool abreArquivoCRED(string nome, QStringList tags,int passo,string arqCre);
    void carregaFiltros();
    bool filtro(QStringList token);

    string GetStdoutFromCommand(string cmd);
    void imprimeREdeSentenca(string nomeArq);
    bool abreArquivoTAGFFR(string arquivo, QStringList tags, string arqFfr);
    void preanalyze(QString base, char idioma, char tipoAnalise);
    void estatisticaPalavras(QString);
    bool redePalavras (string infArq, int janela);
    void imprimeNet (string nomeArq);
    bool redeSentencas (string infArq);
    bool redeFrases (string infArq);
    bool eFimSentenca(QString);
    void lowerWord (QString nomeArqE, QString nomeArqS);
    int palFuncionalT;
    int palVocabularioT;
    int palContenidoT;
    double zipfpromedio;
    void frequenciaPalavra (QString nomeArq);
    void simpleSubs (QString nomeArqE, QString nomeArqS, QStringList regAlvo, QStringList regSubs);
    QStringList filtroGram,filtroPala;
};

#endif // TRATATEXTO_H
