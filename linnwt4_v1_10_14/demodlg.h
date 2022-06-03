/***************************************************************************
                          demodlg.h  -  description
                             -------------------
    begin                : Son Aug 27 2017
    copyright            : (C) 2017 by Andreas Lindenau
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
#ifndef DEMODLG_H
#define DEMODLG_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QT_BEGIN_NAMESPACE

#include "nwt4window.h"
#include "konstdef.h"

class demodlg : public QDialog{
    Q_OBJECT

public:
  demodlg(QWidget *parent = 0);
  ~demodlg();
  void setdaten(const TDemoFile &);
  TDemoFile getdaten();
  void tip(bool);

public slots:

private:
  QPushButton *ok;
  QPushButton *cancel;
  QCheckBox *cbmaxy;
  QCheckBox *cbminy;
  QCheckBox *cbdshift;
  QCheckBox *cbswrk1;
  QCheckBox *cbswrk2;
  QGroupBox *grdemofile;

  TDemoFile demofile;

private slots:

};

#endif // DEMODLG_H
