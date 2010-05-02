#include <Qt>
#include <QApplication>
#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>
#include <QComboBox>
#include <QDebug>
#include <QPalette>
#include <QSpacerItem>

#include <QPaintEngine>
#include <QPaintEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QScrollArea>

#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <QTabWidget>


#include "player.h"
#include "playerwidgetitem.h"
#include "mainwin.h"
#include "aspectedlayout.h"
#include "volumebar.h"
#include "bindisplay.h"
#include "cpuwidget.h"

Mainwin::Mainwin(Player *player, QWidget *w) 
            : QWidget(w)
{
    layout1= new QBoxLayout(QBoxLayout::TopToBottom);
    label = new QLabel("Player:");
    //label->setMaximumHeight(80);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    button = new QPushButton("Handles");
    btn2 = new QPushButton("Play");
    path = new QLineEdit("/home/pihhan/Videa/EVO/evo-2009-03-02-1.ogg");
    select = new QPushButton("Select");
    pausebtn = new QPushButton("Pause");


#if 0
    PlayerWidgetItem *pwitem = new PlayerWidgetItem(player);
    QBoxLayout *playerLayout = new QHBoxLayout();
    playerLayout->addWidget(new QWidget());
    playerLayout->addItem(pwitem);
    playerLayout->addWidget(new QWidget());
#else
    AspectedLayout *playerLayout = new AspectedLayout();
    playerLayout->setCentralWidget(player);
#endif

    this->player = player;

    resolutions = new QComboBox();
    connect( resolutions, SIGNAL(currentIndexChanged(int)), this, SLOT(resolutionBoxChanged(int)) );

    connect( button, SIGNAL(clicked()), player, SLOT(displayHandle()));
    connect( button, SIGNAL(clicked()), this, SLOT(displayHandle()));

    gsttest = new GstTest(true);

    connect ( btn2, SIGNAL(clicked()), this, SLOT(startplayer()));
    connect( pausebtn, SIGNAL(clicked()), this, SLOT(pauseplayer()) );
    connect( select, SIGNAL(clicked()), this, SLOT(selectFile()));

    VolumeBar *bar = new VolumeBar();
    bar->setValue(50);
    gsttest->watchers.registerWatcher(bar, gsttest->level);

    QPushButton *displayBinBtn = new QPushButton("Display bin...");
    connect(displayBinBtn, SIGNAL(clicked()), this, SLOT(displayBin()) );

    QVBoxLayout *layout2 = new QVBoxLayout();

    layout1->addWidget(label);
    layout1->addLayout(playerLayout);
    layout1->addWidget(path);
    layout1->addWidget(button);
    layout1->addWidget(select);
    layout1->addWidget(btn2);
    layout1->addWidget(pausebtn);

    CpuWidget *cpu = new CpuWidget();

    layout2->addWidget(resolutions);
    layout2->addWidget(bar);
    layout2->addWidget(displayBinBtn);

    layout2->addWidget(cpu);


    QTabWidget *tabw = new QTabWidget();

    QWidget *w1 = new QWidget();
    w1->setLayout(layout1);

    QScrollArea *area = new QScrollArea();
    area->setWidget(w1);
    QWidget *w2 = new QWidget();
    w2->setLayout(layout2);

    tabw->addTab(area, "Video");
    tabw->addTab(w2, "Audio");
    
    new QVBoxLayout(this);
    layout()->addWidget(tabw);



    
    setLocation(path->text());

    probeResolutions();
}

void Mainwin::startplayer()
{
    //gsttest->pause();
    QSize videosize(gsttest->negotiatedWidth(), gsttest->negotiatedHeight());
    if (videosize.width() > 0 && videosize.height() > 0) {
        player->setVideoSize(videosize);
    }
    gsttest->togglePlay();
    player->setPlaying(true);
}

void Mainwin::pauseplayer()
{
    gsttest->pause();
    player->setPlaying(false);
}

void Mainwin::setLocation(const QString &path)
{
    this->path->setText(path);
    QByteArray array = path.toUtf8();
    gsttest->setLocation(array.data());
    gsttest->setXID(player->winId());
}

void Mainwin::selectFile()
{
    QString nowpath = QFileDialog::getOpenFileName(this, "vyber Ogg soubor");
    qDebug() << "selected file: " << nowpath << endl;
    setLocation(nowpath);
    emit fileSelected(path->text());
}

void Mainwin::displayX11info(const QX11Info &info)
{
    qDebug() << " X11 info: " << endl;
    qDebug() << "depth " << info.depth() << " screen " << info.screen() 
        << " cells " << info.cells() << endl;

    qDebug() << "root window " << QX11Info::appRootWindow() << endl;
    Display *display = QX11Info::display();
    Window rootwin;
    int x, y;
    unsigned int width, height;
    unsigned int border, depth;

    Status ret = XGetGeometry(display, QX11Info::appRootWindow(), &rootwin,
            &x, &y, &width, &height, &border, &depth);
    if (ret == Success) {
        qDebug() << "position " << x << "," << y << " size " << width 
        << "," << height << " border " << border << " depth " << depth << endl;
    } else {
        qWarning() << "Failed to get X11 geometry; ret=" << ret;
    }
}

void Mainwin::displayHandle()
{
    qDebug() << "Mainwin handle " << x11PictureHandle() << " winid " << winId();

    Display *display = QX11Info::display();
    Window rootwin;
    int x, y;
    unsigned int width, height;
    unsigned int border, depth;

    qDebug() << "Display is " << DisplayString(display);

    Status ret = XGetGeometry(display, winId(), &rootwin,
            &x, &y, &width, &height, &border, &depth);
    if (true || ret == Success) {
        qDebug() << "position " << x << "," << y << " size " << width 
        << "," << height << " border " << border << " depth " << depth << endl;
    } else {
        qWarning() << "Failed to get X11 geometry; ret=" << ret;
    }

    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, winId(), &attrs) == Success) {
        qDebug() << "X11 attrs: root=" << attrs.root << " visual=" << attrs.visual
                << " position=[" << attrs.x << "," << attrs.y;
    } else {
        qWarning() << "Failed to get X11 attributes";
    }

    displayX11info(x11Info());

    qDebug() << "Player size: " << player->width() << "x" << player->height();

    gsttest->printSourcePads();
    gsttest->enumerateDevices(gsttest->source);
    gsttest->enumerateDevices(gsttest->audiosrc);
}

void Mainwin::probeResolutions()
{
    gsttest->pause();
    updateResolutions();
    gsttest->stop();
}

void Mainwin::updateResolutions()
{
    if (!gsttest)
        return;
    std::vector<int> res = gsttest->getResolutions();
    resolutions->clear();
    for (unsigned int i = 0; i< res.size(); i++) {
        if (res[i] >0) {
            QString text = QString::number(i) + "x" + QString::number(res[i]);
            resolutions->addItem(text);
        }
    }
}

void Mainwin::resolutionBoxChanged(int index)
{
    QString text = resolutions->itemText(index);
    QString delim("x");
    int pos = text.indexOf(delim);
    QString w = text.left(pos);
    QString h = text.mid((pos + delim.length()));
    int width = w.toInt();
    int height = h.toInt();
    qDebug() << "Setting Resolution " << width << "X" << height;
    gsttest->setResolution(width, height);
}


void Mainwin::displayBin()
{
    BinDisplay *win = new BinDisplay();
    win->setBin(GST_BIN(gsttest->pipeline));
    win->setMinimumSize(500,200);
    win->setVisible(true);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GstTest::initialize(argc, argv);

    Player *player = new Player();
    QWidget *mainwin = new Mainwin(player);

    mainwin->show();
    return app.exec();
}

