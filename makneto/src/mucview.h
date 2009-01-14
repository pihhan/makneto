/*
 * mucview.h
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCVIEW_H
#define MUCVIEW_H

#include <QWidget>
#include <QListWidget>
#include "makneto.h"
class QPushButton;
class QListWidgetItem;

class MUCView : public QWidget
{
  Q_OBJECT
public:
  /**
   * Default constructor
   */
  MUCView(QWidget *parent, Makneto *makneto);

  /**
   * Destructor
   */
  virtual ~MUCView();
public slots:
  void createMUCClicked(bool toggled);
  void joinMUCClicked(bool toggled);
  void bookmarkedMUC(QListWidgetItem *item);
  
private:
  QVBoxLayout *m_mainlayout;
  QHBoxLayout *m_buttonslayout;

  QPushButton *m_createMUC;
  QPushButton *m_joinMUC;
  
  QListWidget *m_MUCbookmarks;

  Makneto *m_makneto;
};

#endif // MUCVIEW_H
