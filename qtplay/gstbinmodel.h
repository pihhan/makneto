
#ifndef GSTBINMODEL_H
#define GSTBINMODEL_H

#include <QAbstractItemModel>
#include <vector>
#include <gst/gst.h>


/** @brief Helper structure to remember path to root of tree. */
class GstBinIndex
{
    public:
    GstBinIndex(const QString &name, int parent);
    GstBinIndex(GstBin *bin, int parent);

    QString name;
    int     parent;
};
typedef std::vector<GstBinIndex>    BinIndexVector;



class GstBinModel : public QAbstractItemModel
{
    public:
    typedef     std::vector<GstElement *>   ElementVector;
    enum ColumnFunctions {
        C_NAME,
        C_HAS_SINK,
        C_HAS_SOURCE,
        C_CURRENT_STATE,
        C_PENDING_STATE,
        C_DEBUG_NAME,
    };
    typedef     std::vector<enum ColumnFunctions>   FunctionsVector;
    const static int  BIN_FLAG = (1 << 6);

    GstBinModel(GstBin *bin);
    virtual ~GstBinModel();

    void setBin(GstBin *bin);

    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool hasChildren(const QModelIndex &parent) const;

    QVariant dataForColumn(const QModelIndex &index, enum ColumnFunctions type) const;
    static QString getHeaderName(enum ColumnFunctions f);

    GstElement * getElement(GstBin *bin, int n) const;
    void clearElements();

    private:
    void binUpdated();
    int addIndex(const GstBinIndex &index);
    GstBin *getBin(const GstBinIndex &index) const;
    GstBin *getBin(unsigned int position, bool parent=false) const;
    unsigned int getIndexId(const QString &name) const;
    GstBinIndex getIndex(unsigned int position) const;

    GstBin *m_bin;
    ElementVector  m_elements;
    FunctionsVector  m_functions;
    BinIndexVector  m_indexes;


};


#endif


