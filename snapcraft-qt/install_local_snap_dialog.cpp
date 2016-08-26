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
    ui->remove_selected_snap->setDisabled(true);
    ui->selected_snap_info->setDisabled(true);
    ui->property_grid->hide();


    list_installed_snaps();
}

void Install_local_snap_dialog::install_ready_read(){
    ui->terminal_output->append("<span style='color:red'>Snap Package: </span>"+snap_path+" will install shortly...<br>");
    QString output = install->readAll().replace("[K","");  //this is fucking weired char snapcraft throwing in output
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

    ui->installed_package->clear(); //clear list cause new data available now

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

    QFile file2("/tmp/l.list");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file2);
    QString s =list->readAll();
    out << s.remove("Name");
    file2.close(); //important

    QFile file("/tmp/l.list");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;
     QTextStream in(&file);int i = 1;
     while (!in.atEnd()) {

        QString line = in.readLine();
        if(line.split(" ").at(0).length()>0){
        ui->installed_package->addItem(QString::number(i++)+" - "+line.split(" ").at(0));
        }
     }
     file.remove();

    ui->terminal_output->append("<span style='color:red'>Snap Package Manager: </span>listed installed Snaps.<br>");
    ui->property_grid->hide(); //cause nothing is selected in the list at this time
}

//return string from listwidget
QString Install_local_snap_dialog::inputstring() {
QModelIndexList templatelist = ui->installed_package ->selectionModel()->selectedIndexes();
QString  stringlist;
foreach (const QModelIndex &index, templatelist) {
stringlist.append(index.data(Qt::DisplayRole).toString()/*.section(' ',0 ,0)*/); //get the lable part form the string
}

return stringlist;
}


//fuction to calculate dir size
quint64 dir_size(const QString & str)
{
    quint64 sizex = 0;
    QFileInfo str_info(str);
    if (str_info.isDir())
    {
        QDir dir(str);
        QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs |  QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.isDir())
            {
                    sizex += dir_size(fileInfo.absoluteFilePath());
            }
            else
                sizex += fileInfo.size();

        }
    }
    return sizex;
    //qDebug()<<sizex;

}


void Install_local_snap_dialog::on_installed_package_clicked(const QModelIndex &index)
{
    if(inputstring().length()>0){

        ui->remove_selected_snap->setDisabled(false);
        ui->selected_snap_info->setDisabled(false);
        ui->property_grid->show();
        ui->name->setText(inputstring().split("- ").at(1));
        ui->remove_selected_snap->setText("Remove- "+inputstring().split("- ").at(1));

//calculate size of installed package
       QString addin_path = "/snap/"+inputstring().split("- ").at(1);
       int cache_size = dir_size(addin_path);
       QString cache_unit;

       if(cache_size > 1024*1024*1024)
       {
           cache_size = cache_size/(1024*1024*1024);
           cache_unit = " GB";
       }
       if(cache_size > 1024*1024)
       {
           cache_size = cache_size/(1024*1024);
           cache_unit = " MB";
       }
       else if(cache_size > 1024)
       {
           cache_size = cache_size/(1024);
           cache_unit = " kB";
       }
       else
       {
           cache_size = cache_size;
           cache_unit = " B";
       }
       ui->size->setText(QString::number(cache_size) + cache_unit);

    }
}


void Install_local_snap_dialog::on_remove_selected_snap_clicked()
{
    process_name = "_Removed_";
    QString prog = "pkexec";
    QStringList args;
    args<<"snap"<<"remove"<<inputstring().split("- ").at(1);
    install->start(prog, args);

}
