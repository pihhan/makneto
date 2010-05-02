
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include "mainwindow.h"
#include "qspectrum.h"
#include "qpipeline.h"


MainWindow::MainWindow(QSpectrum *spectrum, QPipeline *pipe)
    : QWidget(), m_pipeline(pipe), m_playing(false)
{
    m_spectrumwidget = new QSpectrumWidget();
    m_spectrumwidget->setSpectrum( spectrum );
    m_playbutton = new QPushButton("Play/Stop");

    m_timebox = new QSpinBox();
    m_timebox->setRange(50, 100000);
    m_timebox->setValue(1000);
    m_timebox->setSingleStep(50);
    connect(m_timebox, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));

    m_alphabox = new QSpinBox();
    m_alphabox->setRange(0, 100);
    m_alphabox->setValue(30);
    connect(m_alphabox, SIGNAL(valueChanged(int)), this, SLOT(alphaChanged(int)));

    QLabel *l_timeout = new QLabel(tr("Timeout: "));
    QLabel *l_alpha = new QLabel(tr("Alpha: "));

#ifdef BOXLAYOUT
    QHBoxLayout *l = new QHBoxLayout(this);
    l->addWidget(m_playbutton);
    l->addWidget(m_spectrumwidget);
#else
    QGridLayout *l = new QGridLayout(this);
    l->addWidget(m_spectrumwidget, 0, 0, 4, 5);
    l->addWidget(m_playbutton, 4, 0);
    l->addWidget(l_alpha, 4, 1);
    l->addWidget(m_alphabox, 4, 2);
    l->addWidget(l_timeout, 4, 3);
    l->addWidget(m_timebox, 4, 4);
#endif
    setLayout(l);

    connect(m_playbutton, SIGNAL(clicked()), this, SLOT(playPause()) );
}


void MainWindow::playPause()
{
    if (m_pipeline) {
        if (m_playing) {
            m_pipeline->setState(GST_STATE_PAUSED);
        } else {
            m_pipeline->setState(GST_STATE_PLAYING);
        }
        m_playing = ! m_playing;
    }
}

void MainWindow::alphaChanged(int value)
{
    float fraction = value / 100.0;
    m_spectrumwidget->spectrum()->setAlpha(fraction);
}

void MainWindow::timeoutChanged(int value)
{
    m_spectrumwidget->setTimeout(value);
}


