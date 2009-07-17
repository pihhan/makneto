#include "maknetomainwindow.h"
#include "makneto.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include <QtCrypto>
#include <QDebug>

static const char description[] = I18N_NOOP("A KDE 4 collaborative software");

static const char version[] = "0.1";

int main(int argc, char **argv)
{	
	QCA::Initializer init;

	KAboutData about("makneto", 0, ki18n("Makneto"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 Jaroslav Reznik"), KLocalizedString(), 0, "rezzabuh@gmail.com");
	about.addAuthor( ki18n("Jaroslav Reznik"), KLocalizedString(), "rezzabuh@gmail.com" );
  about.addAuthor( ki18n("Radek Novacek"), KLocalizedString(), "rad.n@centrum.cz" );
  about.addAuthor( ki18n("Petr Menšík"), KLocalizedString(), "pihhan@cipis.net" );
	KCmdLineArgs::init(argc, argv, &about);

	KApplication app;
  app.setWindowIcon(KIcon("makneto"));

	// Makneto main
	Makneto *makneto = new Makneto;
	MaknetoMainWindow *mainWindow = new MaknetoMainWindow(makneto);
  makneto->setMaknetoMainWindow(mainWindow);
    
	mainWindow->show();

        app.setQuitOnLastWindowClosed(true);
        if (app.quitOnLastWindowClosed()) {
            qDebug() << " will quit on close";
        }

	return app.exec();
}
