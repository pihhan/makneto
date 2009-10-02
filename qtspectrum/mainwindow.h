#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class MainWindow : public QWidget
{
    public:
    MainWindow(QObject *parent=0);

    void createPipeline();

};

#endif


