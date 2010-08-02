
#include "gstpluginlist.h"

GstPluginListModel::GstPluginListModel()
{
    populate();
}

QVariant GstPluginListModel::data(const QModelIndex &index, int role) const
{
    return QVariant(m_plugin_names.at(index.row()) );
}

QModelIndex GstPluginListModel::index(int row, int column, const QModelIndex &parent ) const
{
    return createIndex(row, column, 0);
}

int GstPluginListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int GstPluginListModel::rowCount(const QModelIndex &parent) const
{
    return m_plugin_names.size();
}


void GstPluginListModel::populate()
{
    m_plugin_names.clear();

    GstRegistry * registry = gst_registry_get_default();
    GList * g_plugins = gst_registry_get_plugin_list(registry);
    for (GList *i = g_list_first(g_plugins); i; i = g_list_next(i)) {
        GstPlugin *plugin = (GstPlugin *) i->data;
        m_plugin_names << QString(gst_plugin_get_name(plugin));

        gst_object_unref(plugin);
    }
    reset();
}

