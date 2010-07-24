
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KIcon>

#include <kfiledialog.h>

#include "soundfiletester.h"

SoundFileTester::SoundFileTester(MediaManager *manager, QWidget *parent)
    : QWidget(parent), m_manager(manager)
{
    QFormLayout *l = new QFormLayout();
    setLayout(l);

    edt_filename = new QLineEdit();

    l->addRow(new QLabel(tr("Filename:")), edt_filename);

    btn_play = new QPushButton(KIcon("player_play"), tr("&Play"));
    connect(btn_play, SIGNAL(clicked()), this, SLOT(play()) );
    btn_stop = new QPushButton(KIcon("player_stop"), tr("&Stop"));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(stop()) );
    btn_find = new QPushButton(KIcon("filefind"), tr("&Find"));
    connect(btn_find, SIGNAL(clicked()), this, SLOT(find()) );


    QHBoxLayout *btnlayout = new QHBoxLayout();
    btnlayout->addWidget( btn_play );
    btnlayout->addWidget( btn_stop );
    btnlayout->addWidget( btn_find );

    l->addRow(new QWidget(), btnlayout);

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
    m_manager->player()->playFile(edt_filename->text() );
}

void SoundFileTester::stop()
{
    reportMsg("Neumime stop.");
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


void SoundFileTester::reportMsg(const QString &msg)
{
    lbl_status->setText(msg);
}



void SoundFileTester::playerStarted()
{
    reportMsg("Player begun playing.");
}

void SoundFileTester::playerStopped()
{
    reportMsg("Player finished.");
}

