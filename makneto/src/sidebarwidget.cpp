/*
 * sidebarwidget.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sidebarwidget.h"
#include "connectionview.h"
#include "kmultitabbar.h"
#include "kiconloader.h"
#include "klocale.h"
#include "kdebug.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>


SidebarWidget::SidebarWidget(QWidget *) : m_currentIndex(-1)
{
	m_layout = new QHBoxLayout(this);
	m_layout->setMargin(0);
	m_layout->setSpacing(0);

	m_multitab = new KMultiTabBar(KMultiTabBar::Left, this);
	m_widgets = new QStackedWidget();

	m_layout->addWidget(m_multitab);
	m_layout->addWidget(m_widgets);

	m_multitab->setPosition(KMultiTabBar::Left);

	setMinimumSize(0, 0);

	setLayout(m_layout);
}

SidebarWidget::~SidebarWidget()
{

}

int SidebarWidget::appendTabWidget(QWidget *widget, const QPixmap &pic, int id, const QString &text)
{
	// append tab and tab signal
	m_multitab->appendTab(pic, id, text);
	connect(m_multitab->tab(id), SIGNAL(clicked(int)), this, SLOT(tabClicked(int)));

	// append widget to stack
	m_widgets->insertWidget(id, widget);

	return 0;
}

void SidebarWidget::setCurrentIndex(int index)
{
	if (m_currentIndex != index)
	{
		// reset current tab
		m_multitab->setTab(m_currentIndex, false);

		m_currentIndex = index;

		// set widget to index
		m_multitab->setTab(m_currentIndex, true);
		
		m_widgets->setCurrentIndex(index);
	}
}

void SidebarWidget::tabClicked(int id)
{
	if (m_currentIndex != id)
	{
		setCurrentIndex(id);
		m_widgets->setVisible(true);
	}
	else
	{
		m_widgets->setVisible(!m_widgets->isVisible());
	}
}

void SidebarWidget::hideAll()
{
  m_widgets->setCurrentIndex(-1);
  m_widgets->setVisible(false);
  for (int i = 0; i < m_widgets->count(); i++)
    m_multitab->setTab(i, false);
}

#include "sidebarwidget.moc"
