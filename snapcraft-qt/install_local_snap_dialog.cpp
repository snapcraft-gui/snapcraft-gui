#include "install_local_snap_dialog.h"
#include "ui_install_local_snap_dialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include "store.h"

Install_local_snap_dialog::Install_local_snap_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Install_local_snap_dialog)
{
    ui->setupUi(this);

    install=new QProcess(this);
    list = new QProcess(this);
    list_updates = new QProcess(this);
    info_process =new QProcess(this);

    connect(this->install,SIGNAL(readyRead()),this,SLOT(install_ready_read()));
    connect(install,SIGNAL(finished(int)),this,SLOT(install_process_finished(int )));
    connect(list_updates,SIGNAL(finished(int)),this,SLOT(list_updates_ready(int)));
    connect(info_process,SIGNAL(finished(int)),this,SLOT(info_process_finished(int)));

    ui->install_button->setDisabled(true);
//    ui->remove_snap->setDisabled(true);
    ui->remove_selected_snap->setDisabled(true);
    ui->selected_snap_info->setDisabled(true);
    ui->property_grid->hide();
    ui->selected_snap_info->hide();

    //to set update manager at startup
    ui->update->setCheckable(true);
    ui->update->setChecked(true);
    ui->update->setChecked(false);
    ui->update->hide();

    //style
    QString style ="QPushButton{color: #000000; background-color: #B5B8B7; border-width: 1px; border-color: #302F2F; border-style: solid; padding-top: 3px; padding-bottom: 3px; padding-left: 3px; padding-right: 3px; border-radius: 2px; outline: none;}"
                                        "QPushButton:disabled { background-color: #302F2F; border-width: 1px; border-color: #302F2F; border-style: solid; padding-top: 3px; padding-bottom: 3px; padding-left: 5px; padding-right: 5px; /*border-radius: 2px;*/ color: #454545;}"
                                        "QPushButton:focus { background-color: #B5B8B7; color: #000000;}"
                                        "QPushButton:hover{border: 1px solid #302F2F;color: #000000;}"
                                        "QPushButton:pressed { background-color: #484846;color: silver;}";

    //styling command buttons
    ui->remove_selected_snap->setStyleSheet(style.toUtf8());
    ui->install_button->setStyleSheet(style.toUtf8());

    list_installed_snaps();
}

void Install_local_snap_dialog::install_ready_read(){
    ui->terminal_output->append("<span style='color:red'>Snap Package: </span>"+snap_path+" will install shortly...<br>");
    QString output = install->readAll().replace("[K","");  //this is fucking weired char snapcraft throwing in output
    ui->terminal_output->setText(output);

}

void Install_local_snap_dialog::install_process_finished(int i){
    if(i==0){ //successfull execution
    ui->terminal_output->append("<span style='color:green'>"+process_name+"</span><br>");
   if(process_name=="_Installed_"){
    installed_snap_name = ui->terminal_output->toPlainText().split("\n").at(0);
    installed_snap_name =installed_snap_name.split(" ").at(0);
    ui->terminal_output->append("installed_snap_name is "+installed_snap_name+"<br>");
   }
}
else{//when process exits with status code 1 error while execution
        ui->terminal_output->append("Something went wrong, maybe you are trying to remove a package that is not installed.<br>");
    }

    if(ui->terminal_output->toPlainText().contains("_Installed_")){
       ui->install_button->setDisabled(true);
       ui->snap_path->clear();
   }
    else if(ui->terminal_output->toPlainText().contains("_Removed_"))
    {
//        ui->remove_snap->setDisabled(true);
        if(!ui->snap_path->text().isEmpty()){
        ui->install_button->setDisabled(false);
        }
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
    ui->remove_selected_snap->setDisabled(true);
    ui->selected_snap_info->setDisabled(true);
}

//return string from listwidget when user select a listitem
QString Install_local_snap_dialog::inputstring() {
QModelIndexList templatelist = ui->installed_package ->selectionModel()->selectedIndexes();
QString  stringlist;
foreach (const QModelIndex &index, templatelist) {
stringlist.append(index.data(Qt::DisplayRole).toString()/*.section(' ',0 ,0)*/); //get the lable part form the string
}
return stringlist;
}
//return string from listwidget when user select a listitem
QString Install_local_snap_dialog::inputstring_from_updatable() {
QModelIndexList templatelist = ui->updatable ->selectionModel()->selectedIndexes();
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
}

//on click on list item in installed snap list
void Install_local_snap_dialog::on_installed_package_clicked(const QModelIndex &index)
{ if(index.isValid()){
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

    //show snap info in mini info terminal
    on_selected_snap_info_clicked();
}


void Install_local_snap_dialog::on_remove_selected_snap_clicked()
{
    process_name = "_Removed_";
    QString prog = "pkexec";
    QStringList args;
    args<<"snap"<<"remove"<<inputstring().split("- ").at(1);
    install->start(prog, args);

}


//multipurpose function to sets info in mini terminal for selected installed/updatable snap
void Install_local_snap_dialog::on_selected_snap_info_clicked()
{
    QString prog = "snap";
    QStringList args;
    //decide if the incoming string is from updatable list widget or from installed list widget
    if(ui->update_frame->isEnabled()){
         args<<"refresh"<<"--list"<<inputstring_from_updatable().split("- ").at(1);
    }else{
         args<<"list"<<inputstring().split("- ").at(1);
    }

//    QProcess *process = new QProcess(this);
    ui->info_terminal->setText("<span style='color:red'>Info: </span>Please wait getting info...");
    info_process->start(prog, args);
    //info_process->waitForFinished();

}
//info process finised
void Install_local_snap_dialog::info_process_finished(int k){
    if(k==0){
    QString out =info_process->readAll();
    out.replace("Name","").replace("Version","").replace("Rev","").replace("Developer","").replace("Notes","");

    //add data to info terminal
    ui->info_terminal->setText("<span style='color:red'>Info: </span>"+out);
   // qDebug()<<ui->info_terminal->toPlainText();
    QString plain_text = ui->info_terminal->toPlainText().remove("Info: ");
            QStringList out_data_as_list(plain_text.split(" "));
           // qDebug()<<out_data_as_list;
            QString name , version ,revision,developer ,notes;
            QString color = "<span style='color:#3D8EC9'>";
            name=color+"name:</span>"+out_data_as_list.at(0)+" ";
            version=color+"ver:</span>"+out_data_as_list.at(1)+"</span>"+" ";
            revision=color+"rev:</span>"+out_data_as_list.at(2)+"</span>"+" ";
            developer=color+"dev:</span>"+out_data_as_list.at(3)+"</span>"+" ";
            notes=color+"note:</span>"+out_data_as_list.at(4)+"</span>"+" ";
            ui->info_terminal->setText("<span style='color:red'>Info: </span>"+name+version+revision+developer+notes);
            //set values in info frame
            ui->revision->setText(out_data_as_list.at(2));
            ui->version->setText(out_data_as_list.at(1));
    }
    else{
        ui->info_terminal->setText("<span style='color:red'>Info: </span>Something went wrong, Network connection required.");
    }

}


//UPDATE MANAGER

//show hide update_frame on update_manager button toggled
void Install_local_snap_dialog::on_update_toggled(bool checked)
{
    if(!checked){ //update manaher not enabled
        ui->updatable->clear();
        //ui->installed_package->clear();
        //list_installed_snaps();//populate list of installed snaps
        ui->install_remove_frame->setEnabled(true);
        ui->update_frame->setEnabled(false);
//        //set remove button enabled if snap from installed list is selected
//        if(ui->remove_snap->text().size()>6){
//        ui->remove_selected_snap->setEnabled(true);
//        }else{
//            ui->remove_selected_snap->setEnabled(false);
//        }
        ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span>switched install-remove mode.");
    }
    else{//update manager enabled

        //load last update time
        QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
        ui->last_checked->setText(settings.value("last_update_check").toString());
        ui->install_remove_frame->setEnabled(false);
        ui->update_frame->setEnabled(true);
        ui->remove_selected_snap->setEnabled(false);
        ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span>switched update-manager mode.");
        //inside update options
        ui->update_options->setEnabled(false);
    }
}

//check for update button slot
void Install_local_snap_dialog::on_check_for_update_clicked()
{
    ui->updatable->clear(); //empty list before geting new updates
    ui->update_options->setDisabled(true);
    ui->update_button->setText("Update");

    QString prog = "snap";
    QStringList args;
    args<<"refresh"<<"--list";

    ui->check_for_update->setDisabled(true);
    ui->check_for_update->setText("Please wait");
    ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span> getting updates, Please wait...");

    list_updates->start(prog, args);


}

void Install_local_snap_dialog::list_updates_ready(int j){


    ui->check_for_update->setDisabled(false);
    ui->check_for_update->setText("Check for Updates");

    QFile file2("/tmp/updates.list");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file2);
    QString s = list_updates->readAll();
    out << s.remove("Name");
    file2.close(); //important

    QFile file("/tmp/updates.list");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;
     QTextStream in(&file);int i = 1;
     while (!in.atEnd()) {

        QString line = in.readLine();
        if(line.split(" ").at(0).length()>0){
        ui->updatable->addItem(QString::number(i++)+" - "+line.split(" ").at(0));
        }
     }
     file.remove();
     if(ui->updatable->count()>0 && j==0){  // if command execution successfull and we got something in updates
         //set last update time and save it to qSettings
         ui->last_checked->setText(QDateTime::currentDateTime().toString());
         QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
         settings.setValue("last_update_check",ui->last_checked->text());//saved time
         ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span> Updates are ready to install.");
      }
     else if(j==1){//execiton error or network problem or anything
         ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span>Something went wrong, Network connection required.");
     }
     else{//if execution was okay but no updates found
         ui->last_checked->setText(QDateTime::currentDateTime().toString());
         ui->info_terminal->setText("<span style='color:red'>Snap Package Manager: </span> Updates not found. All Snaps are up to date.");
     }

}

//on items clicked
void Install_local_snap_dialog::on_updatable_clicked(const QModelIndex &index)
{
    if(index.isValid()){
    if(inputstring_from_updatable().length()>0){
        ui->update_options->setDisabled(false);
        ui->update_options->setEnabled(true);
        ui->update_button->setText("Update: "+inputstring_from_updatable().split("- ").at(1));
        //show snap info in mini info terminal
        on_selected_snap_info_clicked();
    }
    else{
        ui->update_options->setEnabled(false);
    }}
}

void Install_local_snap_dialog::on_tabWidget_currentChanged(int index)
{

    if(index==0){//install tab
        ui->update->setChecked(false);
    }else if(index==1){//update tab
        ui->update->setChecked(true);
    }

}
