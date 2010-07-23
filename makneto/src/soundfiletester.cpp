
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KIcon>
#include "soundfiletester.h"

SoundFileTester::SoundFileTester(MediaManager *manager, QWidget *parent)
    : QWidget(parent), m_manager(manager)
{
    QFormLayout *l = new QFormLayout();
    setLayout(l);

    edt_filename = new QLineEdit();

    l->addRow(new QLabel(tr("Filename:")), edt_filename);

    btn_play = new QPushButton(KIcon("play"), tr("&Play"));
    btn_stop = new QPushButton(KIcon("stop"), tr("&Stop"));
    btn_find = new QPushButton(tr("&Find"));

    QHBoxLayout *btnlayout = new QHBoxLayout();
    btnlayout->addWidget( btn_play );
    btnlayout->addWidget( btn_stop );
    btnlayout->addWidget( btn_find );

    l->addRow(new QWidget(), btnlayout);

}

void SoundFileTester::play()
{
}

void SoundFileTester::stop()
{
}

void SoundFileTester::find()
{
}

