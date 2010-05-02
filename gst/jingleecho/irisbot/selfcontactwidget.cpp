
#include <QLabel>
#include <QVBoxLayout>

#include "selfcontactwidget.h"


SelfContactWidget::SelfContactWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    m_volumebar = new VolumeBar();
    m_video = new GstVideoWidget();

    QLabel *label = new QLabel(tr("Self contact"));
    layout->addWidget(m_video);
    layout->addWidget(m_volumebar);
    setLayout(layout);
}


