#include "install_local_snap_dialog.h"
#include "ui_install_local_snap_dialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

Install_local_snap_dialog::Install_local_snap_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Install_local_snap_dialog)
{
    ui->setupUi(this);
    install=new QProcess(this);
    list = new QProcess(this);

    connect(this->install,SIGNAL(readyRead()),this,SLOT(install_ready_read()));
    connect(install,SIGNAL(finished(int)),this,SLOT(install_process_finished(int )));
    ui->install_button->setDisabled(true);
    ui->remove_snap->setDisabled(true);


    list_installed_snaps();
}

void Install_local_snap_dialog::install_ready_read(){
    ui->terminal_output->append("<span style='color:red'>Snap Package: </span>"+snap_path+" will install shortly...<br>");
    QString output = install->readAll().replace("[K","ok");
    ui->terminal_output->setText(output);

}

void Install_local_snap_dialog::install_process_finished(int i){
    if(i==0){
    ui->terminal_output->append("<span style='color:green'>"+process_name+"</span><br>");
   if(process_name=="_Installed_"){
    installed_snap_name = ui->terminal_output->toPlainText().split("\n").at(0);
    installed_snap_name =installed_snap_name.split(" ").at(0);
    ui->terminal_output->append("installed_snap_name is "+installed_snap_name+"<br>");
   }
}
else{//when process exits with status code 1
        ui->terminal_output->append("Something went wrong, maybe you are trying to remove a package that is not installed.<br>");
    }

    if(ui->terminal_output->toPlainText().contains("_Installed_")){
       ui->remove_snap->setDisabled(false);
       ui->install_button->setDisabled(true);
   }
    else if(ui->terminal_output->toPlainText().contains("_Removed_"))
    {
        ui->remove_snap->setDisabled(true);
        ui->install_button->setDisabled(false);
    }
        list_installed_snaps();
}



Install_local_snap_dialog::~Install_local_snap_dialog()
{
    delete ui;
    delete install;
}

void Install_local_snap_dialog::on_choose_snap_clicked()
{
snap_path = QFileDialog::getOpenFileName(this,
           tr("Open .snap"), "", tr("Snap Packages (*.snap)"));
       if(snap_path.length()>0){

           ui->snap_path->setText(snap_path);
           install_command = "install";
           ui->install_button->setDisabled(false);
           ui->terminal_output->append("<span style='color:red'>Snap Package Manager: </span>Selected Package - "+snap_path+"<br>");
          }
       else{ //pop error message
            QMessageBox::warning(this, tr("Snap Package Installer"),
                                       tr("Unable to load .snap file.\n"),
                                       QMessageBox::Ok);
       }
}

void Install_local_snap_dialog::on_install_button_clicked()
{
    process_name = "_Installed_";
    if(devmode){
    QString prog = "pkexec";
    QStringList args;
    args<<"snap"<<"install"<<snap_path <<"--devmode";
    install->start(prog, args);
    }
    else{
        QString prog = "pkexec";
        QStringList args;
        args<<"snap"<<"install"<<snap_path;
        install->start(prog, args);
    }
}

void Install_local_snap_dialog::on_close_dialog_clicked()
{
 this->close();
}

void Install_local_snap_dialog::on_devmode_toggled(bool checked)
{
 devmode = checked;
}

void Install_local_snap_dialog::on_remove_snap_clicked()
{
    process_name = "_Removed_";
    QString prog = "pkexec";
    QStringList args;
    args<<"snap"<<"remove"<<installed_snap_name;
    install->start(prog, args);
}

void Install_local_snap_dialog::list_installed_snaps(){
    QString prog = "snap";
    QStringList args;
    args<<"list";
    list->start(prog, args);
    list->waitForFinished();
    ui->list->setText(list->readAll());

    ui->terminal_output->append("<span style='color:red'>Snap Package Manager: </span>listed installed Snaps.<br>");
}
