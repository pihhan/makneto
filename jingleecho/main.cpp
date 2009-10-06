
#include <iostream>
#include "client.h"

const char *jid;
const char *password;

using namespace gloox;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Spusteni: " << argv[0] << " <jid> <password>" << std::endl;
        return 1;
    }

    EchoClient client(argv[1], argv[2]);
    client.connect();

    std::cout << "quitting." << client.authError() <<  std::endl;

    return 0;
}
