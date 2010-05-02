
#include "logger.h"

using namespace std;

int main(int argc, char *argv[])
{
    Logger l;

    l << "This is a test string" << "with numbers" << 12345 << endl;
    LOGGER(l) << "Velmi kriticka chyba" << endl;
    l.printf("Toto je zapisek pomoci printf: %s %04x", "slava", 56);
    LOGGER(l) << "Velmi kriticka chyba " << "podruhe" << endl;
    LOGGER(l).printf("Aha, ted jsem te dostal, %s", "parchante");
    LOGGER(l) << "Ze?" << endl;
    return 0;
}


