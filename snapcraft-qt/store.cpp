#include "store.h"
#include "ui_store.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

store::store(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::store)
{
    ui->setupUi(this);
    ui->search->setDisabled(true);
    ui->install->setDisabled(true);
    search_process = new QProcess(this);
    install_remove_process = new QProcess(this);
    connect(this->search_process,SIGNAL(finished(int)),this,SLOT(search_finished(int)));
}

store::~store()
{
    delete ui;
}

void store::on_search_clicked()
{
    if(ui->available_packages->count()>0){
        ui->available_packages->clear();
    }
    search_process->start("snap",QStringList()<<"find"<<ui->search_term->text());
    ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>Searching store for "+ui->search_term->text()+"...");
    ui->search->setDisabled(true);
}
void store::search_finished(int j){
  QString s = search_process->readAll();
  QString e = search_process->readAllStandardOutput();
  QString o =search_process->readAllStandardError();
  if(j==0){
      //add item to list
      QFile file2("/tmp/store_packages.list");
      if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
          return;
      QTextStream out(&file2);
      out << s.remove("Name");
      file2.close(); //important

      QFile file("/tmp/store_packages.list");
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
           return;
       QTextStream in(&file);int i = 1;
       while (!in.atEnd()) {

          QString line = in.readLine();
          if(line.split(" ").at(0).length()>0){
          ui->available_packages->addItem(QString::number(i++)+" - "+line.split(" ").at(0));
          }
       }
       file.remove();
       ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>"+QString::number(ui->available_packages->count())+" found for "+ui->search_term->text()+".");
       ui->search->setDisabled(false);

              if(s.contains("not found")||e.contains("not found")||o.contains("not found")){
                  qDebug()<<"stdout  "+e<<"readall "+s<<"error "+o;
                  ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>No Snap found for the searched term.");
                 ui->search->setDisabled(false);
             }
             else if(s.contains("https:")||e.contains("https:")||o.contains("https:")){
                  qDebug()<<"stdout  "+e<<"readall "+s<<"errorline"+o;
                 ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>Cannot connect to store.");
                 ui->search->setDisabled(false);
             }

  }
else if(j==1){
      ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>no snaps found for "+ui->search_term->text()+".");
      ui->search->setDisabled(false);
      qDebug()<<"stdout  "+e<<"readall "+s<<"error "+o;

      if(o.contains("not found")){
          qDebug()<<"stdout  "+e<<"readall "+s<<"error "+o;
          ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>No Snap found for the searched term.");
         ui->search->setDisabled(false);
     }
     else if(s.contains("https:")||e.contains("https:")||o.contains("https:")){
          qDebug()<<"stdout  "+e<<"readall "+s<<"errorline"+o;
         ui->info_terminal->setText("<span style='color:red'>Snap Store: </span>Cannot connect to store.");
         ui->search->setDisabled(false);
     }
  }
}

void store::on_search_term_textChanged(const QString &arg1)
{
    if(arg1.length()>0){
        ui->search->setDisabled(false);
    }
    else{
        ui->search->setDisabled(true);
    }
}

void store::on_available_packages_currentTextChanged(const QString &currentText)
{
    if(currentText.length()>0){
        ui->install->setDisabled(false);
        //decide if snap is to be installed or it has to be removed
        //LOGIC - if <snap list package> will return empty string i.e, package not installed else package installed
        QProcess *process= new QProcess(this);
        process->start("snap",QStringList()<<"list"<<currentText.split("- ").at(1));
        process->waitForFinished();
        QString err= process->readAll();
        //TODO implement install and renove
        if(err.contains(" ")){
           // qDebug()<<err;
            ui->install->setText("Remove");
        }
        else{
            //qDebug()<<err;
            ui->install->setText("Install");
        }
    }
    else{
        ui->install->setDisabled(true);
    }
}

void store::on_install_clicked()
{
    if(ui->install->text().contains("Remove")){//remove snap
        qDebug()<<"removing";
    }
    else{//install snap
        qDebug()<<"installing";
    }
}
