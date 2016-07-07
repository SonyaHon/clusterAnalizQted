#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clusteranaliz.h"
#include <iostream>
#include <string>
#include <QString>
#include <QFileDialog>
#include <QDir>

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

void MainWindow::on_b_dir_ok_clicked()
{
    QString dirName, resDirName;
    dirName = ui->tb_directory->text();
    resDirName = ui->tb_result_dir->text();

    if(QDir(dirName).exists() && QDir(resDirName).exists()) {
        ui->l_ok->setText("OK");
        ui->b_start->setEnabled(true);
    }
    else if(QDir(dirName).exists() && !QDir(resDirName).exists())
        ui->l_ok->setText("No result folder");
    else if(!QDir(dirName).exists() && QDir(resDirName).exists())
        ui->l_ok->setText("No files folder");
}

void MainWindow::on_b_start_clicked()
{
    clusterAnaliz ca = clusterAnaliz(ui->tb_directory->text());
    ca.caDo(atoi(ui->s_clNum->text().toStdString().c_str()), ui->tb_result_dir->text(), ui->tb_directory->text(), ui->checkBox->checkState());
}

void MainWindow::on_b_browse_dir_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory();
    dirName += "/";
    ui->tb_directory->setText(dirName);
}

void MainWindow::on_b_resDirBrowse_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory();
    ui->tb_result_dir->setText(dirName);
}



void MainWindow::on_tb_directory_textEdited(const QString &arg1)
{
    ui->l_ok->setText("select result folder");
}


void MainWindow::on_tb_result_dir_textEdited(const QString &arg1)
{
    ui->l_ok->setText("press ok to check folders");
}
