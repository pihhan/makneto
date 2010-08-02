
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

#if 0
void AVParticipantWidget::updateVolumes(
    int channels,
    double *rms,
    double *peak,
    double *decay)
{
    for (int channel=0; channel < channels; channel++) {
        m_volumebar->updateVolumes(channels, rms, peak, decay);
    }
}
#endif

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

}

void AVParticipantWidget::setVolume(int volume)
{
}

void AVParticipantWidget::setVolume(double volume)
{
}

