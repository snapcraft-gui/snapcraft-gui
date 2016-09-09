#ifndef STORE_H
#define STORE_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class store;
}

class store : public QDialog
{
    Q_OBJECT

public:
    explicit store(QWidget *parent = 0);
    ~store();

private slots:
    void on_search_clicked();

    void on_search_term_textChanged(const QString &arg1);

    void search_finished(int j);

    void on_available_packages_currentTextChanged(const QString &currentText);

    void on_install_clicked();

    void install_remove_finished(int);

    void install_remove_readyRead();


private:
    Ui::store *ui;
    QProcess *search_process;
    QProcess *install_remove_process;
    QString selected_package_name;
};

#endif // STORE_H
