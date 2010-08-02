
#ifndef INCOMINGDIALOG_H
#define INCOMINGDIALOG_H

#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QLabel>

class IncomingCallDialog : public QDialog
{
    public:
    IncomingCallDialog(QWidget *parent = 0);

    void setAudio(bool present);
    void setVideo(bool present);
    void setName(const QString &name);

    void setAutoAccept(int seconds);

    private:
    bool        m_audio;
    bool        m_video;
    int         m_autoAccept;
    QString     m_name;
    QPushButton *m_accept;
    QPushButton *m_reject;
    QLable      *m_label;
};

#endif

