/*
 * maknetoview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#ifndef MAKNETOVIEW_H
#define MAKNETOVIEW_H

#include <QtGui/QWidget>

class QPainter;
class KUrl;

/**
 * This is the main view class for Makneto.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoView : public QWidget
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    MaknetoView(QWidget *parent);

    /**
     * Destructor
     */
    virtual ~MaknetoView();

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
};

#endif // MaknetoVIEW_H
