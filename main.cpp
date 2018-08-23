#include <QApplication>
#include "guipalabras.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    guiPalabras w;
    w.show();
    return a.exec();
}
