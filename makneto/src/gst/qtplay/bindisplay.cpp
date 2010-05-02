
#include "gstbinmodel.h"
#include "bindisplay.h"

BinDisplay::BinDisplay(QWidget *parent)
    : QTreeView(parent)
{

}


void BinDisplay::setBin(GstBin *bin)
{
    GstBinModel *model = new GstBinModel(bin);
    setModel(model);
    // TODO: delete?
}
