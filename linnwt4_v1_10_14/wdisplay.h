/***************************************************************************
                          wdisplay.h  -  description
                             -------------------
    begin                : 15.Mai.2007
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
#ifndef WDISPLAY_H
#define WDISPLAY_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

/*
#include <QMap>
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QString>
#include <QLineEdit>
#include <QObject>
#include <QVector>
#include <QPointF>
#include <QPen>
#include <QAction>

#include <QMouseEvent>
#include <QtGui>
#include <QImage>
#include <QImageWriter>
#include <QFileDialog>
*/

#include "konstdef.h"

/*
class QPoint;
class QPointF;
class QPen;
class QString;
class QStringList;
class QImage;
class QImageWriter;
class QDir;
class QFileDialog;
class QAction;
class QSettings;
*/

class QMarker
{
public:
  QMarker();

  int m;//Nummer des Markers
  double frq[6];//Frequenz des Markers
  double db1[6];//dB Kanal1
  double db2[6];//dB Kanal2
  double swv1[6];//swv Kanal1
  double swv2[6];//swv Kanal2
  double dbh1[6];//dB Hintergrund Kurve1
  double dbh2[6];//dB Hintergrund Kurve2
  double dbh3[6];//dB Hintergrund Kurve3
  double dbh4[6];//dB Hintergrund Kurve4
  double dbh5[6];//dB Hintergrund Kurve5
  double swvh1[6];//swv Hintergrund Kurve1
  double swvh2[6];//swv Hintergrund Kurve2
  double swvh3[6];//swv Hintergrund Kurve3
  double swvh4[6];//swv Hintergrund Kurve4
  double swvh5[6];//swv Hintergrund Kurve5
  QPoint point[6];//position in Pixel
  bool bmarkerein[6];//Marker ein/aus
  QStringList strlist;//Liste der Ausgabestrings
  QPoint textpoint;//position des Textes
  bool bkeinmarker;//alle Marker aus

  bool addMarker();//Marker hinzu
  void clrMarker();//alle Marker loeschen
  void setPosIdx(QPoint,int);//set MarkerPosition 1..6
  void setPos(QPoint);//set Markerposition current
  void setMarkertextPos(QPoint);//neue Position Markertext
  void setdB1(int,double);//dB1 eines Markers setzen
  void setdB2(int,double);//dB2 eines Markers setzen
  void setdBh1(int,double);//dBh1 eines Markers setzen
  void setdBh2(int,double);//dBh2 eines Markers setzen
  void setdBh3(int,double);//dBh3 eines Markers setzen
  void setdBh4(int,double);//dBh4 eines Markers setzen
  void setdBh5(int,double);//dBh5 eines Markers setzen
  void setFrq(double);//Frq eines Markers setzen
  void setFrqIdx(int,double);//Frq eines Markers setzen
  bool isPosition(QPoint);//Position des aktuellen Markers
  int textPosX();//Markertextpos X
  int posX();//Markeposition
  int textPosY();//Markertextpos Y
  QPoint MarkertextPos();//Markertextpos
  void addMarkerStrList(QString);//MarkerListe add
  void clrMarkerStrList();//Markerliste loeschen
  void frq_korrektur(double, double);//Korrektur der Markerfrequenzen, wenn ausserhalb
};

class PlotSettings
{
  public:
  PlotSettings();

  void scroll(int dx, int dy);
  void adjust();
  double spanX() const { return maxX - minX; }
  double spanY() const { return maxY - minY; }

  double minX;
  double maxX;
  double schrittweiteX;
  int mpunkteX;
  double minY;
  double maxY;
  int numXTicks;
  int numYTicks;
  double dblinie;
  double swvlinie1;
  double swvlinie2;
  bool bfontsize;
  QString settingspath;
  QString infoversion;
  evariante hwvariante;

  QPoint popuppoint;

  QString xeinheit;
  QString yeinheit;
  QString zeinheit;
  QColor colorhintergrund;
  QPen penmarkerschrift;
  int markerfontsize;
  int fontsize;
  QPen penspur;
  QPen penschrift;
  QPen pensolidline;
  QPen pendotline;
  QPen pendashline;
  QPen pendashdotline;
  QPen pendashdotdotline;
  //QPen pendashdotdot0dbline;
  QPen penlinek;
  QPen penlinekmarker;
  QPen pendotlinek;
  QPen pendashlinek;
  QPen pendashdotlinek;
  QPen pendashdotdotlinek;
  QPen penfmarken;
  QPen penhlinek1;
  QPen penhlinek2;
  QPen penhlinek3;
  QPen penhlinek4;
  QPen penhlinek5;

  void setminX(double);
  void setmaxX(double);
  void setschrittweiteX(double);
  void setmpunkteX(int);
  void setminY(double);
  void setmaxY(double);
  void setdbline(double);
  void setnumXTicks(int);
  void setnumYTicks(int);
  void setPopupPoint(QPoint p);
  void setxeinheit(const QString &);
  void setyeinheit(const QString &);
  void setzeinheit(const QString &);
  void setcolorfmarken(QColor);
  void setcolorhintergrund(QColor);
  void setcolorschrift(QColor);
  void setcolormarkerschrift(QColor);
  void setcolorlinek(QColor);
  void setcolorspur(QColor);
  void setcolorlinekmarker(QColor);
  void setcolorhline1(QColor);
  void setcolorhline2(QColor);
  void setcolorhline3(QColor);
  void setcolorhline4(QColor);
  void setcolorhline5(QColor);
  void setmarkerfontsize(int);
  void setfontsize(int);
  void setEnabledfontsize(bool);
  void setbdBm(bool);
  bool isbdBm();
  void setbwatt(bool);
  bool isbwatt();
  void setbvolt(bool);
  bool isbvolt();

  void setDisplayshift(double);
  double Displayshift();
  void setKurvenshift(double);
  double Kurvenshift();
  void set0dBLine(bool);
  bool is0dBLine();
  void setSettingPath(const QString &);
  void setInfoversion(const QString &);


  private:
  bool bdBm;
  bool bwatt;
  bool bvolt;
  double dshift;
  double kshift;
  bool b0dBLine;

};


class WDisplay : public QWidget
{
  Q_OBJECT

private:
  enum { Margin = 60 };
  TMessKurve dmesskurve;
  THMessKurve dhmesskurve1;
  THMessKurve dhmesskurve2;
  THMessKurve dhmesskurve3;
  THMessKurve dhmesskurve4;
  THMessKurve dhmesskurve5;

public:
    /** construtor */
  WDisplay(QWidget* parent=0);
    /** destructor */
  ~WDisplay();

  void setPlotSettings(const PlotSettings &);
  void setFrqMarken(const TFrqmarken &);
  void writeSettings();
  void readSettings();
  void addMarkerStrList(QString);
  void clrMarkerStrList();
  void setHMessKurve(THMessKurve, int);

public slots:
  void setEnabled(bool);
  void setMessKurve(const TMessKurve &);

protected:
//  void closeEvent(QCloseEvent *event);
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *event);
//  virtual void keyPressEvent( QKeyEvent *event );

private:
  QPixmap pix, pix1, pixcursor;
  QAction *afrequenzanfang;
  QAction *afrequenzende;
  QAction *afrequenzmitte;
  QAction *akurveclr;
  QAction *amarker_plus;
  QAction *apeak_liste;
  QAction *amarker_clr_all;
  QAction *amarker_delta_dblinie;
  QAction *amovetext;
  QAction *afrequenzberechnung;
  QAction *akabellaenge;
  QAction *akabeldaten;
  QAction *akabelimpedanz;

  bool bfrquebergabe;
  bool bmarkerplus;
  bool bmarkerset;
  bool bmarkermove;
  bool bmarkertextmove;
  bool bfrqanfang;
  bool bfrqende;
  bool bfrqmitte;
  bool bdeltadblinie;
  double cursorfrq;
  double markerfrq;
  double cursordb;
  Tkabeldaten kabeldaten;
  bool bd; //BOOL Variable zum Debugen

  QMenu *popupmenu;

  QMarker marker;

  TFrqmarken frqmarken;

  int ObererRand;
  int LinkerRand;
  int hoehe;  //Hoehe der Wobbeldarstellung
  int breite; //Breite der wobbeldarstellung
  PlotSettings dsettings;
  QString settingspath;
  double dbmgrenze;
  //QStringList peaklist;
  //bool bpeaklist;
  TPeaklist peaklist;

  void drawBeschriftung(QPainter *p);
  void drawKurve(QPainter *p);
  void refreshPixmap();
  void refreshKurve();
  QString frq2str(double);
  double db2swv(double);
  double dbm2mwatt(double);
  double dbm2volt(double);
  QString mwatt2str(double d);
  QString volt2str(double d);
  QString swv2str(double d);

private slots:
  void dsetfrqanfang();
  void dsetfrqende();
  void dsetfrqmitte();
  void kurveclr();
  void markerplus();
  void setdbmgrenze();
  void setpeaklist();
  void markerdeltadblinie();
  void setTitleDeltamenue();

  void markerclrall();
  void markertextmove();
  void frequenzuebergabe();
  void fkabellaenge();
  void fkabeldaten();
  void fkabelimpedanz();
  void bildspeichern(QString);
  void sdBmGrenze(double);


  signals:
  void frqanfang2edit(double d);
  void frqende2edit(double d);
  void frqmitte2edit(double d);
  void frq2berechnung(double d);
  void wkurveclr();
  void kdaten(Tkabeldaten);
  void sendinfolist(QStringList);

};

#endif
