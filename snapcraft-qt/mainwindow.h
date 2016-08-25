#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextCodec>
#include <QSplitter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString snapname;  //name of snap
    QString firstline; //first line which should conatin "name:"
    QString snapcraft_yaml; //code inside yaml file
    QProcess *snapcraft;
    QNetworkReply *reply;
    QNetworkAccessManager m_network_manager;
    QSplitter *split1;
    QString done_message;
    int ret;


private slots:
    void setStyle(QString fname);//set style of player from CSS file
    void hide_current_snap_options(); //hide current snap options
    void show_current_snap_options();//show current snap options
    void hide_session_options(); //hide the session options
    void show_session_options(); //show the session options

    void closeEvent(QCloseEvent *event);


    void load_snapcraft_yaml(); // load snapcraft.yaml to interface

    void close_session(); //close session
    void show_tree();

    void on_new_snap_clicked(); // on new snap button clicked
    void on_close_current_clicked();
    void on_open_snap_clicked();
    void on_yaml_textChanged();
    void on_save_snapcraft_clicked();
    void on_snapcraft_path_textChanged(const QString &arg1);
    void on_terminal_textChanged();
    void on_clear_term_clicked();
    void on_tree_now_clicked();
    void on_open_with_files_clicked();
     void on_highlight_clicked();
    void request_done();
    void on_normal_clicked();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionQuit_triggered();
};

#endif // MAINWINDOW_H
