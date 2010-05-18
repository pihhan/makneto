
#include <QVBoxLayout>

#include "callinitwgt.h"

CallInitWidget::CallInitWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *l = new QVBoxLayout();

    l->addSpacing(20);

    m_audiobtn = new QPushButton("Initiate &audio call");
    m_videobtn = new QPushButton("Initiate &video call");


    l->addWidget(m_audiobtn);
    l->addWidget(m_videobtn);
}

void CallInitWidget::emitVideoInit()
{
}


void CallInitWidget::emitAudioInit()
{
}

void CallInitWidget::setJid(const QString &s)
{
    m_jid = s;
}

QString CallInitWidget::jid() const
{
    return m_jid;
}

