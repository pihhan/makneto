
#ifndef SELF_CONTACT_WIDGET_H
#define SELF_CONTACT_WIDGET_H

#include <QWidget>
#include "volumebar.h"
#include "gstvideowidget.h"

class SelfContactWidget : public QWidget
{
    Q_OBJECT
    public:
    SelfContactWidget();

    private:
    GstVideoWidget  *m_video;
    VolumeBar   *m_volumebar;
};

#endif

