
#include <iostream>
#include <gst/gst.h>
#include <cstring>

#include "client.h"

using namespace gloox;

GMainLoop *loop = NULL;

char *jid = NULL;
char *password = NULL;

bool read_config(const char *path)
{
    GKeyFile *keyfile = NULL;
    keyfile = g_key_file_new();
    GError *err = NULL;
    bool r = false;
    const char groupname[] = "xmpp";

    r = g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, &err);
    if (r) {
            if (g_key_file_has_group(keyfile, groupname)) {
                if (g_key_file_has_key(keyfile, groupname, "jid", &err)) {
                    jid = g_key_file_get_string(keyfile, groupname, "jid", &err);
                }
                if (g_key_file_has_key(keyfile, groupname, "password", &err)) {
                    password = g_key_file_get_string(keyfile, groupname, "password", &err);
                }
            }
    } else {
        if (err)
            std::cerr << path << ": " << err->message << std::endl;
    }
    return r;
}


/** @brief Periodically called function to read incoming data from gloox 
    connections. */
static gboolean 
gloox_receive_function(gpointer user_data)
{
    EchoClient *client = (EchoClient *) user_data;
    if (client) {
        ConnectionError e = client->recv(50);
        if (e != ConnNoError) {
            g_main_loop_quit(loop);
            return FALSE;
        }
    }
    return TRUE;
}

void print_help(const char *myname)
{
    std::cout << "Spusteni programu:" << std::endl;
    std::cout << myname << " <jid> <password>" << std::endl;
    std::cout << myname << " -f <config.ini>" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        print_help(argv[0]);
        return 1;
    }
    if (!strcmp(argv[1], "-f")) {
        if (!read_config(argv[2])) {
            std::cerr << "Chyba pri nacitani nastaveni z " << argv[1] << std::endl;
            return 1;
        }
    } else {
        jid = argv[1];
        password = argv[2];
    }

    if (!jid || !password) {
        std::cerr << "Pristupove udaje nejsou znamy" << std::endl;
    }

    gst_init(&argc, &argv);

    loop = g_main_loop_new(g_main_context_default(), false);

    EchoClient client(jid, password);
    client.connect();

    g_idle_add(gloox_receive_function, &client);

    g_main_loop_run(loop);

    std::cout << "quitting." << client.authError() <<  std::endl;

    return 0;
}
