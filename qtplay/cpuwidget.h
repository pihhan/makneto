
#ifndef CPU_WIDGET_H
#define CPU_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

#include "cpucounter.h"

class CpuWidget : public QWidget
{
    Q_OBJECT
    public:

    CpuWidget(QWidget *parent = 0);

    public Q_SLOTS:
    void updateCounters();


    private:

    QLabel *m_userLabel;
    QLabel *m_systemLabel;
    QTimer *m_timer;
    CpuCounter  cpucounter;
};


#endif

