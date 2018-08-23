# -------------------------------------------------
# Project created by QtCreator 2010-06-24T17:25:40
# -------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = palabras
TEMPLATE = app
SOURCES += main.cpp \
    guipalabras.cpp \
    contapalavras.cpp \
    tratatexto.cpp \
    dialogjanela.cpp \
    dialogcrece.cpp \
    config.cpp
HEADERS += guipalabras.h \
    contapalavras.h \
    tratatexto.h \
    dialogjanela.h \
    tratatexto.h \
    dialogcrece.h \
    config.h
FORMS += guipalabras.ui \
    dialogjanela.ui \
    dialogcrece.ui \
    config.ui

DISTFILES += \
    analyzer.py



