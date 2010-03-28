
#include <QBoxLayout>
#include <QVBoxLayout>

#include "avparticipantwidget.h"
#include "aspectedlayout.h"


AVParticipantWidget::AVParticipantWidget(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    m_name = new QLabel();
    m_video = new GstVideoWidget();

    AspectedLayout *videolayout = new AspectedLayout();
    videolayout->setCentralItem(m_video);

    m_volumebar = new VolumeBar();

    layout->addWidget(m_name);
    layout->addLayout(videolayout);
    layout->addWidget(m_volumebar);
    
}

QString AVParticipantWidget::name() const
{
    return m_name->text();
}

void AVParticipantWidget::setName(const QString &name) 
{
    m_name->setText(name);
}


void AVParticipant::prepareWindowId()
{
    // noop, window is now created with widget itself.
}

void AVParticipant::videoResolutionChanged(const FrameSize &size)
{
    m_video->setVideoSize(QSize(size.width, size.height));
}

void AVParticipant::setWindowId(unsigned long id)
{
    if (output())
        output()->setWindowId(id);
}

void AVParticipantWidget::handleExpose()
{
    if (output())
        output()->expose();
}


void AVParticipantWidget::updateVolumes(
    int channels,
    double *rms,
    double *peak,
    double *decay)
{
    for (int channel=0; channel < channels; channel++) {
        m_volumebar
    }
}

void AVParticipantWidget::prepareWindowId()
{
    // noop
}

void AVParticipantWidget::videoResolutionChanged(const FrameSize &size)
{
    m_video->setVideoSize(QSize(size.width, size.height));
}

void AVParticipantWidget::setWindowId(unsigned long id)
{
    m_output->setWindowId(id);
}

void AVParticipantWidget::handleExpose()
{
    m_output->expose();
}

void AVParticipantWidget::updateVolumes(int channels, double *rms, double *peak, double *decay)
{
    m_volumebar->updateVolumes(channels, rms, peak, decay);
}

