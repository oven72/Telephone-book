#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QTranslator>

#include <QDir>
#include <QDateTime>
#include <QTextCodec>
#include <QByteArray>

QSqlDatabase * db;

void strtofile(QString fn, QString s, bool append = true, QByteArray codec = "UTF-8"){
    QTextCodec *cdc = QTextCodec::codecForName(codec);
    QByteArray dat = cdc->fromUnicode(s);

    QFile file(fn);
    if (append) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            return;
        }
    } else {
        if (!file.open(QFile::WriteOnly)) {
            return;
        }
    }
    file.write(dat);
    file.close();
}


void debuglog(QString txt, bool full){
    QString fl = "debug.log";
    if (!full){
        if (txt.length()>500) {
            txt = txt.left(240)+" ... "+txt.right(240);
        }
    }
    QString app = qAppName();
    strtofile(fl, QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss zzz")+" "+app+" "+txt+"\n");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QApplication::applicationDirPath()+"/translation.ru_RU.qm");
    a.installTranslator(&translator);

    //установим параметры формы из инишки
    QSettings *settings = new QSettings(QApplication::applicationDirPath()+"/tels.conf", QSettings::IniFormat);
    settings->beginGroup("db");
    QString db_name = settings->value("db_name", "").toString();
    QString db_connections = settings->value("DB_OPTIONS", "").toString();
    QString db_user_name = settings->value("db_user_name", "").toString();
    QString db_user_password = settings->value("db_user_password", "").toString();
    QString db_hostname = settings->value("db_hostname", "").toString();

    settings->endGroup();
    delete settings;


    db = new QSqlDatabase;
    *db = QSqlDatabase::addDatabase("QIBASE", "TEL");
    db->setConnectOptions(db_connections);
    db->setHostName(db_hostname);
    db->setDatabaseName(db_name);
    db->setUserName(db_user_name);
    db->setPassword(db_user_password);

    if (!db->open()){
        debuglog(db->lastError().text(), true);
        return 5;
    }


    MainWindow w;
    w.show();

    return a.exec();
}
