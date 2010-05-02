
#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QLineEdit>
#include <QX11Info>
#include <QComboBox>

#include "player.h"
#include "gsttest.h"

class QListWidget;

class Mainwin : public QWidget
{
    Q_OBJECT

    public:
        Mainwin(Player *player, QWidget *w= 0);

    void displayX11info(const QX11Info &info);

    public slots:
        void displayHandle();
        void startplayer();
        void pauseplayer();
        void selectFile();
        void updateResolutions();
        void probeResolutions();
        void displayBin();

        void setLocation(const QString &path);
        void resolutionBoxChanged(int index);

    signals:
        void fileSelected(QString path);
    private:

    QLabel *label;
    QPushButton *button;
    QBoxLayout *layout1;
    QPushButton *btn2;
    QPushButton *pausebtn;
    QLineEdit *path;
    QPushButton *select;
    QComboBox *resolutions;

    GstTest *gsttest;
    Player *player;
};



#endif
