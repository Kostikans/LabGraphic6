#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_AsseptButton_clicked()
{
    QVector3D vec(ui->SpinForX->value(),ui->SpinForY->value(),ui->spinForZ->value());
    ui->openGLWidget->addElement(vec);
}
