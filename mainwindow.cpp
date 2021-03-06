#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clusteranaliz.h"
#include <iostream>
#include <string>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <QThread>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_b_browse_dir_clicked()
{
    ui->tb_directory->setText(QFileDialog::getExistingDirectory());
}

void MainWindow::on_b_resDirBrowse_clicked()
{
    ui->tb_result_dir->setText(QFileDialog::getExistingDirectory());
}

void MainWindow::on_b_dir_ok_clicked()
{
    if(QDir(ui->tb_directory->text()).exists() && QDir(ui->tb_result_dir->text()).exists()) {
        ui->b_start->setEnabled(true);
        ui->l_ok->setText("press start to begin");
    }
}

void MainWindow::changeLbl(QString str) {
    ui->l_ok->setText(str);
}

void MainWindow::unBlock() {
    ui->b_browse_dir->setEnabled(true);
    ui->b_resDirBrowse->setEnabled(true);
    ui->b_dir_ok->setEnabled(true);
}

void MainWindow::on_b_start_clicked()
{
    ui->l_ok->setText("Starting...");
    ui->b_start->setEnabled(false);
    ui->b_browse_dir->setEnabled(false);
    ui->b_resDirBrowse->setEnabled(false);
    ui->b_dir_ok->setEnabled(false);
   QThread *thread = new QThread;
   QCoreApplication::processEvents();
   clusterAnaliz *ca = new clusterAnaliz(Q_NULLPTR, ui->tb_directory->text(), ui->tb_result_dir->text(),
                                         ui->checkBox->isChecked(), atoi(ui->s_clNum->text().toStdString().c_str()),
                                         ui->cb_custom_func->isChecked(), ui->tb_csv_table->text());

   ca->moveToThread(thread);

   connect(thread, SIGNAL(started()), ca, SLOT(caDo()));
   connect(ca, SIGNAL(finished()), thread, SLOT(quit()));
   connect(ca, SIGNAL(statusChanged(QString)), this, SLOT(changeLbl(QString)));
   connect(ca, SIGNAL(unBlock()), this, SLOT(unBlock()));

   thread->start();
}

void MainWindow::on_cb_custom_func_clicked(bool checked)
{
    if(checked) {
        ui->b_csv_browse->setEnabled(true);
        ui->tb_csv_table->setEnabled(true);
    }
    else {
        ui->b_csv_browse->setEnabled(false);
        ui->tb_csv_table->setEnabled(false);
    }
}

void MainWindow::on_b_csv_browse_clicked()
{
    QString dir = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    ui->tb_csv_table->setText(QFileDialog::getOpenFileName(this, "CSV Table",
                                                           dir,
                                                           tr("CSV tables (*.csv);; All files(*.*)")));
}
