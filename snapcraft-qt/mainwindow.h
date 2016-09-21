#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextCodec>
#include <QMenu>
#include <QSplitter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QTextEdit>

#include "highlighter.h"

#include "ui_clean_dialog.h"
#include "ui_pull_dialog.h"
#include "ui_stage_dialog.h"
#include "ui_prime_dialog.h"
#include "ui_build_dialog.h"
#include "ui_notes.h"
#include "ui_recent_projects.h"

#include "ui_editor_settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;
    Ui::clean_dialog cui;
    Ui::pull_dialog pui;
    Ui::stage_dialog sui;
    Ui::prime_dialog prui;
    Ui::build_dialog bui;
    Ui::notes nui;
    Ui::recent rui;

    Ui::editor_settings e_settings;

    QSettings *settings;

    QString  yaml_ondisk;
    QString  yaml_oneditor;
    QString  last_saved_text;

    QWidget *command_widget;
    QString fileName;
    QString snapname;  //name of snap
    QString firstline; //first line which should conatin "name:"
    QString snapcraft_yaml; //code inside yaml file
    QProcess *snapcraft;


    QSplitter *split1;
    QString done_message;
    int ret;
    Highlighter *highlighter;
    QProcess *pastebin_it;

    //command process
    QProcess *clean_proc;

    //cui vars
    QString menuaction;
    QString part_str;
    QString step_str;

    //snap commands
    QProcess *snap;
    //pull commands
    QProcess *pull;
    //stage commands
    QProcess *stage;
    //prime commands
    QProcess *prime;
    //build command
    QProcess *build;
    QString paraller_build;

    QProcess *parts_update;
    //login
    QProcess *login;

    //yaml editor menu
    QMenu *editor_menu;

    //network
    QNetworkReply *reply;
    QNetworkAccessManager m_network_manager;
    int rand;

    //lineno.
    QTextEdit *currentLineNumberArea();

    //recent project
    QString recent_Project_url;


private slots:
    bool saveFile();

    void highlightCurrentLine();

    void documentWasModified();
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
//    void on_highlight_clicked();
//    void request_done();
    void on_normal_clicked();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionQuit_triggered();
    void on_actionInstall_a_snap_triggered();
    void on_actionClose_Currrent_triggered();
    void on_actionList_plugins_triggered();
    void on_actionSnapcraft_Plugins_Help_triggered();
    void on_actionWebsite_triggered();
    void on_open_with_gedit_clicked();
    void on_package_manager_clicked();
    void on_pastebin_it_clicked();
    void pastebin_it_finished(int k);
    void on_search_store_clicked();
    void on_actionUbuntu_Paste_triggered();
    void on_clean_toolButton_clicked();

    //clean steps slots
    void clean_custom();
    void clean_all();
    void step_changed(QString);
    void part_text_changed(QString);
    void custom_clean(); //slot to perform custom clean process
    void clean_proc_readyRead();
    void clean_proc_finished(int);

    void on_actionClean_triggered();
    void on_snap_clicked();
    void snap_finished(int);
    void snap_snapcraft();
    void snap_readyRead();
    void set_name_snap();
    void on_yaml_customContextMenuRequested(const QPoint &pos);

    void insertPlainText();


    void on_zoom_in_clicked();
    void on_zoom_out_clicked();
    void on_undo_btn_clicked();
    void on_redo_btn_clicked();
    void on_pull_clicked();
    void on_stage_clicked();
    void on_prime_clicked();
    void on_actionSearchStore_triggered();
    void on_actionAbout_triggered();

    //pull command
    void all_radio_toggled(bool);
    void change_puis_pullbtn_text(QString);
    void pull_command_requested();
    void pull_finished(int);
    void pull_readyRead();
    void set_name_pull();

   //stage command
   void stage_all_radio_toggled(bool);
   void change_suis_stagebtn_text(QString);
   void stage_command_requested();
   void stage_finished(int);
   void stage_readyRead();
   void set_name_stage();

   //prime command
   void prime_all_radio_toggled(bool);
   void change_prime_uis_stagebtn_text(QString);
   void prime_command_requested();
   void prime_finished(int);
   void prime_readyRead();
   void set_name_prime();

   //build command
   void on_build_clicked();
   void build_all_radio_toggled(bool);
   void change_build_buis_stagebtn_text(QString);
   void build_command_requested();
   void build_finished(int);
   void build_readyRead();
   void set_name_build();
   void no_parallel_build_checked_changed(bool);

   //login/logout
    void on_actionLogin_triggered();
    void  login_finished(int);
    void  login_readyRead();
    void on_yaml_cursorPositionChanged();
     void on_font_currentFontChanged(const QFont &f);
     void on_reload_file_clicked();
     void on_ignore_changes_clicked();


     void add_part_update_output_to_terminal();
     void parts_update_finished(int);
     void on_actionPlugins_list_update_triggered();
     void on_actionDemo_snapcraft_triggered();


     void load_demo();
     void demo_file_request_done();
     void demo_download_progress(qint64, qint64);

     //esitor settings
     void on_editor_settings_clicked();
     void  e_settings_keep_sync_toggled(bool);
     void  e_settings_save_font_toggled(bool);
     void  e_settings_save_zoom_toggled(bool);
     void on_font_currentIndexChanged(int index);

     //updater
     void check_for_updates();
     void version_request_done();

     //editor
     void duplicateText();
     void on_dup_clicked();


     //line no.
     void updateLineNumberArea(QTextEdit *lineNumberArea, int num);
     void setLineNumberArea();
     void updateLineNumArea(int num);
     void resize_line_number_widget();
     void show_hide_line_no(bool);
     void on_pasteup_2_clicked();
     void on_select_all_clicked();

//     tree
     void show_hide_tree_toggled(bool);

//editor indentation
//     void indent(int delta);
//     void increaseIndentation();
//     void decreaseIndentation();

//notes
     void on_notes_clicked();
     void setData();
     void saveData();
     void nui_cursorChanged();
     void on_actionNotes_triggered();
     void changeflag_clicked();
//recent project
     void on_open_recent_clicked();
     void recent_Project_url_changed(QString );
     void open_recent_project();
     void remove_recent_project();
};

#endif // MAINWINDOW_H
