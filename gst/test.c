#include <gst/gst.h>


GstRegistry * default_register;
GList *plugins;

void iterate_features(gpointer data, gpointer user_data)
{
    GstPluginFeature *feature = (GstPluginFeature *) data;
    GstPlugin *plugin = (GstPlugin *) user_data;
    GstElementFactory * elemfac;

    elemfac = gst_element_factory_find(gst_plugin_feature_get_name(feature));
    printf("\tfeature: %s rank %d %s\n", 
        gst_plugin_feature_get_name(feature),
        gst_plugin_feature_get_rank(feature),
        (elemfac) ? "is Factory" : ""
    );
    if (elemfac) {
        gchar **uris;
        gchar *i;
        int c;
        uris = gst_element_factory_get_uri_protocols(elemfac);
        if (uris) {
        printf("\tSupported URIs: ");
        printf("%s", *uris);
        for (c=0, i=uris[c]; i; i=uris[c], ++c) {
            printf(" %s", i);
        }
        printf("\n");
        }
        
    }
}

void iterate_plugins(gpointer data, gpointer user_data)
{
    GstPlugin *plugin = (GstPlugin *) data;
    GstRegistry *reg = (GstRegistry *) user_data;
    GList * features;

    printf("plugin: %s version: %s license: %s\n", 
        gst_plugin_get_name(plugin),
        gst_plugin_get_version(plugin),
        gst_plugin_get_license(plugin) 
    );
    features = gst_registry_get_feature_list_by_plugin(reg, gst_plugin_get_name(plugin));
    g_list_foreach(features, iterate_features, plugin);
    gst_plugin_feature_list_free(features);

}

void iterate_registry(GstRegistry *registry)
{
    plugins = gst_registry_get_plugin_list(default_register);
    g_list_foreach(plugins, iterate_plugins, default_register);
    gst_plugin_list_free(plugins);
}

int main(int argc, char *argv[])
{
    GstPlugin * plugin;

    gst_init(&argc, &argv);
    default_register = gst_registry_get_default();
    iterate_registry(default_register);
    

    return 0;
}
