#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_b_browse_dir_clicked();

    void on_b_resDirBrowse_clicked();

    void on_b_dir_ok_clicked();

    void on_b_start_clicked();

    void changeLbl(QString);
    void unBlock();

    void on_cb_custom_func_clicked(bool checked);

    void on_b_csv_browse_clicked();

public:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
