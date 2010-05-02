#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>

#include "qpipeline.h"
#include "qspectrum.h"
#include "qspectrumwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    public:
    MainWindow(QSpectrum *spectrum, QPipeline *pipe);

    public slots:
    void playPause();
    void timeoutChanged(int value);
    void alphaChanged(int value);

    private:
    QSpectrumWidget *m_spectrumwidget;
    QPipeline *m_pipeline;
    QPushButton *m_playbutton;
    QSpinBox    *m_alphabox;
    QSpinBox    *m_timebox;
    bool    m_playing;

};

#endif


