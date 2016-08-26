#ifndef INSTALL_LOCAL_SNAP_DIALOG_H
#define INSTALL_LOCAL_SNAP_DIALOG_H

#include <QDialog>

namespace Ui {
class Install_local_snap_dialog;
}

class Install_local_snap_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Install_local_snap_dialog(QWidget *parent = 0);
    ~Install_local_snap_dialog();

private:
    Ui::Install_local_snap_dialog *ui;
};

#endif // INSTALL_LOCAL_SNAP_DIALOG_H
