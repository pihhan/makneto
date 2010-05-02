#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QDebug>

class GstVideoWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QSize videoSize READ videoSize WRITE setVideoSize)
    public:
        GstVideoWidget(QWidget *parent = 0) ;
        virtual ~GstVideoWidget();

        void setVideoSize(const QSize size);
        QSize videoSize() const;
        virtual QSize sizeHint() const;
        virtual QSize minimumSizeHint() const;

        bool playing() const;
        void setPlaying(bool playing);


    public slots:
        void displayHandle();

    protected:
        virtual void paintEvent(QPaintEvent *event);



    private:
    QSize m_videoSize;
    bool  m_playing;

};


#endif

