
#include <QDebug>
#include "gstbinmodel.h"

static void gst_iterator_counter(gpointer data, gpointer user_data);


/*------*/

GstBinIndex::GstBinIndex(const QString &name, int parent) 
    : name(name), parent(parent)
{
}

GstBinIndex::GstBinIndex(GstBin *bin, int parent) 
    : parent(parent)
{
    if (bin) {
        char * cname = gst_object_get_name(GST_OBJECT(bin));
        name = QString(cname);
        qDebug() << " created index from bin: " << name;
    } else {
        g_error("NULL bin passed to GstBinIndex constructor.");
    }
}


GstBinModel::GstBinModel(GstBin *bin)
    : m_bin(bin)
{
    gst_object_ref(GST_OBJECT(m_bin));

    m_functions.push_back(C_NAME);
    m_functions.push_back(C_CURRENT_STATE);
    m_functions.push_back(C_PENDING_STATE);
    m_functions.push_back(C_HAS_SINK);
    m_functions.push_back(C_HAS_SOURCE);

    gchar *binname = gst_object_get_name(GST_OBJECT(bin));
    addIndex(GstBinIndex(binname, -1));
    g_free(binname);

    binUpdated();
}

GstBinModel::~GstBinModel()
{
    gst_object_unref(m_bin);
}

static void gst_iterator_counter(gpointer data, gpointer user_data)
{
    int *count = static_cast<int *>(user_data);
    if (data && count)
        ++(*count);
}

/** @brief return nth element from bin, or NULL if there is not enough elements.
    @param i Iterator of bin elements, from function like gst_bin_iterate_* 
    @param n Numeric index of element inside bin to get. 
    You should unref returned element, unless it is NULL. */
static GstElement *gst_iterator_get_nth(GstIterator *i, int n)
{
    int count = 0;
    GstElement *found = NULL;
    while (i) {
        GstElement *e = NULL;
        switch (gst_iterator_next(i, (void **) &e)) {
            case GST_ITERATOR_DONE:
                gst_iterator_free(i);
                i = 0;
                break;
            case GST_ITERATOR_OK:
                if (count == n) {
                    found = e;
                    gst_iterator_free(i);
                    i = 0;
                } else {
                    gst_object_unref(GST_OBJECT(e));
                    ++count;
                }
                break;
            case GST_ITERATOR_ERROR:
                g_error("Iterator error.");
                gst_iterator_free(i);
                i = 0;
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(i);
                i = 0;
                break;
                
        }
    }
    return found;
}

/** @brief Return nth element. 
    @param Bin to get element from.
    @param n Number of element to get.
    @return Element found at position, or NULL if there is no enough elements.
    You should unref returned element. */
GstElement * GstBinModel::getElement(GstBin *bin, int n) const
{
    GstElement *e = NULL;
    char *name = gst_object_get_name(GST_OBJECT(bin));
    qDebug() << "Getting data from bin " << name << " row " << n;
    g_free(name);
    GstIterator *i = gst_bin_iterate_sorted(bin);
    e = gst_iterator_get_nth(i, n);
    return e;
}

/** @brief return data for specified index and specified function type. */
QVariant GstBinModel::dataForColumn(const QModelIndex &index, enum ColumnFunctions type) const
{
    int row = index.row();
    GstElement *e = NULL;
    GstBin *bin = m_bin;
    if (index.isValid()) {
        bool getparent = (index.internalId() & BIN_FLAG);
        int filtered = ((~BIN_FLAG) & index.internalId());
        bin = getBin(filtered, getparent);
        char *name = NULL;
        if (bin)
            name = gst_object_get_name(GST_OBJECT(bin));
        qDebug() << "Getting bin #" << filtered << " parent=" << getparent
            << " bin=" << name;
        if (name)
            g_free(name);
    } else
        return QVariant();

    if (bin) {
        e = getElement(bin, row);
        if (!e)
            return QVariant();
    } else {
        qDebug() << "dataForColumn: null BIN! row " << row 
            << " id " << index.internalId();
        return QVariant();
    }

    switch (type) {
        case C_NAME:
            {
                gchar *name = gst_object_get_name(GST_OBJECT(e));
                QVariant v(name);
                g_free(name);
                gst_object_unref(e);
                return v;
            }
        case C_HAS_SINK:
            {
                int count = 0;
                GstIterator *it = gst_element_iterate_sink_pads(e);
                gst_iterator_foreach(it, gst_iterator_counter, &count);
                gst_object_unref(e);
                return QVariant(count);
            }
        case C_HAS_SOURCE:
            {
                int count = 0;
                GstIterator *it = gst_element_iterate_src_pads(e);
                gst_iterator_foreach(it, gst_iterator_counter, &count);
                gst_object_unref(e);
                return QVariant(count);
            }
        case C_CURRENT_STATE:
            {
                QVariant v(GST_STATE(e));
                gst_object_unref(e);
                return v;
            }
        case C_PENDING_STATE:
            {
                QVariant v(GST_STATE_PENDING(e));
                gst_object_unref(e);
                return v;
            }
        case C_DEBUG_NAME:
            {
                gchar *name = gst_object_get_name(GST_OBJECT(e));
                gchar *binname = gst_object_get_name(GST_OBJECT(bin));
                QString val("%1 in %2(%3)");
                QVariant v(val.arg(name).arg(binname).arg(index.internalId()));
                g_free(name);
                g_free(binname);
                gst_object_unref(e);
                return v;
            }
    }
    return QVariant();
}

void GstBinModel::setBin(GstBin *bin)
{
    m_bin = bin;
    binUpdated();
}

int GstBinModel::columnCount(const QModelIndex & parent) const
{
    return m_functions.size();
}

QString GstBinModel::getHeaderName(enum ColumnFunctions f) 
{
    switch (f) {
        case C_NAME:
            return tr("Name");
        case C_HAS_SOURCE:
            return tr("Sources");
        case C_HAS_SINK:
            return tr("Sinks");
        case C_CURRENT_STATE:
            return tr("Current");
        case C_PENDING_STATE:
            return tr("Pending");
        default:
            return QString();
    }
}

QVariant GstBinModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            if (section >= 0 && section <= (int) m_functions.size())
                return QVariant(getHeaderName(m_functions[section]));
            else
                return QVariant();
        default:
            return QVariant();
    }
}

/** @brief Get data for index and role. */
QVariant GstBinModel::data(const QModelIndex &index, int role) const
{
    enum ColumnFunctions f = C_NAME;
    if (index.column() >= 0 && index.column() <= (int) m_functions.size())
        f = m_functions[index.column()];
    switch (role) {
        case Qt::DisplayRole:
            return dataForColumn(index, f);
        case Qt::DecorationRole:
            return QVariant();
        case Qt::ToolTipRole:
        {
            QVariant v = dataForColumn(index, C_DEBUG_NAME);
            QString s = QString("%2:%1").arg(v.toString()).arg(index.row());
            return QVariant(s);
        }
        default:
            return QVariant();
    }
    return QVariant();
}


/** @brief Creates index by row, column and parent index. */
QModelIndex GstBinModel::index(int row, int column, const QModelIndex &parent) const
{
    GstBin *bin = m_bin;
    int parentid = 0;
    bool    isbin = false;

    if (parent.isValid()) {
        parentid = parent.internalId();
        isbin = (parentid & BIN_FLAG);
        parentid = ((~BIN_FLAG) & parentid);
        bin = getBin(parentid, false);
    }

    char *name = gst_object_get_name(GST_OBJECT(bin));
    qDebug() << "index get bin id=" << parentid << " parent=" << isbin
        << " name=" << name << " row=" << row;
    g_free(name);

    GstElement *e = getElement(bin, row);
    if (e) {
        if (GST_IS_BIN(e)) {
            bin = GST_BIN(e);
            char *cname = gst_object_get_name(GST_OBJECT(e));
            unsigned int newid = getIndexId(cname);
            qDebug() << "index with name " << cname 
                << " newid=" << newid 
                << " parentid=" << parentid
                << " parent valid=" << parent.isValid();
            g_free(cname);
            parentid = BIN_FLAG | newid;
        }
        gst_object_unref(e);
        return createIndex(row, column, parentid);
    } else return QModelIndex();
}

/** @brief Check if item has any children, that means if it should display
    expanding symbol for child items. */
bool GstBinModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
//        GstElement *e = static_cast<GstElement *>(parent.internalPointer());
//        bool isbin = (e && GST_IS_BIN(e));
//        gst_object_unref(GST_OBJECT(e));
        int filtered = (~BIN_FLAG) & parent.internalId();
        bool recurse = (BIN_FLAG) & parent.internalId();
        GstBin *bin = getBin(filtered);
        if (bin) {
            char *name = gst_object_get_name(GST_OBJECT(bin));
            bool isbin = (filtered>0);
            qDebug() << "hasChildren name=" << name << " isbin=" << isbin
                << " recurse=" << recurse;
            g_free(name);
            return (isbin && recurse) ;
        } else return false;
    } else 
        return (m_bin != NULL);
}

QModelIndex GstBinModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        int parentid = index.internalId();
        bool recurse = (BIN_FLAG & parentid);
        if (recurse) {
            parentid = (~BIN_FLAG) & parentid;
            if (parentid <= (int) m_indexes.size()) {
                parentid = m_indexes[parentid].parent;
                if (parentid > 0)
                    return createIndex(0, index.column(), parentid);
                else
                    return QModelIndex();
            }
        } else if (parentid > 0) {
            return createIndex(0, index.column(), parentid);
        }
        // FIXME: do i need valid parent at all?
    }
    return QModelIndex();
}

int GstBinModel::rowCount(const QModelIndex & parent) const
{
    GstBin *bin = m_bin;
    if (parent.isValid()) {
        int id = (~BIN_FLAG) & parent.internalId();
        bin = getBin(id, false);
    }
    if (bin)
        return bin->numchildren;
    else 
        return 0;
}

void GstBinModel::clearElements()
{
    for (ElementVector::iterator it=m_elements.begin(); it!= m_elements.end(); it++) {
        gst_object_unref(GST_OBJECT(*it));
    }
    m_elements.clear();
}

/** @brief Create internal state indexes, that means iterate bin and find all
    bin inside, get their names and numbers for tree hierarchy tree creation. */
void GstBinModel::binUpdated()
{
    clearElements();

    GstIterator *i = gst_bin_iterate_elements(m_bin);
    unsigned int lastIndex = 0;
    while (i) {
        GstElement *e = NULL;
        switch (gst_iterator_next(i, (void **) &e)) {
            case GST_ITERATOR_DONE:
                i = NULL;
                break;
            case GST_ITERATOR_OK:
                //m_elements.push_back(e);
                if (GST_IS_BIN(e)) {
                    GstBin *bin = GST_BIN(e);
                    lastIndex = addIndex(GstBinIndex(bin, lastIndex));
                    char *name = gst_object_get_name(GST_OBJECT(e));
                    qDebug() << "Added index " << lastIndex << " for object "
                        << name;
                    g_free(name);
                }
                gst_object_unref(e);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(i);
                break;
            case GST_ITERATOR_ERROR:
                g_error("Error at iterating bin %s", gst_object_get_name(GST_OBJECT(m_bin)));
                i = NULL;
                break;
        }
    }
}


int GstBinModel::addIndex(const GstBinIndex &index)
{
    m_indexes.push_back(index);
    return m_indexes.size()-1;
}

GstBinIndex GstBinModel::getIndex(unsigned int position) const
{
    if (position < m_indexes.size())
        return m_indexes[position];
    else
        return GstBinIndex("", 0);
}

unsigned int GstBinModel::getIndexId(const QString &name) const
{
    for (unsigned int i=0; i < m_indexes.size(); i++) {
        if (name == m_indexes[i].name)
            return i;
    }
    return 0;
}


GstBin * GstBinModel::getBin(unsigned int position, bool parent) const
{
    if (position & BIN_FLAG) {
        parent = false;
        position = ((~BIN_FLAG) & position);
    }
    if (position < m_indexes.size()) {
        if (parent && m_indexes[position].parent != -1) {
            position = m_indexes[position].parent;
        }
        return getBin(m_indexes[position]);
    } else
        return NULL;
}

/** @brief Get GstBin instance from index. 
    That means it walk thru whole bin tree, and using names from indexes,
    get single bin and find inside. */
GstBin * GstBinModel::getBin(const GstBinIndex &index) const
{
    GstBinIndex x(index);
    BinIndexVector backtree;
    while (x.parent != -1) {
        backtree.push_back(x);
        qDebug() << "pushing backtree index for " << x.name;
        x = m_indexes[x.parent];
    }

    GstBin *bin = m_bin;
    for (BinIndexVector::reverse_iterator it=backtree.rbegin(); 
            it!=backtree.rend(); 
            it++) {
        const char *name = it->name.toLocal8Bit().data();
        GstElement *e = gst_bin_get_by_name(bin, name);
        if (e) {
            if (GST_IS_BIN(e)) 
                bin = GST_BIN(e);
            gst_object_unref(GST_OBJECT(e));
        } else {
            qDebug() << "failed to get element called " << name 
                << " from bin " << index.name;
            return NULL;
        }
    }
    return bin;
}

