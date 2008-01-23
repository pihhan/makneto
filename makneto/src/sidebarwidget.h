/*
 * sidebarwidget.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QtGui/QWidget>

class KMultiTabBar;
class QHBoxLayout;
class QStackedWidget;

/**
 * This is sidebar widget for Makneto
 *
 * @short Sidebar widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class SidebarWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	SidebarWidget(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~SidebarWidget();

	int appendTabWidget(QWidget *widget, const QPixmap &pic, int id, const QString &text); 
	int currentIndex() const { return m_currentIndex; }
	void setCurrentIndex(int index);
private slots:
	void tabClicked(int id);
private:
	QHBoxLayout *m_layout;
	KMultiTabBar *m_multitab;
	QStackedWidget *m_widgets;
	int m_currentIndex;
};

#endif // SIDEBARWIDGET_H
