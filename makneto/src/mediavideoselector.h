
#ifndef MEDIAVIDEOSELECTOR_H
#define MEDIAVIDEOSELECTOR_H

#include <QString>
#include <QWidget>
#include <QBoxLayout>

#include "maknetocontact.h"

/** @brief Widget for displaying one, or more of user videos. */
class MediaVideoSelector : public QWidget
{
    Q_OBJECT
    public:

    MediaVideoSelector();

    QString selected() const;

    void selectUser(const QString &username);
    void addWidget(QWidget *w);
    void createUser(MaknetoContact *contact);
   
    signals:
    void userSelected(const QString &selected);
    void noOneSelected();

    private:
    QString m_selected;
    QBoxLayout *m_layout;

};

#endif

