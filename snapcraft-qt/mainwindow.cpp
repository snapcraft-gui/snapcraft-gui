#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "highlighter.h"
#include "install_local_snap_dialog.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>
#include <QDesktopServices>

#include <QNetworkRequest>
#include <QCloseEvent>
#include <QSplitter>
#include <QSettings>
#include <QClipboard>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    on_actionSnapcraft_Plugins_Help_triggered();

    split1 = new QSplitter(0);

    split1->addWidget(ui->yaml);
    split1->addWidget(ui->tree);
    split1->setOrientation(Qt::Horizontal);

    ui->horizontalLayout_5->addWidget(split1);


    QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->dockWidget_2->restoreGeometry(settings.value("outputdock_state").toByteArray());
    split1->restoreState(settings.value("split1_state").toByteArray());

    setStyle(":/rc/style.qss");
    ui->clean_toolButton->setPopupMode(QToolButton::InstantPopup);

    //TODO check if snapcraft is installed

    //initiate interface
    hide_current_snap_options();
    on_yaml_textChanged();

    ui->snapcraft_path->setText("test");
    ui->snapcraft_path->clear();
    ui->terminal->setText("test");
    ui->terminal->clear();
    ui->highlight->hide();//temperory

    snapcraft=new QProcess(this);
    pastebin_it=new QProcess(this);
    done_message ="<br><span style='color:green'>Done.</span><br>";
    ui->save_snapcraft->setDisabled(true);

    connect(this->pastebin_it,SIGNAL(finished(int)),this,SLOT(pastebin_it_finished(int)));


    //some info in ui->yaml
    ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");//do not chnage this phrase it will breakfunctionality "Keshav Bhatt [keshavnrj@gmail.com]"
    //some info in ui->tree
    ui->tree->setText("Here you can see contents of loaded project directory where snapcraft.yaml is located in a tree-like format.<br>You can see size of Files and Directories, Directories are identified by '/' sign while Executable files are by '*' sign.<br><br>NOTE:<span style='color:grey;font-size:14px;'> Max display depth of the directory tree is 3 to generate tree faster !</span>");



}


//Loading qss and setting style from it--------------------------------
void MainWindow::setStyle(QString fname)
{
    QFile styleSheet(fname); // load CSS file
    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open file");
        return;
    }
    qApp->setStyleSheet(styleSheet.readAll()); // set own style
    styleSheet.close(); //close the file
}
//Loading qss and setting style from it-------------------------------



//hide current snap options-------------------------------
void MainWindow::hide_current_snap_options(){
    ui->current_snap->hide();
    ui->close_current->hide();

    ui->actionClose_Currrent->setDisabled(true);//since we having no project opened
}
//hide current snap options-------------------------------

//hide current snap options-------------------------------
void MainWindow::show_current_snap_options(){
    ui->current_snap->show();
    ui->close_current->show();

    ui->actionClose_Currrent->setDisabled(false);//since we having project opened
    ui->actionOpen->setDisabled(true);//since we have to close current before opening other
    ui->actionNew->setDisabled(true);//since we have to close current before creating new
}
//hide current snap options-------------------------------




//hide the session options-------------------------------
void MainWindow::hide_session_options(){
    ui->new_snap->hide();
    ui->open_snap->hide();
}
//hide the session options-------------------------------

//show the session options-------------------------------
void MainWindow::show_session_options(){
    ui->new_snap->show();
    ui->open_snap->show();
}
//show the session options-------------------------------


//load snapcraft.yaml to interface-------------------------------
void MainWindow::load_snapcraft_yaml(){
    QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        //to read the name of snap
        QStringList napname ;
        QFileInfo info(fileName);
        //qDebug()<<info.size();
        if(info.size()>0){

        while (!in.atEnd())
          {
            napname.append(in.readLine());
          }
        //add data to editor
        for(int i= 0; i<napname.size() ;i++){
        ui->yaml->append(napname.at(i));
        }

        firstline = napname.at(0); //save first line to watch snap name changes, to chnage name throughout the session

        ui->terminal->append("<span style='color:red'>Opening </span> <b>"+fileName+"</b>");

        //virtually click save button
      //  on_save_snapcraft_clicked();
        ui->save_snapcraft->setDisabled(true);

        //set current snap name
        if(napname.at(0).contains("name:")){
        snapname = napname.at(0);
        snapname = snapname.split("#").at(0);
        snapname = snapname.remove("name:");
        ui->current_snap->setText(" Current : <b>"+snapname+"</b>");
        }
        else{
            //terminal dump (snap name not set)
            ui->terminal->append("<br>Please Specify name of snap in line: <b>1</b>");
        }

        //save the initial content into a string to compare later in yaml text xhanged slot to change state of save btn
        snapcraft_yaml = ui->yaml->toPlainText();
     //   on_highlight_clicked();
        on_normal_clicked();
}
        }


//load snapcraft.yaml to interface-------------------------------



void MainWindow::on_open_snap_clicked()
{
  ui->yaml->clear();
  fileName = QFileDialog::getOpenFileName(this,
        tr("Open SnapCraft"), "", tr("Snapcraft Files (*.yaml)"));
  if(fileName.length()>13){ //verify we got file (atleat file name will be 13"snapcraft.yaml")
      //load file to interface
      load_snapcraft_yaml(); //loaded yaml
      //hide the session options
      hide_session_options();
      //show current snap options
      show_current_snap_options();

      //set snap name
      ui->snapcraft_path->setText(fileName);
      setWindowTitle("SnapCraft-Gui >> "+snapname + " @ "+fileName);

      //run tree command
      show_tree();
  }
  else{ //pop error message
       QMessageBox::warning(this, tr("Snapcraft"),
                                  tr("Unable to load snapcraft file.\n"),
                                  QMessageBox::Ok);
       //some info in ui->yaml
       ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");
       //some info in ui->tree
       ui->tree->setText("Here you can see contents of loaded project directory where snapcraft.yaml is located in a tree-like format.<br>You can see size of Files and Directories, Directories are identified by '/' sign while Executable files are by '*' sign.<br><br>NOTE:<span style='color:grey;font-size:14px;'> Max display depth of the directory tree is 3 to generate tree faster !</span>");

       hide_current_snap_options();
  }


}

void MainWindow::show_tree(){ //create tree

    bool tree_present = QFileInfo("/usr/bin/tree").exists();
    if(tree_present||QFileInfo("/usr/local/bin/tree").exists()){

    QString prog = "tree";
    QStringList args;
    QFile f(fileName);
    QString path = QFileInfo(f).filePath().remove("snapcraft.yaml");
    args<<"-Q"<<"-h"<<"-F"<<"--du"<<"-L"<<"3"<<path;  // to depth of 3
    QProcess *tree = new QProcess(this);
    tree->start(prog, args);
    ui->terminal->append("<span style='color:red'>Computing tree: </span>"+fileName);
    tree->waitForFinished();
    ui->terminal->append("<span style='color:green'>Done.</span><br>");
    ui->tree->setText(tree->readAll());
    }
    else {
     ui->terminal->append("<span style='color:red'>Tree: </span>tree not installed.<br>");
     ui->tree->setText("<span style='color:red'>Tree: </span>tree not installed.<br><br>We assume tree binary is installed in one of these locations:<br><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;/usr/bin/tree<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;/usr/local/bin/tree");
  }





}

void MainWindow::on_new_snap_clicked()
{
    ui->yaml->clear();
    //get dir path from qfiledialog
    fileName = QFileDialog::getExistingDirectory(this,
          tr("Select a Directory to init SnapCraft"),"" , QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(fileName.length()>1){//verify we got directory

        fileName=fileName+"/snapcraft.yaml";

   // run prog
    QString prog = "snapcraft";
    QStringList args;
    args<<"init";
    snapcraft->setWorkingDirectory(fileName.remove("/snapcraft.yaml"));

    snapcraft->start(prog,args);
    ui->terminal->append("\nInitializing snapcraft in <b>"+fileName.remove("/snapcraft.yaml")+"</b><br>");
    snapcraft->waitForFinished();
    ui->terminal->append(snapcraft->readAll());

    fileName=fileName+"/snapcraft.yaml";

    //load file to interface
    load_snapcraft_yaml();
    //hide the session options
    hide_session_options();
    //show current snap options
    show_current_snap_options();

    //set snap name
    ui->snapcraft_path->setText(fileName);

    //open snapcraft.yaml from initialized dir
    show_tree();
    }
    else{//if directory is empty or user cancelled init
        QMessageBox::warning(this, tr("Snapcraft"),
                                   tr("Unable to init snapcraft !\n\n"
                                    "Directory invalid."),
                                   QMessageBox::Ok);
        hide_current_snap_options();
    }

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_close_current_clicked()
{
    //check if chnages in editor are to be saved first
    if(ui->save_snapcraft->isEnabled()){  //changes are made by user
        QMessageBox msgBox;
        msgBox.setText("Snapcraft.yaml has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        ret = msgBox.exec(); //return code

        switch (ret) {
          case QMessageBox::Save:
            // Save was clicked
            on_save_snapcraft_clicked();
            //close current snap
            close_session();
            //show session options
            show_session_options();
            //hide current_snap_options
            hide_current_snap_options();
            snapcraft_yaml.clear();
            ui->save_snapcraft->setDisabled(true);
              break;
          case QMessageBox::Discard:
              // Don't Save was clicked
            //close current snap
            close_session();
            //show session options
            show_session_options();
            //hide current_snap_options
            hide_current_snap_options();
            snapcraft_yaml.clear();
            ui->save_snapcraft->setDisabled(true);
              break;
          case QMessageBox::Cancel:
              // Cancel was clicked
            ret=2334123;//setting my own code to evoke cancel event
            //do nothing
              break;
          default:
              // should never be reached
              break;
        }
    }
    else{
        //close current snap
        close_session();
        //show session options
        show_session_options();
        //hide current_snap_options
        hide_current_snap_options();
        snapcraft_yaml.clear();
        ui->save_snapcraft->setDisabled(true);
    }

}

//close the session -------------------------------
void MainWindow::close_session(){

this->setWindowTitle("SnapCraft-Gui");
ui->snapcraft_path->clear();
ui->actionOpen->setDisabled(false); //to allow user open new snapcraft.yaml
ui->actionNew->setDisabled(false); //to allow user create new snapcraft.yaml

//some info in ui->yaml
ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");
//some info in ui->tree
ui->tree->setText("Here you can see contents of loaded project directory where snapcraft.yaml is located in a tree-like format.<br>You can see size of Files and Directories, Directories are identified by '/' sign while Executable files are by '*' sign.<br><br>NOTE:<span style='color:grey;font-size:14px;'> Max display depth of the directory tree is 3 to generate tree faster !</span>");

}
//close the session -------------------------------


//snapcraft text change events
void MainWindow::on_yaml_textChanged()
{
if(snapcraft_yaml == ui->yaml->toPlainText()){
    ui->save_snapcraft->setDisabled(true);
}
else{
    ui->save_snapcraft->setDisabled(false);
}

if(ui->yaml->toPlainText().contains("Keshav Bhatt [keshavnrj@gmail.com]")){//do not chnage "Keshav Bhatt [keshavnrj@gmail.com]"
    ui->save_snapcraft->setDisabled(true);
}

//save first line to some string
//QString fst = firstline;

if(ui->yaml->toPlainText().length()>1&&ui->yaml->toPlainText().split(QRegExp("[\r\n]"),QString::SkipEmptyParts).at(0) !=firstline){
    ui->terminal->append("<b style='color:red'>Project warning:</b> snapname changed <b style='color:green'>[Suggestion] :</b>Please Click Save before performing build.<br>");
  //  qDebug()<<ui->yaml->toPlainText().split("\n").at(0);

}
snapname = ui->yaml->toPlainText().split("\n").at(0);
snapname = snapname.split("#").at(0);
snapname = snapname.remove("name:");
ui->current_snap->setText(" Current : <b>"+snapname+"</b>");

}

void MainWindow::on_save_snapcraft_clicked()
{
     //save snapcraft.yaml
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << ui->yaml->toPlainText();

    //save the initial content into a string to compare later in yaml text xhanged slot to change state of save btn
    snapcraft_yaml = ui->yaml->toPlainText();

    ui->terminal->append("<span style='color:red'>Editor: </span>Saved file"+fileName+done_message);

    //disable after save
    ui->save_snapcraft->setDisabled(true);

}


// enable disable widgets if snapcraft.yaml is open or closed
void MainWindow::on_snapcraft_path_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()){

        ui->yaml->clear();
        ui->yaml->setDisabled(true);

        ui->tree->clear();
        ui->tree->setDisabled(true);
        ui->tree_now->setDisabled(true);

        ui->terminal->clear();

        ui->commands_frame->setDisabled(true);

        ui->open_with_files->setDisabled(true);

        ui->pastebin_it->setDisabled(true);

        ui->open_with_gedit->setDisabled(true);

    }
    else{
        ui->yaml->setDisabled(false);

        ui->tree->setDisabled(false);
        ui->tree_now->setDisabled(false);

        ui->commands_frame->setDisabled(false);

        ui->open_with_files->setDisabled(false);

        ui->pastebin_it->setDisabled(false);

        ui->open_with_gedit->setDisabled(false);


    }
}

void MainWindow::on_terminal_textChanged()
{
    if(ui->terminal->toPlainText().length()<1){
        ui->clear_term->setDisabled(true);
    }
    else{
        ui->clear_term->setDisabled(false);
    }

    //to remove uneeded snapname changed lines when file loads to editor's textchange events
    if(ui->terminal->toPlainText().contains("Opening ")){
        QString old_term_data = ui->terminal->toPlainText();
        QString new_term_data =old_term_data.split("Opening ").at(1);
        ui->terminal->setText("<br><span style='color:red'>Opening: </span>"+new_term_data+done_message);
    }
}

void MainWindow::on_clear_term_clicked()
{
    ui->terminal->clear();
}



void MainWindow::on_tree_now_clicked()
{
    show_tree();
}

//open snapcraft path-----------------------------------
void MainWindow::on_open_with_files_clicked()
{
QDesktopServices::openUrl(QUrl(ui->snapcraft_path->text().remove("/snapcraft.yaml") ));
ui->terminal->append("<span style='color:red'>File Manager: </span>Open snapcraft base dir with system's Filemanager."+done_message);
}
//open snapcraft path-----------------------------------

//get highlight data read iit-----------------------------
//void MainWindow::on_highlight_clicked()
//{
//     delete highlighter;

//   QString damn =  QUrl::toPercentEncoding(ui->yaml->toPlainText());
//   QNetworkRequest request(QUrl("http://markup.su/api/highlighter?language=YAML&theme=SpaceCadet&source="+damn));
//   reply =m_network_manager.get(request);
//   ui->terminal->append("<span style='color:red'>Editor : </span> highlighting wait...[].<br>");
//   connect(this->reply,SIGNAL(finished()),this,SLOT(request_done()));


//}
////loaded highlight data
//void MainWindow::request_done(){
//  if(this->reply->error() == QNetworkReply::NoError){
//   QByteArray ans= reply->readAll();
//   QString s_data = QTextCodec::codecForMib(106)->toUnicode(ans);  //106 is textcode for UTF-8 here --- http://www.iana.org/assignments/character-sets/character-sets.xml
//   ui->yaml->setHtml(s_data.replace("background:#0d0d0d;","background:transparent;font-family: Ubuntu;font-size: 15px;"));
//   ui->terminal->append("<span style='color:red'>Editor: </span>Set highlight mode.[process part]."+done_message);
//   ui->highlight->setChecked(true);
//   ui->normal->setChecked(false);
//   }

//   else if(this->reply->error()== QNetworkReply::OperationCanceledError){
//       QMessageBox::information(0, QObject::tr("Error !"),
//                                QObject::tr("Cancelled by User."));
//       ui->terminal->append("canceled by user.");
//       ui->highlight->setChecked(false);
//       ui->normal->setChecked(true);
//   }
//   else if(this->reply->error()==QNetworkReply::NetworkSessionFailedError )
//   {
//      QMessageBox::critical(0, QObject::tr("Error !"),
//                                QObject::tr("Please try again , you need a working internet connection to highlight file YAML file"
//                                            "."));
//      ui->highlight->setChecked(false);
//      ui->normal->setChecked(true);
//   }
//   else{
//       QMessageBox::critical(this, QObject::tr("Error !"),
//                                tr("Network Error !<br><br><i>Currently Snapcraft-gui uses online service to highlight yaml.</i>"));
//       ui->terminal->append("<span style='color:red'>Editor: </span><span style='color:skyblue'>Network Error:</span><i>Currently Snapcraft-gui uses online service to highlight yaml.</i><br>");
//  ui->highlight->setChecked(false);
//  ui->normal->setChecked(true);

//  }
//}
//end get highlight data read iit-----------------------------

//togle back to normal view (no syntax high_lightning)
void MainWindow::on_normal_clicked()
{
        ui->highlight->setChecked(false);
        ui->normal->setChecked(true);
        ui->terminal->append("<span style='color:red'>Editor: </span>Set normal mode."+done_message);

        highlighter = new Highlighter(ui->yaml->document());
}


void MainWindow::closeEvent(QCloseEvent *event)
{

    //check for project changes are pending to save
    on_close_current_clicked();

    if(ret==2334123){//my custom code to check cancel button event
        event->ignore();
    }
    else{

    QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("split1_state", split1->saveState());
    settings.setValue("outputdock_state", ui->dockWidget_2->saveGeometry());
    QMainWindow::closeEvent(event);
    qDebug()<<"closing";}
}



//file menu slots
void MainWindow::on_actionNew_triggered()
{
    on_new_snap_clicked();
}

void MainWindow::on_actionOpen_triggered()
{
    on_open_snap_clicked();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionQuit_triggered()
{
    //check for project changes are pending to save
    on_close_current_clicked();

    if( ret==2334123){
        //cancel
    }
    else{
    qApp->quit();
    }

}

void MainWindow::on_actionClose_Currrent_triggered()
{
    on_close_current_clicked();
}


//install snap from local file
void MainWindow::on_actionInstall_a_snap_triggered()
{
    //popup a Package manager
    Install_local_snap_dialog *install = new Install_local_snap_dialog(this);
    install->exec();
}



void MainWindow::on_actionList_plugins_triggered()
{
    QString prog = "snapcraft";
    QStringList args;
    args<<"list-plugins";
    QProcess *process = new QProcess(this);
    process->start(prog, args);
    process->waitForFinished();
    QString out = process->readAll();

    ui->terminal->clear();
    QStringList t =out.split(" ");
    for(int i=0;i<t.size();i++){
        if(QString(t.at(i)).length()>0){//skip empty lines
        ui->terminal->append(QString("<span style='color:skyblue'>"+t.at(i)+"</span>").remove("\n")); // remove \n to skip newline
        }
    }

}

void MainWindow::on_actionSnapcraft_Plugins_Help_triggered()
{
    QString prog = "snapcraft";
    QStringList args;
    args<<"help"<<"plugins";
    QProcess *process = new QProcess(this);
    process->start(prog, args);
    process->waitForFinished();
    QString out = process->readAll();

        ui->terminal->setText(out);


}

void MainWindow::on_actionWebsite_triggered()
{
        QDesktopServices::openUrl(QUrl("https://github.com/keshavbhatt/snapcraft-gui"));
}

void MainWindow::on_open_with_gedit_clicked()
{
    QDesktopServices::openUrl(QUrl(ui->snapcraft_path->text()));
}

void MainWindow::on_package_manager_clicked()
{
    ui->actionInstall_a_snap->trigger();
}

void MainWindow::on_pastebin_it_clicked()
{
    QString o = "cat "+ui->snapcraft_path->text()+"| pastebinit";
    pastebin_it->start("bash", QStringList()<<"-c"<< o);
    ui->pastebin_it->setText("Wait..");
    ui->pastebin_it->setDisabled(true);
}

void MainWindow::pastebin_it_finished(int k){

    if(k==0){
        QString url = pastebin_it->readAll();
        ui->terminal->append("<span style='color:red'>Editor: </span>"+url);
        ui->terminal->append("<span style='color:red'>Editor: </span>pastebin url copied to clipboard.");
        ui->pastebin_it->setText("Pastebin-it");
        ui->pastebin_it->setDisabled(false);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(url);
    }
    else if(k==1){
         ui->terminal->append("<span style='color:red'>"+pastebin_it->readAllStandardError()+"</span>");
         ui->pastebin_it->setText("Pastebin-it");
         ui->pastebin_it->setDisabled(false);
    }
}
