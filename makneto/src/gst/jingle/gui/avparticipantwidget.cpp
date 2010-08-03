
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>

#include "avparticipantwidget.h"
#include "aspectedlayout.h"
#include "avoutput.h"


AVParticipantWidget::AVParticipantWidget(QWidget *parent)
    : QFrame(parent), m_selected(false)
{
    QVBoxLayout *layout = new QVBoxLayout();

    m_name = new QLabel();
    m_video = new GstVideoWidget();

    AspectedLayout *videolayout = new AspectedLayout();
    videolayout->setCentralWidget(m_video);

    m_volumebar = new VolumeBar();

    layout->addWidget(m_name);
    layout->addLayout(videolayout);
    layout->addWidget(m_volumebar);

    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLayout(layout); 
}

AVParticipantWidget::~AVParticipantWidget()
{
}

QString AVParticipantWidget::name() const
{
    return m_name->text();
}

void AVParticipantWidget::setName(const QString &name) 
{
    m_name->setText(name);
}


void AVParticipantWidget::prepareWindowId()
{
    // noop, window is now created with widget itself.
    WId winid = m_video->winId();
    QApplication::syncX();
    emit videoWindowIdChanged(winid);
}

void AVParticipantWidget::videoResolutionChanged(const FrameSize &size)
{
    m_video->setVideoSize(QSize(size.width, size.height));
}

void AVParticipantWidget::setWindowId(unsigned long id)
{
    if (output())
        output()->setWindowId(id);
}

void AVParticipantWidget::handleExpose()
{
    if (output())
        output()->expose();
}

void AVParticipantWidget::updateVolumes(int channels, double *rms, double *peak, double *decay)
{
    m_volumebar->updateVolumes(channels, rms, peak, decay);
}

void AVParticipantWidget::updateMessage(GstMessage *msg)
{
    
}

void AVParticipantWidget::setSelected(bool s)
{
    m_selected = s;
}


bool AVParticipantWidget::selected() const
{
    return m_selected;
}

/** @brief Slot connected to video expose signal. */
void AVParticipantWidget::videoExposed()
{
    handleExpose();
}


void AVParticipantWidget::videoResolutionChanged(const QSize &size)
{
    m_video->setVideoSize(size);
}


void AVParticipantWidget::displayVolume(int volume)
{
    double dv = volume / 100.0;
    double zero = 0.0;
    m_volumebar->updateVolumes(1, &dv, &zero, &zero);
}

void AVParticipantWidget::setVolume(int volume)
{
    double dv = volume / 100.0;
    setVolume(dv);
}

void AVParticipantWidget::setVolume(double volume)
{
    // TODO
}

void AVParticipantWidget::videoDisabled()
{
    m_video->setPlaying(false);
}

void AVParticipantWidget::videoEnabled()
{
    m_video->setPlaying(true);
}

void AVParticipantWidget::audioDisabled()
{
    m_volumebar->setEnabled(false);
}

void AVParticipantWidget::audioEnabled()
{
    m_volumebar->setEnabled(true);
}


