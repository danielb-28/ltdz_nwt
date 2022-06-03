/***************************************************************************
                          configfile.h  -  description
                             -------------------
    begin                : Son Aug 17 2003
    copyright            : (C) 2003 by Andreas Lindenau
    email                : DL4JAL@darc.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CALIBDLG_H
#define CALIBDLG_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QT_BEGIN_NAMESPACE

//#include <QDialog>;
//#include <QPushButton>;
//#include <QRadioButton>;

#include "nwt4window.h"
#include "konstdef.h"

/*
class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
*/

class calibdlg : public QDialog{
    Q_OBJECT

public:
  calibdlg(QWidget *parent = 0);
  ~calibdlg();
  void setdaten(const QMesskopf &);
  QMesskopf getdaten();
  void setHWVariante(evariante);
  void setTitel(QString &);
  void tip(bool);
  void init();

public slots:

private:
  QMesskopf messkopf;
  QPushButton *weiter;
  QPushButton *cancel;
  QComboBox *cbmesskopf;
  QLabel *lmesskopf;

  QGroupBox *grmkauswahl;

  evariante hwvariante;

private slots:
  void fweiter();
  void setmesskopf(int);

};


#endif // CALIBDLG_H
