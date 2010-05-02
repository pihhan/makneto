
#include <QHBoxLayout>
#include <QTextStream>
#include "cpuwidget.h"



CpuWidget::CpuWidget(QWidget *parent)
    : QWidget(parent), m_userLabel(0), m_systemLabel(0), m_timer(0)
{
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), SLOT(updateCounters()) );

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(new QLabel(tr("User")) );
    m_userLabel = new QLabel("-");
    m_systemLabel = new QLabel("-");
    layout->addWidget( m_userLabel );
    layout->addWidget( new QLabel(tr("System") ));
    layout->addWidget( m_systemLabel );

    setLayout(layout);
    m_timer->start();
}


void CpuWidget::updateCounters()
{
    float user = cpucounter.userPercent();
    float system = cpucounter.systemPercent();
    QString usrtxt;
    QTextStream ustream(&usrtxt);
    QString systxt;
    QTextStream sstream(&systxt);

    ustream.setRealNumberPrecision(2);
    ustream << user << " %";
    sstream.setRealNumberPrecision(2);
    sstream << system << " %";

    m_userLabel->setText(usrtxt);
    m_systemLabel->setText(systxt);
}

