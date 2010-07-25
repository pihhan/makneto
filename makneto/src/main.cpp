#include "maknetomainwindow.h"
#include "makneto.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include <QtCrypto>
#include <QDebug>
#include <QTextCodec>

#include <gst/gst.h>

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

	g_thread_init(NULL);
        
        GError *err =NULL;

        /* Parse glib/gst options, ignore unknown parameters.
        This will remove parameters supported by gstreamer 
        from parameters. */
        GOptionContext *ctx = g_option_context_new("");
        g_option_context_set_help_enabled(ctx, FALSE);
        g_option_context_set_ignore_unknown_options(ctx, TRUE);
        GOptionGroup *gst_group = gst_init_get_option_group();
        g_option_context_add_group(ctx, gst_group);

        gchar *gsthelptext = g_option_context_get_help(ctx, FALSE, gst_group);
        KCmdLineOptions gstopts;
        gstopts.add(QByteArray("gst"), ki18n(gsthelptext));
        g_free(gsthelptext);

        if (g_option_context_parse(ctx, &argc, &argv, &err)) {
            qDebug("GStreamer cmdline parameters parsed succesfully.");
        } else {
            qWarning() << "GStreamer cmdline parameters failed to parse: "
                    << err->message;
            g_error_free(err);
        }
		
		gst_init(NULL, NULL);

        /** Let KDE parser parse all internal options not removed by
        gstreamer parsing. */
        KCmdLineArgs::init(argc, argv, &about);
        KCmdLineArgs::addCmdLineOptions(gstopts, ki18n("gst"));

		/* Expect strings from C are in UTF-8 format, as is common for 
		   GStreamer and GLib applications. */
		QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

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
