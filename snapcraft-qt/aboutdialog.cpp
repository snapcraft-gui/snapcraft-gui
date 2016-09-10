/*
*This file is part of Ultimate-Media-Downloader.
*
*Ultimate-Media-Downloader is not a free software: you can't redistribute it nor modify
*it without the author <keshavnrj@gmail.com> Keshav Bhatt permission.
*
*Ultimate-Media-Downloader is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/images/Snapcraft-gui.png"));
            this->setStyleSheet(QString("background-color:#302F2F;"));
            ui->label->setStyleSheet("width:132px;height:132px;");


            QFile file(":/txt/contrib.txt");
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                    return;
                QTextStream in(&file);
                QStringList strl ;
                while (!in.atEnd())
                  {
                    strl.append(in.readLine());
                  }


                ui->contrib_list->setColumnCount(3);
                ui->contrib_list->setRowCount(strl.count());
                QList<QString> headerLabels;
                headerLabels << "Name" << "Contribution" << "Contact";
                ui->contrib_list->setHorizontalHeaderLabels(headerLabels);


              for (int i = 0; i < strl.count(); ++i){
                  //name
                ui->contrib_list->setItem(i,0 , new QTableWidgetItem(QString(strl.at(i).split(">>").at(0))));
                  //contirb
                ui->contrib_list->setItem(i,1 , new QTableWidgetItem(QString(strl.at(i).split(">>").at(1))));
                  //email
                ui->contrib_list->setItem(i,2 , new QTableWidgetItem(QString(strl.at(i).split(">>").at(2))));
                }

 }
AboutDialog::~AboutDialog()
{
    delete ui;
}




