
#ifndef PLAYERWIDGETITEM_H
#define PLAYERWIDGETITEM_H

#include <QWidgetItem>
#include "player.h"

/** @brief Trida pro usporadani v layoutu se zachovanim aspect ratio. */
class PlayerWidgetItem : public QWidgetItem
{
    public:
    PlayerWidgetItem(Player *player);

    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth(int w) const;

    virtual QSize minimumSize() const;

    private:
    Player *m_player;

};

#endif

