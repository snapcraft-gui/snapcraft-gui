#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>
#include <QDesktopServices>

#include <QNetworkRequest>

#include <QSplitter>
#include <QSettings>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



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

    snapcraft=new QProcess(0);
    done_message ="<br><span style='color:green'>Done.</span><br>";

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
}
//hide current snap options-------------------------------

//hide current snap options-------------------------------
void MainWindow::show_current_snap_options(){
    ui->current_snap->show();
    ui->close_current->show();
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
        qDebug()<<info.size();
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
        //set current snap name
        if(napname.at(0).contains("name:")){
        snapname = napname.at(0);
        snapname = snapname.split("#").at(0);
        snapname = snapname.remove("name:");
        ui->current_snap->setText("current : <b>"+snapname+"</b>");
        }
        else{
            //terminal dump (snap name not set)
            ui->terminal->append("<br>Please Specify name of snap in line: <b>1</b>");
        }

        //save the initial content into a string to compare later in yaml text xhanged slot to change state of save btn
        snapcraft_yaml = ui->yaml->toPlainText();
}
        }


//load snapcraft.yaml to interface-------------------------------



void MainWindow::on_open_snap_clicked()
{
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

      //run tree command
      show_tree();

  }
  else{ //pop error message
       QMessageBox::warning(this, tr("Snapcraft"),
                                  tr("Unable to load snapcraft file.\n"),
                                  QMessageBox::Ok);
       hide_current_snap_options();
  }


}

void MainWindow::show_tree(){ //create tree

    QString prog = "tree";
    QStringList args;
    QFile f(fileName);
    QString path = QFileInfo(f).filePath().remove("snapcraft.yaml");
    args<<"-L"<<"3"<<path;  // to depth of 3
    QProcess *tree = new QProcess(this);
    tree->start(prog, args);
  //  tree->waitForFinished();

  QString tree_output;//=  tree->readAll();
  qDebug()<<tree->readAll();
  QString tree_error = tree->readAllStandardOutput();
  //check tree is installed on system
  if(tree_error.contains("installed")){
   ui->terminal->append(tree_output+done_message);
  }
  else{
  ui->terminal->append("<span style='color:red'>Computing tree: </span>"+fileName+done_message);
  ui->tree->setText(tree_output);
  }




}

void MainWindow::on_new_snap_clicked()
{
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
    //close current snap
    close_session();
    //show session options
    show_session_options();
    //hide current_snap_options
    hide_current_snap_options();

    snapcraft_yaml.clear();
    ui->save_snapcraft->setDisabled(true);

}

//close the session -------------------------------
void MainWindow::close_session(){

ui->snapcraft_path->clear();

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

//save first line to some string
//QString fst = firstline;

if(ui->yaml->toPlainText().length()>1&&ui->yaml->toPlainText().split(QRegExp("[\r\n]"),QString::SkipEmptyParts).at(0) !=firstline){
    ui->terminal->append("<b style='color:red'>Warning:</b> snapname changed <b style='color:green'>[Suggestion] :</b>Please Click Save before performing build");
  //  qDebug()<<ui->yaml->toPlainText().split("\n").at(0);

}
snapname = ui->yaml->toPlainText().split("\n").at(0);
snapname = snapname.split("#").at(0);
snapname = snapname.remove("name:");
ui->current_snap->setText("current : <b>"+snapname+"</b>");

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
        ui->terminal->setDisabled(true);

        ui->commands_frame->setDisabled(true);

        ui->open_with_files->setDisabled(true);

    }
    else{
        ui->yaml->setDisabled(false);

        ui->tree->setDisabled(false);
        ui->tree_now->setDisabled(false);

        ui->terminal->setDisabled(false);

        ui->commands_frame->setDisabled(false);

        ui->open_with_files->setDisabled(false);


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
void MainWindow::on_highlight_clicked()
{
   QString damn =  QUrl::toPercentEncoding(ui->yaml->toPlainText());
   QNetworkRequest request(QUrl("http://markup.su/api/highlighter?language=YAML&theme=SpaceCadet&source="+damn));
   reply =m_network_manager.get(request);
   connect(this->reply,SIGNAL(finished()),this,SLOT(request_done()));

}
//loaded highlight data
void MainWindow::request_done(){
  if(this->reply->error() == QNetworkReply::NoError){
   QByteArray ans= reply->readAll();
   QString s_data = QTextCodec::codecForMib(106)->toUnicode(ans);  //106 is textcode for UTF-8 here --- http://www.iana.org/assignments/character-sets/character-sets.xml
   ui->yaml->setHtml(s_data.replace("background:#0d0d0d;","background:transparent;font-family: Ubuntu;font-size: 15px;"));
   ui->terminal->append("<span style='color:red'>Editor: </span>Set highlight mode."+done_message);

   }

   else if(this->reply->error()== QNetworkReply::OperationCanceledError){
       QMessageBox::information(0, QObject::tr("Error !"),
                                QObject::tr("Cancelled by User."));
       ui->terminal->append("canceled by user.");
   }
   else if(this->reply->error()==QNetworkReply::NetworkSessionFailedError )
   {
      QMessageBox::critical(0, QObject::tr("Error !"),
                                QObject::tr("Please try again , you need a working internet connection to highlight file YAML file"
                                            "."));
   }
   else{
       QMessageBox::critical(this, QObject::tr("Error !"),
                                tr("Network Error !<br><br><i>Currently Snapcraft-gui uses online service to highlight yaml.</i>"));
       ui->terminal->append("<span style='color:red'>Network Error:</span><i>Currently Snapcraft-gui uses online service to highlight yaml.</i>");
   }
}
//end get highlight data read iit-----------------------------

//togle back to normal view (no syntax high_lightning)
void MainWindow::on_normal_clicked()
{
    ui->yaml->setText(ui->yaml->toPlainText());
    ui->terminal->append("<span style='color:red'>Editor: </span>Set normal mode."+done_message);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("split1_state", split1->saveState());
    settings.setValue("outputdock_state", ui->dockWidget_2->saveGeometry());
    QMainWindow::closeEvent(event);
    qDebug()<<"closing";
}




