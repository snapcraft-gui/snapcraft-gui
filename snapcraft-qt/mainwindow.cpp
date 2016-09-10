#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "highlighter.h"
#include "install_local_snap_dialog.h"
#include "ui_clean_dialog.h"
#include "store.h"
#include "aboutdialog.h"
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
#include <QDateTime>
#include <QStyle>
#include <QMenu>
#include <QTimer>
#include <QMimeData>

#include <QPainter>





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    split1 = new QSplitter(this);

    snapcraft=new QProcess(this);
    pastebin_it=new QProcess(this);

    clean_proc =new QProcess(this);
    snap=new QProcess(this);
    pull=new QProcess(this);
    stage=new QProcess(this);
    prime=new QProcess(this);
    build=new QProcess(this);
    parts_update= new QProcess(this);


    login = new QProcess(this);

    split1->addWidget(ui->yaml_2);
    split1->addWidget(ui->yaml);
    split1->addWidget(ui->tree);
    split1->setOrientation(Qt::Horizontal);
    split1->setChildrenCollapsible(false);

    ui->horizontalLayout_5->addWidget(split1);


    QSettings settings("com.keshavnrj.snapcraft-gui", "snapcraft-gui");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->dockWidget_2->restoreGeometry(settings.value("outputdock_state").toByteArray());
    split1->restoreState(settings.value("split1_state").toByteArray());

    setStyle(":/rc/style.qss");

    //TODO check if snapcraft is installed
     //added snapcraft as dependency in debian control

    //initiate interface
    hide_current_snap_options();
    on_yaml_textChanged();

    //trigger text change events on some widgets to init thr gui
    ui->snapcraft_path->setText("test");
    ui->snapcraft_path->clear();
    ui->terminal->setText("test");
    ui->terminal->clear();
    ui->highlight->hide();//temperory hide online highlighter button
    ui->zoom->setText(QString::number(ui->yaml->fontInfo().pixelSize()));//editor zoom lable


    done_message ="<br><span style='color:green'>Done.</span><br>";
    ui->save_snapcraft->setDisabled(true);

    connect(this->pastebin_it,SIGNAL(finished(int)),this,SLOT(pastebin_it_finished(int)));

    // commands connections
    connect(this->clean_proc,SIGNAL(readyRead()),this,SLOT(clean_proc_readyRead()));
    connect(this->clean_proc,SIGNAL(finished(int)),this,SLOT(clean_proc_finished(int)));
    connect(this->snap,SIGNAL(finished(int)),this,SLOT(snap_finished(int)));
    connect(this->snap,SIGNAL(readyRead()),this,SLOT(snap_readyRead()));
    connect(this->pull,SIGNAL(finished(int)),this,SLOT(pull_finished(int)));
    connect(this->pull,SIGNAL(readyRead()),this,SLOT(pull_readyRead()));
    connect(this->stage,SIGNAL(finished(int)),this,SLOT(stage_finished(int)));
    connect(this->stage,SIGNAL(readyRead()),this,SLOT(stage_readyRead()));
    connect(this->prime,SIGNAL(finished(int)),this,SLOT(prime_finished(int)));
    connect(this->prime,SIGNAL(readyRead()),this,SLOT(prime_readyRead()));
    connect(this->build,SIGNAL(finished(int)),this,SLOT(build_finished(int)));
    connect(this->build,SIGNAL(readyRead()),this,SLOT(build_readyRead()));
    connect(this->parts_update,SIGNAL(readyRead()),this,SLOT(add_part_update_output_to_terminal()));
    connect(this->parts_update,SIGNAL(finished(int)),this,SLOT(parts_update_finished(int)));

    connect(ui->yaml,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));
    connect(ui->yaml->document(), &QTextDocument::contentsChanged,this, &MainWindow::documentWasModified);

    ui->yaml->setContextMenuPolicy(Qt::CustomContextMenu);//to allow pop custom context menu in yaml editor
    //some info in ui->yaml
    ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");//do not chnage this phrase it will breakfunctionality "Keshav Bhatt [keshavnrj@gmail.com]"
    //some info in ui->tree
    ui->tree->setText("Here you can see contents of loaded project directory where snapcraft.yaml is located in a tree-like format.<br>You can see size of Files and Directories, Directories are identified by '/' sign while Executable files are by '*' sign.<br><br>NOTE:<span style='color:grey;font-size:14px;'> Max display depth of the directory tree is 3 to generate tree faster !</span>");


    //style
    QString style ="QPushButton{color: #000000; background-color: #B5B8B7; border-width: 1px; border-color: #302F2F; border-style: solid; padding-top: 3px; padding-bottom: 3px; padding-left: 3px; padding-right: 3px; border-radius: 2px; outline: none;}"
                                        "QPushButton:disabled { background-color: #302F2F; border-width: 1px; border-color: #302F2F; border-style: solid; padding-top: 3px; padding-bottom: 3px; padding-left: 5px; padding-right: 5px; /*border-radius: 2px;*/ color: #454545;}"
                                        "QPushButton:focus { background-color: #B5B8B7; color: #000000;}"
                                        "QPushButton:hover{border: 1px solid #302F2F;color: #000000;}"
                                        "QPushButton:pressed { background-color: #484846;color: silver;}";

    //styling command buttons
    ui->snap->setStyleSheet(style.toUtf8());
    ui->pull->setStyleSheet(style.toUtf8());
    ui->stage->setStyleSheet(style.toUtf8());
    ui->prime->setStyleSheet(style.toUtf8());
    ui->build->setStyleSheet(style.toUtf8());
    ui->clean_toolButton->setStyleSheet(style.toUtf8());
    ui->reload_file->setStyleSheet(style.toUtf8());
    ui->ignore_changes->setStyleSheet(style.toUtf8());


    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(12);
    font.setWeight(QFont::DemiBold);
    ui->yaml->setFont(font);

    const int tabStop = 4;  // 4 characters

    QFontMetrics metrics(font);
    ui->yaml->setTabStopWidth(tabStop * metrics.width(""));
    highlightCurrentLine();


    ui->file_changed_frame->hide();
    highlighter = new Highlighter(ui->yaml_2->document());
    ui->yaml_2->setFont(font);
    ui->yaml_2->setReadOnly(true);
    ui->yaml_2->hide();

    ui->tree->setMinimumWidth(100);
    ui->yaml->setMinimumWidth(100);
    ui->yaml_2->setMinimumWidth(100);
    split1->setCollapsible(2,false);
    split1->setCollapsible(1,false);
    split1->setCollapsible(0,false);

}



void MainWindow::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if ( !ui->yaml->isReadOnly()) {
        QTextEdit::ExtraSelection selection ;

        QColor lineColor = QColor(201, 191, 253, 15);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = ui->yaml->textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    ui->yaml->setExtraSelections(extraSelections);
}

void MainWindow::documentWasModified(){
    ui->save_snapcraft->setEnabled(true);
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
        //to file as stringlist to identify name of snap
        QStringList napname ;
        QFileInfo info(fileName);
        if(info.size()>0){//to check if file is not empty
        while (!in.atEnd())
          {
            napname.append(in.readLine());
          }
        //add data to editor from napname stringlist
        for(int i= 0; i<napname.size() ;i++){
        ui->yaml->append(napname.at(i));
        }
        ui->yaml->setText(ui->yaml->toPlainText().toUtf8());//finally set yaml data to editor

        last_saved_text =yaml_oneditor;//save yaml text as last_saved so that we can keep track on yaml file on disk , full function is below in code

        firstline = napname.at(0); //save first line to watch snap name changes, to chnage name throughout the session

        ui->terminal->append("<span style='color:red'>Opening </span> <b>"+fileName+"</b>");//some terminal info

        ui->save_snapcraft->setDisabled(true);//disable save since we just loaded file ,we are in open funtcion

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

        //save the initial content into a string to compare later in yaml text changed slot to change state of save btn , we doing it from two signals channels one our customm and other editor.document.modified()
        snapcraft_yaml.clear();
        snapcraft_yaml = ui->yaml->toPlainText().toUtf8();
//      on_highlight_clicked();
        on_normal_clicked();
}
}
// edn load snapcraft.yaml to interface-------------------------------

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
    if(tree_present||QFileInfo("/usr/local/bin/tree").exists()){//check if tree command available or not

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
//on editor font changed
void MainWindow::on_font_currentFontChanged(const QFont &f)
{
    ui->yaml->setFont(f) ;
    ui->yaml_2->setFont(f) ;
    //save font state TODO
}


void MainWindow::on_new_snap_clicked()
{
    ui->yaml->clear();
    //get dir path from qfiledialog
    fileName = QFileDialog::getExistingDirectory(this,
          tr("Select a Directory to init SnapCraft"),"" , QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(fileName.length()>1){//verify we got directory

   // run prog
    QString prog = "snapcraft";
    QStringList args;
    args<<"init";
    snapcraft->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));

    snapcraft->start(prog,args);
    ui->terminal->append("\nInitializing snapcraft in <b>"+QString(fileName).remove("/snapcraft.yaml")+"</b><br>");
    snapcraft->waitForFinished();
    ui->terminal->append(snapcraft->readAll());

    fileName=fileName+"/snapcraft.yaml";


    load_snapcraft_yaml();
    hide_session_options();
    show_current_snap_options();

    //set snapcraft file path
    ui->snapcraft_path->setText(fileName);

    //open snapcraft.yaml from initialized dir
    show_tree();
    }
    else{//if directory is empty or user cancelled init
        QMessageBox::warning(this, tr("Snapcraft"),
                                   tr("Unable to init snapcraft !\n\n"
                                    "Directory invalid."),
                                   QMessageBox::Ok);
        //some info in ui->yaml
        ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");
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
            on_save_snapcraft_clicked();
            close_session();
            show_session_options();
            hide_current_snap_options();
            snapcraft_yaml.clear();
            ui->save_snapcraft->setDisabled(true);
              break;
          case QMessageBox::Discard:
            close_session();
            show_session_options();
            hide_current_snap_options();
            snapcraft_yaml.clear();
            ui->save_snapcraft->setDisabled(true);
              break;
          case QMessageBox::Cancel:
            ret=2334123;//setting my own code to evoke cancel event
            //do nothing
              break;
          default:
              // should never be reached
              break;
        }
    }
    else{
        close_session();
        show_session_options();
        hide_current_snap_options();
        snapcraft_yaml.clear();
        ui->save_snapcraft->setDisabled(true);
    }
}

//close the session -------------------------------
void MainWindow::close_session(){
ui->yaml->document()->clearUndoRedoStacks();//clear() below can do that but fuck it we doing it with document
ui->yaml->clear();
ui->snapcraft_path->clear();
this->setWindowTitle("SnapCraft-Gui");
ui->actionOpen->setDisabled(false); //to allow user open new snapcraft.yaml
ui->actionNew->setDisabled(false); //to allow user create new snapcraft.yaml


//some info in ui->yaml
ui->yaml->setText("This is snapcraft.yaml editor with snapcraft's specific yaml syntax highlight support.<br> An online syntax highlighter backend is also integrated which support lots of themes.<br>Click New to create new Snapcraft project or click Open to load existing snapcraft project.<br><br>#This tool is Developed by - Keshav Bhatt [keshavnrj@gmail.com].");//do not chnage this phrase it will breakfunctionality "Keshav Bhatt [keshavnrj@gmail.com]"
//some info in ui->tree
ui->tree->setText("Here you can see contents of loaded project directory where snapcraft.yaml is located in a tree-like format.<br>You can see size of Files and Directories, Directories are identified by '/' sign while Executable files are by '*' sign.<br><br>NOTE:<span style='color:grey;font-size:14px;'> Max display depth of the directory tree is 3 to generate tree faster !</span>");

last_saved_text.clear();
yaml_ondisk.clear();
yaml_oneditor.clear();
this->setWindowState(Qt::WindowNoState);
this->setWindowState(Qt::WindowMaximized);
}
//close the session -------------------------------


//snapcraft text change events
void MainWindow::on_yaml_textChanged()
{
    ui->undo_btn->setEnabled(ui->yaml->document()->isUndoAvailable());
    ui->redo_btn->setEnabled(ui->yaml->document()->isRedoAvailable());

if(snapcraft_yaml == ui->yaml->toPlainText()){
    ui->save_snapcraft->setDisabled(true);
}
else if(snapcraft_yaml != ui->yaml->toPlainText()){
    ui->save_snapcraft->setDisabled(false);
}

if(ui->yaml->toPlainText().contains("Keshav Bhatt [keshavnrj@gmail.com]")){//do not chnage "Keshav Bhatt [keshavnrj@gmail.com]"
    ui->save_snapcraft->setDisabled(true);
}

//save first line to some string
//QString fst = firstline;

if(ui->yaml->toPlainText().length()>1&&ui->yaml->toPlainText().split(QRegExp("[\r\n]"),QString::SkipEmptyParts).at(0) !=firstline){
//    ui->terminal->append("<b style='color:red'>Project warning:</b> snapname changed <b style='color:green'>[Suggestion] :</b>Please Click Save before performing build.<br>");
  //  qDebug()<<ui->yaml->toPlainText().split("\n").at(0);

}
snapname = ui->yaml->toPlainText().split("\n").at(0);
snapname = snapname.split("#").at(0);
snapname = snapname.remove("name:");
ui->current_snap->setText(" Current : <b>"+snapname+"</b>");

//show doc stats
ui->doc_stats->setText("Word count: "+QString::number(ui->yaml->document()->characterCount())+" "
                       "Line count: "+QString::number(ui->yaml->document()->lineCount()));


yaml_oneditor=ui->yaml->toPlainText().toUtf8();
}

void MainWindow::on_save_snapcraft_clicked()
{
    ui->terminal->append("save requested<br>");
     //save snapcraft.yaml

    //if file saved
    if(saveFile()){
    ui->terminal->append("<span style='color:red'>Editor: </span>Saved file"+fileName+done_message);

    //disable after save
    ui->save_snapcraft->setDisabled(true);

    //save the initial content into a string to compare later in yaml text xhanged slot to change state of save btn
    snapcraft_yaml.clear();
    snapcraft_yaml = ui->yaml->toPlainText();
    }

}

//save snapcraft.yaml return
bool MainWindow::saveFile()
{
    if(!fileName.contains("snapcraft.yaml")){
        fileName=fileName+"/snapcraft.yaml";
    }   //to avoid requesting filesave as a directory (in some cases) , so wrote this new save file function to output what going on
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->yaml->toPlainText().toUtf8();
    last_saved_text =ui->yaml->toPlainText().toUtf8();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return true;
}

// enable disable widgets if snapcraft.yaml is open or closed
void MainWindow::on_snapcraft_path_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()){

        ui->actionOpen_Snapcraft_with_Gedit->setEnabled(false);

        ui->yaml->clear();
        ui->yaml->setDisabled(true);

        ui->tree->clear();
        ui->tree->setDisabled(true);
        ui->tree_now->setDisabled(true);

        ui->terminal->clear();

        ui->commands_frame->setDisabled(true);

        ui->open_with_files->setDisabled(true);

        ui->pastebin_it->setDisabled(true);

        ui->actionUbuntu_Paste->setDisabled(true);

        ui->open_with_gedit->setDisabled(true);

        ui->actionBuild->setDisabled(true);
        ui->actionClean->setDisabled(true);
        ui->actionPrime->setDisabled(true);
        ui->actionPull->setDisabled(true);
        ui->actionSnap->setDisabled(true);
        ui->actionStage->setDisabled(true);
    }
    else{

        ui->actionOpen_Snapcraft_with_Gedit->setEnabled(true);

        ui->yaml->setDisabled(false);

        ui->tree->setDisabled(false);
        ui->tree_now->setDisabled(false);

        ui->commands_frame->setDisabled(false);

        ui->open_with_files->setDisabled(false);

        ui->actionUbuntu_Paste->setDisabled(false);
        ui->pastebin_it->setDisabled(false);

        ui->open_with_gedit->setDisabled(false);

        ui->actionBuild->setDisabled(false);
        ui->actionClean->setDisabled(false);
        ui->actionPrime->setDisabled(false);
        ui->actionPull->setDisabled(false);
        ui->actionSnap->setDisabled(false);
        ui->actionStage->setDisabled(false);
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

    //rules
    if(ui->terminal->toPlainText().contains("snapcraft update")){
      on_actionPlugins_list_update_triggered();
    }
    }

void MainWindow::parts_update_finished(int i){
    if(i==0){
        ui->terminal->append(parts_update->readAll());
        ui->terminal->append(done_message);
    }
    else{
        ui->terminal->append(parts_update->readAllStandardError());
    }

}
void MainWindow::add_part_update_output_to_terminal(){
}

void MainWindow::on_clear_term_clicked()
{
    ui->terminal->clear();
}



void MainWindow::on_tree_now_clicked()
{
    qDebug()<<ui->tree->width();
//    if(ui->tree->width()<10){
//        qDebug()<<"true";
//        ui->tree->resize(600,ui->tree->height());
//        split1;
//    }
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

void MainWindow::on_actionClean_triggered()
{
    clean_custom();
}


//pastebin-it function
void MainWindow::on_pastebin_it_clicked()
{
    ui->save_snapcraft->click();
    ui->terminal->append("<span style='color:red'>Editor: </span>Saved file before uploading as paste.<br>");

    QString o = "cat "+ui->snapcraft_path->text()+"| pastebinit";
    pastebin_it->start("bash", QStringList()<<"-c"<< o);
    ui->pastebin_it->setText("Wait..");
    ui->terminal->append("<span style='color:red'>Editor: </span>Uploading paste please wait... <br>");
    ui->pastebin_it->setDisabled(true);
}

void MainWindow::pastebin_it_finished(int k){

    if(k==0){
        QString url = pastebin_it->readAll();
        ui->terminal->append("<span style='color:red'>Editor: </span>"+url+" @ "+QDateTime::currentDateTime().toString()+"<br>");
        ui->terminal->append("<span style='color:red'>Editor: </span>pastebin url copied to clipboard. <br>");
        ui->pastebin_it->setText("Pastebin-it");
        ui->pastebin_it->setDisabled(false);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(url);
    }
    else if(k==1){
         ui->terminal->append("<span style='color:red'>"+pastebin_it->readAllStandardError()+"</span><br>");
         ui->pastebin_it->setText("Pastebin-it");
         ui->pastebin_it->setDisabled(false);
    }
}

//initiate store
void MainWindow::on_search_store_clicked()
{
    store *Store = new store(this);
    Store->exec();
}

void MainWindow::on_actionUbuntu_Paste_triggered()
{
    on_pastebin_it_clicked();
}

//the clean toolbutton
void MainWindow::on_clean_toolButton_clicked()
{
        QMenu *menu=new QMenu(this);
        menu->addAction(QIcon(":/rc/rc/branch_closed.png"),QString("Custom"),this,SLOT(clean_custom()));
        menu->addAction(QIcon(":/rc/rc/branch_closed.png"),QString("All"), this,SLOT(clean_all()));
        menu->popup(ui->clean_toolButton->mapToGlobal(QPoint(0,30)));
}

void MainWindow::clean_custom(){

  command_widget=new QWidget();
  cui.setupUi(command_widget);
  command_widget->setWindowFlags(Qt::Popup);
  command_widget->move(ui->clean_toolButton->mapToGlobal(QPoint(-command_widget->width()+60,30)));
  cui.clean_button->setDisabled(true);
  command_widget->showNormal();


  //connections to cui
  connect(cui.step_combo,SIGNAL(currentIndexChanged(QString)),this,SLOT(step_changed(QString)));
  connect(cui.clean_button,SIGNAL(clicked(bool)),this,SLOT(custom_clean()));
  connect(cui.part_name,SIGNAL(textChanged(QString)),this,SLOT(part_text_changed(QString)));
  connect(cui.part_name,SIGNAL(returnPressed()),this,SLOT(custom_clean()));

  cui.step_combo->setCurrentIndex(1);
  cui.step_combo->setCurrentIndex(0);

}

//slots to cui
void MainWindow::custom_clean(){
    menuaction="custom";//work around to prevent crash due to clean_dialog initialization when process finished by cleanall menu action
    if(part_str.length()>0){
    if(step_str!="all"||step_str!="All"){
    ui->terminal->setText("<span style='color:red'>Cleaning - </span>"+step_str+" of "+part_str+"\n<br>");

    QStringList arg;
    arg<<"clean"<<part_str<<"--step"<<step_str;

    clean_proc->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
    clean_proc->start("snapcraft",arg);

    ui->build->setDisabled(false);
    ui->stage->setDisabled(true);
    ui->snap->setDisabled(true);
    ui->pull->setDisabled(true);
    ui->prime->setDisabled(true);
}
    }
    else if(step_str=="all"||step_str=="All"){ // when part is nothing i.e cleaning all
        ui->terminal->setText("<span style='color:red'>Cleaning - </span>"+step_str+"\n<br>");
        QStringList arg;
        arg<<"clean";
        clean_proc->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        clean_proc->start("snapcraft",arg);

        ui->build->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->prime->setDisabled(true);
    }


}

void MainWindow::clean_proc_finished(int i){

    if(i==0){
        if(menuaction!="All_"){ //this will only happen when commands come from custom clean dialog
        if(command_widget->isVisible()){
        command_widget->close();}
        }
        ui->terminal->append("<span style='color:red'>Cleaning - </span>Finished.");
        show_tree(); //tree as/maybe the structure changed
        part_str.clear();
        step_str.clear();
    }
    else{
        if(menuaction!="All_"){//this will only happen when commands come from custom clean dialog
        if(command_widget->isVisible()){
        command_widget->close();}
        }
        //check if snapcraft.yaml exists in dir or not
        bool n = QFileInfo(ui->snapcraft_path->text()).exists();
        if(!n){//to check if yaml file is there or not
         ui->terminal->append("snapcraft.yaml file not in directory.\n");
        }
        else{//when snapcraft cant perform clean step
         //no need to tree since nothing changed
         ui->terminal->append("<span style='color:red'>Error: </span>Something went wrong. Maybe part is not defined in snapcraft.yaml.\n<br>");
         ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+clean_proc->readAllStandardError());
         part_str.clear();
         step_str.clear();
        }
    }
    ui->build->setDisabled(false);
    ui->stage->setDisabled(false);
    ui->snap->setDisabled(false);
    ui->pull->setDisabled(false);
    ui->prime->setDisabled(false);
}

void MainWindow::clean_proc_readyRead(){
     ui->terminal->append(clean_proc->readAll());
}

void MainWindow::clean_all(){

    menuaction="All_";//work around to prevent crash due to clean_dialog initialization when process finished by cleanall menu action

    step_str="all";
    ui->terminal->setText("<span style='color:red'>Cleaning - </span>"+step_str+"\n<br>");
    QStringList arg;
    arg<<"clean";
    clean_proc->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
    clean_proc->start("snapcraft",arg);

    ui->build->setDisabled(true);
    ui->stage->setDisabled(true);
    ui->snap->setDisabled(true);
    ui->pull->setDisabled(true);
    ui->prime->setDisabled(true);
}

void MainWindow::step_changed(QString step){
    if(step!="All"){
    step_str=step.toLower(); //lower case the steps to make them good for snapcraft command in future
    if(part_str.isEmpty()){
        cui.clean_button->setDisabled(true);
    }
    cui.part_name->setDisabled(false);
    cui.clean_button->setText("Clean "+step_str+" of "+part_str);
    }
    else{ //step is all
     step_str=step.toLower();
     cui.part_name->clear();
     part_str.clear();
     cui.part_name->setDisabled(true);
     cui.clean_button->setDisabled(false);
     cui.clean_button->setText("Clean "+step_str);
    }
}
void MainWindow::part_text_changed(QString part){
    cui.part_name->setText(part.simplified());
     if(cui.step_combo->currentText()!="All"){
    if(part.length()>0){
        cui.clean_button->setDisabled(false);
        part_str=part;
        cui.clean_button->setText("Clean "+step_str+" of "+part_str);

    }
    else{
    cui.clean_button->setDisabled(true);
    }
     }
    else{
        cui.clean_button->setDisabled(true);
    }
}


//snap clicked
void MainWindow::on_snap_clicked()
{
    if(ui->snap->text()=="Snap"){
    if(ui->save_snapcraft->isEnabled()){  //changes are made by user
        QMessageBox msgBox;
        msgBox.setText("Snapcraft.yaml has been modified.");
        msgBox.setInformativeText("Do you want to save your\n\nchanges before building ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.buttons().at(0)->setText("Save and Snap");
        msgBox.buttons().at(1)->setText("Snap without Saving");
        ret = msgBox.exec(); //return code


        switch (ret){
          case QMessageBox::Yes:
            //save craft file
            on_save_snapcraft_clicked();
            //snap the craft file
            snap_snapcraft();
              break;
          case QMessageBox::No:
               //snap the craft file
                snap_snapcraft();
              break;
          default:
              // should never be reached
              break;
        }
    }
    else{
        snap_snapcraft();
    }
 }else{
        ui->snap->setText("Snap");//instantly
        snap->kill();
    }
}
// snap command via Qprocess
void MainWindow::snap_snapcraft(){

    ui->build->setDisabled(true);
    ui->pull->setDisabled(true);
    ui->stage->setDisabled(true);
    ui->prime->setDisabled(true);

    if(ui->snap->text()=="Snap"){
    snap->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
    snap->start("snapcraft",QStringList()<<"snap");

    ui->snap->setText("Cancel");
    ui->snap->setDisabled(false);

    ui->terminal->setText("<span style='color:red'>Snapcraft: </span>snapping "+fileName+"<br>");
}
    else{
        ui->snap->setText("Snap");//instantly
        snap->kill();
        show_tree();
    }
}

void MainWindow::snap_finished(int i){

    if(i==0){
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Finished.");
        show_tree();
    }
    else{
        if(ui->snap->text()=="Snap"){
           ui->terminal->append("<span style='color:red'>Snapcraft: </span>Cancelled on user request");
        }else{
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Something went wrong.<br>");
        ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+snap->readAllStandardError());
        }
    }

    QTimer::singleShot(500,this,SLOT(set_name_snap()));  //timer to output "Something went wrong." by changing name after a while
    ui->snap->setDisabled(false);

    ui->build->setDisabled(false);
    ui->pull->setDisabled(false);
    ui->stage->setDisabled(false);
    ui->prime->setDisabled(false);
}

void MainWindow::set_name_snap(){ //this is lol
    ui->snap->setText("Snap");
}
void MainWindow::snap_readyRead(){

    ui->terminal->append(snap->readAll());
}

void MainWindow::on_yaml_customContextMenuRequested(const QPoint &pos)
{


//       QMenu *menu =  new QMenu(this);//= ui->yaml->createStandardContextMenu();
       editor_menu=new QMenu(this);


       editor_menu->addAction(QString("Copy"),ui->yaml,SLOT(copy()) );
       editor_menu->addAction(QString("Cut"),ui->yaml,SLOT(cut()) );
       editor_menu->addAction(QString("Paste"),this,SLOT(insertPlainText()));
       editor_menu->addAction(QString("Select All"),ui->yaml,SLOT(selectAll()) );
       editor_menu->addSeparator();
       editor_menu->addAction(QString("Undo ("+QString::number(ui->yaml->document()->availableUndoSteps())+")"),ui->yaml,SLOT(undo()) );
       editor_menu->addAction(QString("Redo("+QString::number(ui->yaml->document()->availableRedoSteps())+")"),ui->yaml,SLOT(redo()) );


        //copy
       QString d = ui->yaml->textCursor().selectedText();
       bool text_available;
       if(d.length()>0){
        text_available=false;
       }else{text_available=true;}
       editor_menu->actions().at(0)->setDisabled(text_available);
       editor_menu->actions().at(1)->setDisabled(text_available);

       //paste
       if (const QMimeData *md = QApplication::clipboard()->mimeData()){
                 editor_menu->actions().at(2)->setEnabled(md->hasText());
       }

       //Select All
       bool has_text;
       if(ui->yaml->document()->blockCount()>0){
           has_text=true;
       }else{has_text=false;}
        editor_menu->actions().at(3)->setEnabled(has_text);
        //undo
       editor_menu->actions().at(5)->setEnabled(ui->yaml->document()->isUndoAvailable());
       //redo
       editor_menu->actions().at(6)->setEnabled(ui->yaml->document()->isRedoAvailable());


       editor_menu->exec(ui->yaml->viewport()->mapToGlobal(pos));
       delete editor_menu;
}


void MainWindow::insertPlainText(){
    const QClipboard *clipboard = QApplication::clipboard();
    QString clip =clipboard->text();
    ui->yaml->textCursor().insertText(clip);
}

void MainWindow::on_zoom_in_clicked()
{
     ui->yaml->zoomIn(1);
     ui->zoom->setText(QString::number(ui->yaml->fontInfo().pixelSize()));
}

void MainWindow::on_zoom_out_clicked()
{
    ui->yaml->zoomOut(1);
    ui->zoom->setText(QString::number(ui->yaml->fontInfo().pixelSize()));
}

void MainWindow::on_undo_btn_clicked()
{
    ui->yaml->undo();
}

void MainWindow::on_redo_btn_clicked()
{
    ui->yaml->redo();
}




void MainWindow::on_actionSearchStore_triggered()
{
    on_search_store_clicked();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog abt;
    abt.exec();
}



//pull commands
void MainWindow::on_pull_clicked()
{
    if(ui->pull->text()=="Cancel"){
        ui->pull->setText("Pull");//instantly , so that we can detect user cliked cancel
        pull->kill();
        show_tree();
    }
    else
    {
    command_widget=new QWidget();
    pui.setupUi(command_widget);

    //toggle pui radios
    pui.all_radio->setChecked(true);
    pui.part_name->setDisabled(true);

    command_widget->setWindowFlags(Qt::Popup);
    command_widget->move(ui->pull->mapToGlobal(QPoint(-command_widget->width()+100,30)));
    command_widget->showNormal();

    connect(pui.all_radio,SIGNAL(toggled(bool)),this,SLOT(all_radio_toggled(bool)));
    connect(pui.part_name,SIGNAL(textChanged(QString)),this,SLOT(change_puis_pullbtn_text(QString)));
    connect(pui.pull_button,SIGNAL(clicked(bool)),this,SLOT(pull_command_requested()));

    }
}

void MainWindow::pull_command_requested(){

    if(pui.pull_button->text()=="Pull All Parts"){
        pull->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        pull->start("snapcraft",QStringList()<<"pull");

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->prime->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->pull->setText("Cancel");
        pui.pull_button->setDisabled(true);
        ui->pull->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>pulling parts in "+fileName+"<br>");

    }
    else{
        pull->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        pull->start("snapcraft",QStringList()<<"pull"<<pui.part_name->text());

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->prime->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->pull->setText("Cancel");
        pui.pull_button->setDisabled(true);
        ui->pull->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>pulling "+pui.part_name->text()+" part of "+fileName+"<br>");

    }

}

void MainWindow::change_puis_pullbtn_text(QString txt){
    pui.part_name->setText(txt.simplified());
    if(txt.length()>0){
        pui.pull_button->setEnabled(true);
        pui.pull_button->setText("Pull "+txt);
    }else{
        pui.pull_button->setEnabled(false);
    }
}

void MainWindow::pull_finished(int i){
    if(i==0){
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Finished.");
        show_tree();
    }
    else{
        if(ui->pull->text()=="Pull"){
           ui->terminal->append("<span style='color:red'>Snapcraft: </span>Pull Cancelled on user request");
        }else{
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Something went wrong. Maybe part is not defined in snapcraft.yaml<br><span style='color:green'>Suggestions: </span>check syntax errors in yaml file.<br><span style='color:green'>Suggestion: </span>This may happen due to internet connectivity issues.<br>");
        ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+pull->readAllStandardError());
        }
    }

    QTimer::singleShot(500,this,SLOT(set_name_pull()));  //timer to output "Something went wrong." by changing name after a while
    ui->pull->setDisabled(false);

    ui->build->setDisabled(false);
    ui->snap->setDisabled(false);
    ui->stage->setDisabled(false);
    ui->prime->setDisabled(false);
    ui->clean_toolButton->setDisabled(false);

    if(command_widget->isVisible()){
    command_widget->close();
    }
}

void MainWindow::set_name_pull(){
ui->pull->setText("Pull");
}
void MainWindow::pull_readyRead(){
ui->terminal->append(pull->readAll());
}

void MainWindow::all_radio_toggled(bool checked)
{
    if(checked){
        pui.part_name->setDisabled(true);
        pui.pull_button->setDisabled(false);
        pui.pull_button->setText("Pull All Parts");
        pui.part_name->clear();
    }
    else if(!checked){
        if(pui.part_name->text().length()<0)
        pui.pull_button->setText("Pull");
        pui.part_name->setDisabled(false);
        pui.pull_button->setDisabled(true);
    }
}


//stage command
void MainWindow::on_stage_clicked()
{
    if(ui->stage->text()=="Cancel"){
        ui->stage->setText("Stage");//instantly , so that we can detect user cliked cancel
        stage->kill();
        show_tree();
    }
    else
    {
    command_widget=new QWidget();
    sui.setupUi(command_widget);

    //toggle sui radios
    sui.all_radio->setChecked(true);
    sui.part_name->setDisabled(true);

    command_widget->setWindowFlags(Qt::Popup);
    command_widget->move(ui->stage->mapToGlobal(QPoint(-command_widget->width()+100,30)));
    command_widget->showNormal();

    connect(sui.all_radio,SIGNAL(toggled(bool)),this,SLOT(stage_all_radio_toggled(bool)));
    connect(sui.part_name,SIGNAL(textChanged(QString)),this,SLOT(change_suis_stagebtn_text(QString)));
    connect(sui.stage_button,SIGNAL(clicked(bool)),this,SLOT(stage_command_requested()));

    }
}

void MainWindow::stage_command_requested(){

    if(sui.stage_button->text()=="Stage All Parts"){
        stage->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        stage->start("snapcraft",QStringList()<<"stage");

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->prime->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->stage->setText("Cancel");
        sui.stage_button->setDisabled(true);
        ui->stage->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>stagging parts of "+fileName+"<br>");

    }
    else{
        stage->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        stage->start("snapcraft",QStringList()<<"stage"<<sui.part_name->text());

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->prime->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->stage->setText("Cancel");
        sui.stage_button->setDisabled(true);
        ui->stage->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>stagging "+sui.part_name->text()+" part of "+fileName+"<br>");

    }

}
void MainWindow::change_suis_stagebtn_text(QString txt){

    sui.part_name->setText(txt.simplified());
    if(txt.length()>0){
        sui.stage_button->setEnabled(true);
        sui.stage_button->setText("Stage "+txt);
    }else{
        sui.stage_button->setEnabled(false);
    }
}

void MainWindow::stage_finished(int i){
    if(i==0){
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Finished.");
        show_tree();
    }
    else{
        if(ui->stage->text()=="Pull"){
           ui->terminal->append("<span style='color:red'>Snapcraft: </span>Stage Cancelled on user request");
        }else{
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Something went wrong. Maybe part is not defined in snapcraft.yaml<br><span style='color:green'>Suggestions: </span>check syntax errors in yaml file.<br><span style='color:green'>Suggestion: </span>This may happen due to internet connectivity issues.<br>");
        ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+stage->readAllStandardError());

        }
    }

    QTimer::singleShot(500,this,SLOT(set_name_stage()));  //timer to output "Something went wrong." by changing name after a while
    ui->stage->setDisabled(false);

    ui->build->setDisabled(false);
    ui->snap->setDisabled(false);
    ui->pull->setDisabled(false);
    ui->prime->setDisabled(false);
    ui->clean_toolButton->setDisabled(false);

    if(command_widget->isVisible()){
    command_widget->close();
    }
}

void MainWindow::set_name_stage(){
ui->stage->setText("Stage");
}
void MainWindow::stage_readyRead(){
ui->terminal->append(stage->readAll());
}

void MainWindow::stage_all_radio_toggled(bool checked)
{
    if(checked){
        sui.part_name->setDisabled(true);
        sui.stage_button->setDisabled(false);
        sui.stage_button->setText("Stage All Parts");
        sui.part_name->clear();
    }
    else if(!checked){
        if(sui.part_name->text().length()<0)
        sui.stage_button->setText("Stage");
        sui.part_name->setDisabled(false);
        sui.stage_button->setDisabled(true);
    }
}


//prime command
void MainWindow::on_prime_clicked()
{
    if(ui->prime->text()=="Cancel"){
        ui->prime->setText("Prime");//instantly , so that we can detect user cliked cancel
        prime->kill();
        show_tree();
    }
    else
    {
    command_widget=new QWidget();
    prui.setupUi(command_widget);

    //toggle sui radios
    prui.prime_all_radio->setChecked(true);
    prui.part_name->setDisabled(true);

    command_widget->setWindowFlags(Qt::Popup);
    command_widget->move(ui->prime->mapToGlobal(QPoint(-command_widget->width()+100,30)));
    command_widget->showNormal();

    connect(prui.prime_all_radio,SIGNAL(toggled(bool)),this,SLOT(prime_all_radio_toggled(bool)));
    connect(prui.part_name,SIGNAL(textChanged(QString)),this,SLOT(change_prime_uis_stagebtn_text(QString)));
    connect(prui.prime_button,SIGNAL(clicked(bool)),this,SLOT(prime_command_requested()));

    }
}

void MainWindow::prime_command_requested(){

    if(prui.prime_button->text()=="Prime All Parts"){
        prime->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        prime->start("snapcraft",QStringList()<<"prime");

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->prime->setText("Cancel");
        prui.prime_button->setDisabled(true);
        ui->prime->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>primming parts of "+fileName+"<br>");

    }
    else{
        prime->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        prime->start("snapcraft",QStringList()<<"prime"<<prui.part_name->text());

        ui->build->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->prime->setText("Cancel");
        prui.prime_button->setDisabled(true);
        ui->prime->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>primming "+sui.part_name->text()+" part of "+fileName+"<br>");

    }

}
void MainWindow::change_prime_uis_stagebtn_text(QString txt){

    prui.part_name->setText(txt.simplified());
    if(txt.length()>0){
        prui.prime_button->setEnabled(true);
        prui.prime_button->setText("Prime "+txt);
    }else{
        prui.prime_button->setEnabled(false);
    }
}

void MainWindow::prime_finished(int i){
    if(i==0){
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Finished.");
        show_tree();
    }
    else{
        if(ui->prime->text()=="Prime"){
           ui->terminal->append("<span style='color:red'>Snapcraft: </span>Prime Cancelled on user request");
        }else{
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Something went wrong. Maybe part is not defined in snapcraft.yaml<br><span style='color:green'>Suggestions: </span>check syntax errors in yaml file.<br><span style='color:green'>Suggestion: </span>This may happen due to internet connectivity issues.<br>");
        ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+prime->readAllStandardError());
        }
    }

    QTimer::singleShot(500,this,SLOT(set_name_prime()));  //timer to output "Something went wrong." by changing name after a while
    ui->prime->setDisabled(false);

    ui->build->setDisabled(false);
    ui->snap->setDisabled(false);
    ui->pull->setDisabled(false);
    ui->stage->setDisabled(false);
    ui->clean_toolButton->setDisabled(false);

    if(command_widget->isVisible()){
    command_widget->close();
    }
}

void MainWindow::set_name_prime(){
ui->prime->setText("Prime");
}
void MainWindow::prime_readyRead(){
ui->terminal->append(prime->readAll());
}

void MainWindow::prime_all_radio_toggled(bool checked)
{
    if(checked){
        prui.part_name->setDisabled(true);
        prui.prime_button->setDisabled(false);
        prui.prime_button->setText("Prime All Parts");
        prui.part_name->clear();
    }
    else if(!checked){
        if(prui.part_name->text().length()<0)
        prui.prime_button->setText("Prime");
        prui.part_name->setDisabled(false);
        prui.prime_button->setDisabled(true);
    }
}

//build command
void MainWindow::on_build_clicked()
{
    if(ui->build->text()=="Cancel"){
        ui->build->setText("Build");//instantly , so that we can detect user cliked cancel
        build->kill();
        show_tree();
    }
    else
    {
    command_widget=new QWidget();
    bui.setupUi(command_widget);

    //toggle sui radios
    bui.build_all_radio->setChecked(true);
    bui.part_name->setDisabled(true);

    command_widget->setWindowFlags(Qt::Popup);
    command_widget->move(ui->build->mapToGlobal(QPoint(-command_widget->width()+100,30)));
    command_widget->showNormal();

    connect(bui.build_all_radio,SIGNAL(toggled(bool)),this,SLOT(build_all_radio_toggled(bool)));
    connect(bui.part_name,SIGNAL(textChanged(QString)),this,SLOT(change_build_buis_stagebtn_text(QString)));
    connect(bui.build_button,SIGNAL(clicked(bool)),this,SLOT(build_command_requested()));
    connect(bui.no_parallel_build,SIGNAL(toggled(bool)),this,SLOT(no_parallel_build_checked_changed(bool)));

    }
}

void MainWindow::build_command_requested(){

    if(bui.build_button->text()=="Build All Parts"){
        build->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        build->start("snapcraft",QStringList()<<"build"<<paraller_build);

        ui->prime->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->build->setText("Cancel");
        bui.build_button->setDisabled(true);
        ui->build->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>building parts of "+fileName+"<br>");

    }
    else{
        build->setWorkingDirectory(QString(fileName).remove("/snapcraft.yaml"));
        build->start("snapcraft",QStringList()<<"build"<<prui.part_name->text()<<paraller_build);

        ui->prime->setDisabled(true);
        ui->snap->setDisabled(true);
        ui->pull->setDisabled(true);
        ui->stage->setDisabled(true);
        ui->clean_toolButton->setDisabled(true);

        ui->build->setText("Cancel");
        bui.build_button->setDisabled(true);
        ui->build->setDisabled(false);

        ui->terminal->setText("<span style='color:red'>Snapcraft: </span>building "+bui.part_name->text()+" part of "+fileName+"<br>");

    }

}
void MainWindow::change_build_buis_stagebtn_text(QString txt){

    bui.part_name->setText(txt.simplified());
    if(txt.length()>0){
        bui.build_button->setEnabled(true);
        bui.build_button->setText("Build "+txt);
    }else{
        bui.build_button->setEnabled(false);
    }
}

void MainWindow::build_finished(int i){
    if(i==0){
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Finished.");
        show_tree();
    }
    else{
        if(ui->build->text()=="Build"){
           ui->terminal->append("<span style='color:red'>Snapcraft: </span>Build Cancelled on user request");
        }else{
        ui->terminal->append("<span style='color:red'>Snapcraft: </span>Something went wrong. Maybe part is not defined in snapcraft.yaml<br><span style='color:green'>Suggestions: </span>check syntax errors in yaml file.<br><span style='color:green'>Suggestion: </span>This may happen due to internet connectivity issues.<br>");
        ui->terminal->append("<span style='color:red'>Snapcraft returned with : </span>"+build->readAllStandardError());

        }
    }

    QTimer::singleShot(500,this,SLOT(set_name_build()));  //timer to output "Something went wrong." by changing name after a while
    ui->build->setDisabled(false);

    ui->prime->setDisabled(false);
    ui->snap->setDisabled(false);
    ui->pull->setDisabled(false);
    ui->stage->setDisabled(false);
    ui->clean_toolButton->setDisabled(false);

    if(command_widget->isVisible()){
    command_widget->close();
    }
}

void MainWindow::set_name_build(){
ui->build->setText("Build");
}
void MainWindow::build_readyRead(){
ui->terminal->append(build->readAll());
}

void MainWindow::build_all_radio_toggled(bool checked)
{
    if(checked){
       bui.part_name->setDisabled(true);
       bui.build_button->setDisabled(false);
       bui.build_button->setText("Build All Parts");
       bui.part_name->clear();
    }
    else if(!checked){
        if(bui.part_name->text().length()<0)
        bui.build_button->setText("Build");
        bui.part_name->setDisabled(false);
        bui.build_button->setDisabled(true);
    }
}

void MainWindow::no_parallel_build_checked_changed(bool k){

    if(k){
        paraller_build="--no-parallel-build";
    }else{
        paraller_build.clear();
    }
}

//login/logout//////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionLogin_triggered()
{
    connect(this->login,SIGNAL(finished(int)),this,SLOT(login_finished(int)));
    connect(this->login,SIGNAL(readyRead()),this,SLOT(login_readyRead()));
    login->start("gnome-terminal",QStringList()<<"-e"<<"snapcraft login");
}

void MainWindow::login_finished(int k){
    if(k==0){
        qDebug()<<"done";
    }else{
        qDebug()<<"fucked";
    }
}

void MainWindow::login_readyRead(){
    qDebug()<<login->readAll();
//    qDebug()<</
}
//end login/logout//////////////////////////////////////////////////////////////////////////


//to keep an eye on snapcraft file changes on disk if file is changed evoke a reaload button on screen
void MainWindow::on_yaml_cursorPositionChanged()
{
    int currentLine = ui->yaml->textCursor().blockNumber() + 1;
    ui->current_line->setText(QString::number(currentLine));
    //read data of file on disk
    if(!fileName.isEmpty()&& !last_saved_text.isEmpty()){
        QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return;
            QTextStream in(&file);
            QStringList strl ;
            ui->yaml_2->clear();
            strl.clear();
            yaml_ondisk.clear();//checl new changes always
            while (!in.atEnd())
              {
                strl.append(in.readLine());
              }
            //add data to editor
            for(int i= 0; i<strl.size() ;i++){
              ui->yaml_2->append(strl.at(i));
             }
             yaml_ondisk=ui->yaml_2->toPlainText();

    //comapre data of disk file and current editor
    if(yaml_ondisk!=last_saved_text){
        ui->file_changed_frame->show();
        ui->yaml_2->show();//show reload stuffs
    }else{
            ui->file_changed_frame->hide();
            ui->yaml_2->hide();
        }
    }
}

//reload button action
void MainWindow::on_reload_file_clicked()
{
    //so that user can do undo redo
    ui->yaml->selectAll();
    ui->yaml->cut();
    ui->yaml->toPlainText().remove("\n");

    //add data and save current version of file
    ui->yaml->append(yaml_ondisk);
    last_saved_text.clear();
    ui->file_changed_frame->hide();
    ui->yaml_2->hide();
    ui->save_snapcraft->click();
}
//ignore changes action
void MainWindow::on_ignore_changes_clicked()
{
    //last_saved_text.clear();
    ui->file_changed_frame->hide();
    ui->yaml_2->hide();
    //focibly save the user's current changes to file as he requested
    if(!ui->save_snapcraft->isEnabled()){
        ui->save_snapcraft->setEnabled(true);
        ui->save_snapcraft->click();
    }
}

void MainWindow::on_actionPlugins_list_update_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Snapcraft need update plugins list.");
    msgBox.setInformativeText("Do you want to update?");
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.buttons().at(1)->setText("Yes Update");
    ret = msgBox.exec(); //return code

    switch (ret) {
      case QMessageBox::Yes:
        parts_update->start("pkexec",QStringList()<<"snapcraft"<<"update");
        ui->terminal->setText("waiting for user action...");
          break;
      case QMessageBox::Cancel:
          break;
      default:
          // should never be reached
          break;
    }
}
