
#include <cmath>
#include "playerwidgetitem.h"

PlayerWidgetItem::PlayerWidgetItem(Player *player)
    : QWidgetItem(player), m_player(player)
{
}

bool PlayerWidgetItem::hasHeightForWidth() const
{
    QSize size = m_player->videoSize();
    return (size.width() > 0 && size.height() > 0);
}

int PlayerWidgetItem::heightForWidth(int w) const
{
    QSize size = m_player->videoSize();
    if (size.height() > 0 && size.width() > 0) {
        double ratio = ((double) size.height()) / size.width();
        double height = ratio * w;
        return ceil(height);
    }
    return size.height();
}

QSize PlayerWidgetItem::minimumSize() const
{
    return m_player->videoSize();
}

