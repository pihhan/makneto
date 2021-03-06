
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KIcon>
#include <cstdio>

#include <kfiledialog.h>

#include "soundfiletester.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QStringList>
#include <QStringListModel>
#include "gstpluginlist.h"

SoundFileTester::SoundFileTester(MediaManager *manager, QWidget *parent)
    : QWidget(parent), m_manager(manager)
{
    QFormLayout *l = new QFormLayout();
    setLayout(l);
	setWindowTitle(i18n("Sound Player"));

    edt_filename = new QLineEdit("/usr/share/sounds/k3b_success1.wav");
	
	spin_repeats = new QSpinBox();
	spin_repeats->setValue(1);
	connect(spin_repeats, SIGNAL(valueChanged(int)), manager->player(), SLOT(setRepeats(int)) );
	
	spin_delay = new QSpinBox();
	spin_delay->setValue(1000);
	spin_delay->setMaximum(10000);
	spin_delay->setSingleStep(50);
	spin_delay->setSuffix(" ms");
	connect(spin_delay, SIGNAL(valueChanged(int)), manager->player(), SLOT(setDelay(int)) );

    l->addRow(new QLabel(i18n("&Filename:")), edt_filename);

    btn_play = new QPushButton(KIcon("media-playback-start"), tr("&Play"));
    connect(btn_play, SIGNAL(clicked()), this, SLOT(play()) );
	btn_pause = new QPushButton(KIcon("media-playback-pause"), tr("Pa&use"));
	connect(btn_pause, SIGNAL(clicked()), this, SLOT(pause()) );
    btn_stop = new QPushButton(KIcon("media-playback-stop"), tr("&Stop"));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(stop()) );
    btn_find = new QPushButton(KIcon("document-open"), tr("&Locate"));
    connect(btn_find, SIGNAL(clicked()), this, SLOT(find()) );
	btn_display = new QPushButton(tr("&Display"));
	connect(btn_display, SIGNAL(clicked()), this, SLOT(displayPipeline()) );

	l->addRow(new QLabel(i18n("Repeats")), spin_repeats);
	l->addRow(new QLabel(i18n("Delay")), spin_delay);
	
    QHBoxLayout *btnlayout = new QHBoxLayout();
    btnlayout->addWidget( btn_play );
	btnlayout->addWidget( btn_pause);
    btnlayout->addWidget( btn_stop );
    btnlayout->addWidget( btn_find );
	btnlayout->addWidget( btn_display );

	QHBoxLayout *btn2layout = new QHBoxLayout();
	btn_ringing = new QPushButton(tr("Ringing"));
	connect(btn_ringing, SIGNAL(clicked()), m_manager, SLOT(startRingIsRinging()) );
	btn_incoming = new QPushButton(tr("Incoming"));
	connect(btn_incoming, SIGNAL(clicked()), m_manager, SLOT(startRingIncomingCall()) );
	btn_terminated = new QPushButton(tr("Terminated"));
	connect(btn_terminated, SIGNAL(clicked()), m_manager, SLOT(startRingTerminated()) );
	btn_busy = new QPushButton(tr("Busy"));
	connect(btn_busy, SIGNAL(clicked()), m_manager, SLOT(startRingBusy()) );
	btn2layout->addWidget(btn_ringing);
	btn2layout->addWidget(btn_incoming);
	btn2layout->addWidget(btn_terminated);
	btn2layout->addWidget(btn_busy);
	
    l->addRow(new QWidget(), btnlayout);
	l->addRow(new QWidget(), btn2layout);

    lbl_status = new QLabel();
    l->addRow(new QWidget(), lbl_status);


    QtAudioPlayer *player = m_manager->player();
    connect(player, SIGNAL(error(QString)), this, SLOT(reportMsg(QString)) );
    connect(player, SIGNAL(warning(QString)), this, SLOT(reportMsg(QString)) );
    connect(player, SIGNAL(started()), this, SLOT(playerStarted()) );
    connect(player, SIGNAL(finished()), this, SLOT(playerStopped()) );

}

void SoundFileTester::play()
{
    reportMsg("Playing started.");
    bool success = m_manager->player()->playFile(edt_filename->text() );
    if (!success)
        reportMsg("Playing failed!");
}

void SoundFileTester::stop()
{
    m_manager->player()->stop();
    reportMsg("Stopped.");
}

void SoundFileTester::pause()
{
	bool success = m_manager->player()->setFile(edt_filename->text() );
	m_manager->player()->pause();
	reportMsg(success ? "Paused." : "Pause failed.");
}

void SoundFileTester::find()
{
    QString file = KFileDialog::getOpenFileName(KUrl("file:///usr/share/sounds"));

    if (!file.isEmpty()) {
        edt_filename->setText(file);
    } else {
        edt_filename->setText(QString());
    }
}

/** \brief Convert list of strings from glib to Qt. */
QStringList glibToQtStringList(GList *list, bool free_it)
{
	QStringList qlist;
	for (GList *i = g_list_first(list); i; i = g_list_next(i)) {
		gchar * text = (gchar *) i->data;
		qlist << QString(text);
	}
	if (free_it)
		g_list_free(list);
	return qlist;
}

void showPlugins()
{
	QWidget *widget = new QWidget();
	QVBoxLayout *l = new QVBoxLayout();
	widget->setLayout(l);
	
	QListView *view = new QListView();
	GstPluginListModel *model = new GstPluginListModel();
	view->setModel(model);
	l->addWidget(new QLabel("Plugins"));
	l->addWidget(view);
	
	QListView * pathview = new QListView();
	GstRegistry *registry = gst_registry_get_default();
	GList *paths = gst_registry_get_path_list(registry);
	QStringListModel *pathmodel = new QStringListModel(glibToQtStringList(paths, true));
	pathview->setModel(pathmodel);
	l->addWidget(new QLabel("Paths"));
	l->addWidget(pathview);
	
	widget->setVisible(true);
}

void SoundFileTester::displayPipeline() 
{
	QString pipelinetext = QString::fromStdString(m_manager->player()->pipeline->describe());
	reportMsg(pipelinetext);
	qDebug() << pipelinetext;
	
	QtAudioPlayer *player = m_manager->player();
	GstElement *pipe = player->pipeline->element();
	//gst_xml_write_file(pipe, stderr);
	FILE *debugpipe = NULL;
	debugpipe = fopen("debugpipe.xml", "w");
	if (debugpipe) {
		gst_xml_write_file(pipe, debugpipe);
		fclose(debugpipe);
	}
	
	GstPad *srcpad = player->pipeline->getAudioSourcePad();
	if (srcpad) {
		qDebug() << QString::fromStdString(QPipeline::describePad(srcpad));
		GString *s = g_string_new(NULL);
		gst_print_pad_caps(s, 0, srcpad);
		qDebug() << s->str;
		gst_object_unref(srcpad);
		g_string_free(s, TRUE);
	}
	GstPad *sinkpad = player->pipeline->getAudioSinkPad();
	if (sinkpad) {
		qDebug() << QString::fromStdString(QPipeline::describePad(sinkpad));
		GString *s = g_string_new(NULL);
		gst_print_pad_caps(s, 0, sinkpad);
		qDebug() << s->str;
		gst_object_unref(sinkpad);
		g_string_free(s, TRUE);
		
	}
	gst_object_unref(pipe);
	
#if 0
	showPlugins();
#endif
}

void SoundFileTester::reportMsg(const QString &msg)
{
    lbl_status->setText(msg);
	qDebug() << "Message: " << msg;
}



void SoundFileTester::playerStarted()
{
    reportMsg("Player begun playing.");
}

void SoundFileTester::playerStopped()
{
    reportMsg("Player finished.");
}

