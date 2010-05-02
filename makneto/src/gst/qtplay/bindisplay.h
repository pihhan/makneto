
#ifndef BINDISPLAY_H
#define BINDISPLAY_H

#include <QTreeView>
#include <gst/gst.h>

class BinDisplay : public QTreeView
{
    public:
    BinDisplay(QWidget *parent = 0);

    void setBin(GstBin *bin);
};

#endif
