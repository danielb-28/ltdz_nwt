/***************************************************************************
                          modem.h  -  description
                             -------------------
    begin                : Wed Feb 14 2001
    copyright            : (C) 2001 by Lars Schnake
    email                : mail@lars-schnake.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODEM_H
#define MODEM_H

//#############################################################################
//#############################################################################
//#############################################################################
//#############################################################################
//#############################################################################
// Quelltext fuer Linux
//#############################################################################
//#############################################################################
//#############################################################################
//#############################################################################
//#############################################################################
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include <sys/types.h>
#include <termios.h>

#include "konstdef.h"

//modem class for serial communication
//  *@author Lars Schnake
//  *@author DL4JAL Andreas Lindenau Aenderungen fuer NWT7 + HFM9 + NWT FA

class Modem : public QObject  {

Q_OBJECT
public:
  Modem();
  ~Modem();

// lock and open serial port with settings of ConfigData
  bool opentty(QString seriell_dev);
// unlock and close serial port
  bool closetty();
// write a char to port
  bool writeChar(unsigned char);
// write a char[] to port
  bool writeLine(const char *);
// returns the last modem (error?)message
  const QString modemMessage();
// disconnect the notify connection ( see notify )
  void stop();
// simple read from port
  int rs232_read(void *, int);
  int getFD();
// neue funktion fuer den NWT7
  int readttybuffer(void *, int);


public:
  static Modem *modem;

signals:
// is emitted if data was received
  void charWaiting(unsigned char);
  void setTtyText(QString);

public slots:

	// notify if chars arriving from ptc
  void startNotifier();
	// stop the notifier
  void stopNotifier();
	// read from port
  void readtty(int);

private:
  speed_t modemspeed();  // parse modem speed
  bool modem_is_locked;
  int modemfd;
  QSocketNotifier *sn;
  bool data_mode;
  QString errmsg;
//  QString seriell_dev;
  struct termios initial_tty;
  struct termios tty;
  unsigned char dataMask;
  bool lock_device();
  bool unlock_device();
  QString qdev;
};

#endif

