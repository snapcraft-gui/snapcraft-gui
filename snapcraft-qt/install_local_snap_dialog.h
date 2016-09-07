#ifndef INSTALL_LOCAL_SNAP_DIALOG_H
#define INSTALL_LOCAL_SNAP_DIALOG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class Install_local_snap_dialog;
}

class Install_local_snap_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Install_local_snap_dialog(QWidget *parent = 0);
    ~Install_local_snap_dialog();
    QString inputstring();
    QString inputstring_from_updatable();
private slots:
    void install_ready_read();

    void install_process_finished(int i);

    void list_installed_snaps();

    void on_choose_snap_clicked();

    void on_install_button_clicked();

    void on_close_dialog_clicked();

    void on_devmode_toggled(bool checked);

    void on_installed_package_clicked(const QModelIndex &index);

    void on_remove_selected_snap_clicked();

    void on_selected_snap_info_clicked();

    void list_updates_ready(int j);

    void info_process_finished(int k);

    void on_update_toggled(bool checked);

    void on_check_for_update_clicked();

    void on_updatable_clicked(const QModelIndex &index);

    void on_tabWidget_currentChanged(int index);

private:
    Ui::Install_local_snap_dialog *ui;
    QProcess *install;
    QProcess *info_process;
    QProcess *list;
    QProcess *list_updates;
    QString snap_path;
    QString install_command;
    bool devmode;
    QString installed_snap_name; // to remove snap after installation
    QString process_name;
};
class FileUtils
{
public:
  //  static bool removeDir(const QString &dirName);
};
quint64 dir_size(const QString &str);

#endif // INSTALL_LOCAL_SNAP_DIALOG_H


