
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include "incomingdialog.h"


IncomingCallDialog::IncomingCallDialog(QWidget *parent)
    : QWidget(parent)
{
    // TODO: i should use contact from roster or something like that

    m_label = new QLabel();
    m_audio = true;
    m_video = false;
    m_autoaccept = -1;

    m_accept = new QPushButton(tr("Přijmout"));
    m_reject = new QPushButton(tr("Odmítnout"));

    QVBoxLayout *mainlayout = new QVBoxLayout();
    mainlayout->addWidget(m_label);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_accept);
    buttonLayout->addWidget(m_reject);
    mainlayout->addLayout(buttonLayout);

    setLayout(mainlayout);
    if (m_autoaccept >= 0)
        QTimer::singleShot(m_autoaccept*1000, this, SLOT(accept()) );
}

void IncomingCallDialog::setAudio(bool present)
{
    m_audio = present;
    updateDescription();
}

void IncomingCallDialog::setVideo(bool present)
{
    m_video = present;
    updateDescription();
}

void IncomingCallDialog::setName(const QString &name)
{
    m_name = name;
    updateDescription();
}

void IncomingCallDialog::updateDescription()
{
    if (audio && video) {
        m_label->setText(tr("%s žádá o videohovor.").arg(m_name));
    } else if (video) {
        m_label->setText(tr("%s žádá o video sezení.").arg(m_name));
    } else if (audio) {
        m_label->setText(tr("%s žádá o audio sezení.").arg(m_name));
    }
}

