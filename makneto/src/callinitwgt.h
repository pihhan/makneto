
#ifndef CALLINITWIDGET_H
#define CALLINITWIDGET_H

#include <QWidget>
#include <QPushButton>

class CallInitWidget : public QWidget
{
    Q_OBJECT
    public:
    CallInitWidget(QWidget *parent = 0);

    void setJid(const QString &s);
    QString jid() const;

    signals:
    void initVideoCall(const QString &jid);
    void initAudioCall(const QString &jid);

    private:
    QPushButton *m_audiobtn;
    QPushButton *m_videobtn;
    QString     m_jid;

};

#endif

