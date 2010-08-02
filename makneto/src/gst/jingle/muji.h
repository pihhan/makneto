
#ifndef MUJI_H
#define MUJI_H

#include <string>
#include "jingle-content.h"

class Muji
{
    public:
    Muji();

    enum States {
        UNKNOWN = 0,
        PREPARING,
        READY
    }

    ContentList allContents();
    States  state() const;
    std::string room() const;
    std::string participant() const;

    void setState(States s);
    void setRoom(const std::string &room);
    void addContent(const JingleContent &content);
    void setParticipant(const std::string &p);

    private:
    ContentList m_contents;
    std::string m_room;
    std::string m_participant;
    States      m_state;
};

#endif

