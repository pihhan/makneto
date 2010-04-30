
#include <QHBoxLayout>

#include "mediavideoselector.h"
#include "avparticipantwidget.h"
#include "maknetocontact.h"

MediaVideoSelector::MediaVideoSelector()
{
    m_layout = new QHBoxLayout();
    setLayout(m_layout);
}


QString MediaVideoSelector::selected() const
{
    return m_selected;
}


void MediaVideoSelector::selectUser(const QString &username)
{
    m_selected = username;
}

void MediaVideoSelector::addWidget(QWidget *w)
{
    m_layout->addWidget(w);
}

void MediaVideoSelector::createUser(MaknetoContact *contact)
{
    AVParticipantWidget *avp = new AVParticipantWidget();
    avp->setName(contact->name());
    addWidget(avp);
}

