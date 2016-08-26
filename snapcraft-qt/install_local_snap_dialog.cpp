#include "install_local_snap_dialog.h"
#include "ui_install_local_snap_dialog.h"

Install_local_snap_dialog::Install_local_snap_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Install_local_snap_dialog)
{
    ui->setupUi(this);
}

Install_local_snap_dialog::~Install_local_snap_dialog()
{
    delete ui;
}
