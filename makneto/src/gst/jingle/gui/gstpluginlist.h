
#ifndef GST_PLUGIN_LIST_WIDGET_H
#define GST_PLUGIN_LIST_WIDGET_H

#include <gst/gst.h>
#include <QWidget>
#include <QListWidget>
#include <QAbstractListModel>

class GstPluginListModel : public QAbstractListModel
{
    Q_OBJECT
    public:
    GstPluginListModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex() ) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex() ) const;

    private:
    void populate();

    QStringList m_plugin_names;
};

class GstPluginListWidget : public QObject
{
    Q_OBJECT
    public:
};

#endif

