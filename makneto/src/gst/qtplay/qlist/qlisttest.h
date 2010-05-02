#ifndef QLISTTEST_H
#define QLISTTEST_H

#include <QWidget>
#include <QListWidget>

class Test : public QWidget
{
    Q_OBJECT
    public:
    Test();

    private:
    QListWidget *view;
};

#endif
