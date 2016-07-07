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
    void on_b_dir_ok_clicked();

    void on_b_start_clicked();

    void on_b_browse_dir_clicked();

    void on_b_resDirBrowse_clicked();

    void on_tb_directory_editingFinished();

    void on_tb_result_dir_editingFinished();

    void on_tb_directory_textChanged(const QString &arg1);

    void on_tb_result_dir_textChanged(const QString &arg1);

    void on_tb_directory_textEdited(const QString &arg1);

    void on_tb_result_dir_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
