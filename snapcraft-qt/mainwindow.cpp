#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
        while (!in.atEnd())
          {
            napname.append(in.readLine());
          }
        //add data to editor
        for(int i= 0; i<napname.size() ;i++){
        ui->yaml->append(napname.at(i));
        }
        ui->terminal->append("Opening <b>"+fileName+"</b>...<br>Done.");
        //set current snap name
        snapname = napname.at(0);
        ui->current_snap->setText("current : <b>"+snapname.remove("name:")+"</b>");

        //save the initial content into a string to compare later
        snapcraft_yaml = ui->yaml->toPlainText();

}
//load snapcraft.yaml to interface-------------------------------



void MainWindow::on_open_snap_clicked()
{
  fileName = QFileDialog::getOpenFileName(this,
        tr("Open SnapCraft"), "", tr("Snapcraft Files (*.yaml)"));
  if(fileName.length()>13){ //verify we got file
      //load file to interface
      load_snapcraft_yaml();
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

    ui->terminal->append("\nComputing tree for <b>"+fileName+"</b>...<br>Done<br>Ready");

    QString prog = "tree";
    QStringList args;
    QFile f(fileName);
    QString path = QFileInfo(f).filePath().remove("snapcraft.yaml");
    args<<"-L"<<"3"<<path;  // to depth of 3
    QProcess *tree = new QProcess(this);
    tree->start(prog, args);
    tree->waitForFinished();
    ui->tree->setText(tree->readAll());

}

void MainWindow::on_new_snap_clicked()
{

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

if(ui->yaml->toPlainText().length()>0 || snapcraft_yaml!=ui->yaml->toPlainText()){
    ui->save_snapcraft->setDisabled(false);

}
else{
    ui->save_snapcraft->setDisabled(true);
}

}

void MainWindow::on_save_snapcraft_clicked()
{
     //save snapcraft.yaml

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

    }
    else{
        ui->yaml->setDisabled(false);

        ui->tree->setDisabled(false);
        ui->tree_now->setDisabled(false);

        ui->terminal->setDisabled(false);

        ui->commands_frame->setDisabled(false);

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
}

void MainWindow::on_clear_term_clicked()
{
    ui->terminal->clear();
}



void MainWindow::on_tree_now_clicked()
{
    show_tree();
}
