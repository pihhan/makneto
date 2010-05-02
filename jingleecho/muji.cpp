
#include "muji.h"

Muji::Muji()
    : m_state(UNKNOWN)
{
}

ContentList Muji::allContents()
{
    return m_contents;
}

States  state() const
{
    return m_state;
}

std::string Muji::room() const
{
    return m_room;
}

std::string Muji::participant() const
{
    return m_participant;
}


void Muji::setState(States s)
{
    m_state = s;
}

void Muji::setRoom(const std::string &room)
{
    m_room = room;
}

void Muji::addContent(const JingleContent &content)
{
    m_contents.push_back(content);
}

void Muji::setParticipant(const std::string &p)
{
    m_participant = p;
}

