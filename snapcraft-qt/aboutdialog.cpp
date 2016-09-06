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


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
            this->setStyleSheet(QString("background-color:#302F2F;"));
ui->label->setStyleSheet("width:132px;height:132px;");
 }
AboutDialog::~AboutDialog()
{
    delete ui;
}




