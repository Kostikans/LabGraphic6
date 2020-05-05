#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "openglwidget.h"
#include <QLabel>
#include <QColorDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AsseptButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
