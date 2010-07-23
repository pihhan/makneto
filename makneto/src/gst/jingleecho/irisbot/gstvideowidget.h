#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QDebug>

/** @brief Small class to easy displaying of Gstreamer video inside Qt. 
    @author Petr Mensik <pihhan@cipis.net> */
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

        bool keepAspectRatio() const;
        void setKeepAspectRatio(bool keep);

        double zoom() const;
        void setZoom(double zoom);

        bool resizable() const;
        void setResizable(bool is_resizable);

    public slots:
        void displayHandle();

    protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
        virtual int  heightForWidth(int w) const;

    private:
    QSize m_videoSize;
    bool  m_playing;
    bool  m_keepAspectRatio;
    double m_zoom;
    bool  m_resizable;

};


#endif

