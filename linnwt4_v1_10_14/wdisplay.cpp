/***************************************************************************
                          wdisplay.cpp  -  description
                             -------------------
    begin                : Son Aug 10 2003
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
#include <cmath>

/*
//#include <math.h>
#include <stdlib.h>
#include <QDateTime>
#include <QCursor>
#include <QFileDialog>
#include <QLocale>
#include <QtGlobal>
#include <QMessageBox>
#include <QInputDialog>
#include <QStylePainter>
#include <QTextStream>
#include <QMapIterator>
#include <QVector>
#include <QRect>
#include <QString>
*/
#include "wdisplay.h"
#include "qpeaklist.h"


WDisplay::WDisplay(QWidget *parent) : QWidget(parent)
{
//  this->resize(800,480);
  bd=false;
  this->setMinimumSize(400,300);
  this->setWindowTitle(tr("Display","Wobbelfenster"));

  pix = QPixmap(size());  //Hintergrund Pixmap zum zeichnen
  pix1 = QPixmap(size()); //Hintergrund Pixmap zum zeichnen

  /*
  popupdelta = new QMenu("Delta");
  amarker_delta_dblinie = new QAction(tr("Delta zur dB-Zusatzlinie (D)","PopupMenue"), popupdelta);
  amarker_delta_dblinie->setEnabled(false);
  amarker_delta_dblinie->setCheckable(true);
  connect(amarker_delta_dblinie, SIGNAL(triggered()), this, SLOT(markerdeltadblinie()));
  popupdelta->addAction(amarker_delta_dblinie);
  */
  popupmenu = new QMenu();
  amarker_plus = new QAction(tr("Add Frequenz Marker","PopupMenue"), popupmenu);
  amarker_plus->setEnabled(false);
  connect(amarker_plus, SIGNAL(triggered()), this, SLOT(markerplus()));
  popupmenu->addAction(amarker_plus);
  bmarkerplus=false;//Merbit für ADD eines Markes, nach ADD wieder false

  amarker_clr_all = new QAction(tr("CLR all Frequenz Marker","PopupMenue"), popupmenu);
  amarker_clr_all->setEnabled(false);
  connect(amarker_clr_all, SIGNAL(triggered()), this, SLOT(markerclrall()));
  popupmenu->addAction(amarker_clr_all);

  amarker_delta_dblinie = new QAction(tr("Delta zur dB-Zusatzlinie (D)","PopupMenue"), popupmenu);
  amarker_delta_dblinie->setEnabled(false);
  amarker_delta_dblinie->setCheckable(true);
  connect(amarker_delta_dblinie, SIGNAL(triggered()), this, SLOT(markerdeltadblinie()));
  popupmenu->addAction(amarker_delta_dblinie);

  apeak_liste = new QAction(tr("Peak-Liste erzeugen","PopupMenue"), popupmenu);
  apeak_liste->setEnabled(false);
  connect(apeak_liste, SIGNAL(triggered()), this, SLOT(setdbmgrenze()));
  popupmenu->addAction(apeak_liste);
  apeak_liste->setVisible(false);

  popupmenu->addSeparator();
  afrequenzanfang = new QAction(tr("SET Frequenz Start","PopupMenue"), popupmenu);
  connect(afrequenzanfang, SIGNAL(triggered()), this, SLOT(dsetfrqanfang()));
  popupmenu->addAction(afrequenzanfang);
  bfrqanfang=false;
  afrequenzende = new QAction(tr("SET Frequenz Stop","PopupMenue"), popupmenu);
  connect(afrequenzende, SIGNAL(triggered()), this, SLOT(dsetfrqende()));
  popupmenu->addAction(afrequenzende);
  bfrqende=false;
  afrequenzmitte = new QAction(tr("SET Frequenz Center","PopupMenue"), popupmenu);
  connect(afrequenzmitte, SIGNAL(triggered()), this, SLOT(dsetfrqmitte()));
  popupmenu->addAction(afrequenzmitte);
  bfrqmitte=false;
  popupmenu->addSeparator();
  akurveclr = new QAction(tr("Kurve loeschen","PopupMenue"), popupmenu);
  akurveclr->setEnabled(false);
  connect(akurveclr, SIGNAL(triggered()), this, SLOT(kurveclr()));
  popupmenu->addAction(akurveclr);
  popupmenu->addSeparator();
  amovetext = new QAction(tr("Markertext verschieben","PopupMenue"), popupmenu);
  connect(amovetext, SIGNAL(triggered()), this, SLOT(markertextmove()));
  popupmenu->addAction(amovetext);
  bmarkertextmove=false;
  popupmenu->addSeparator();
  afrequenzberechnung = new QAction(tr("Frequenzuebergabe fuer Berechnung","PopupMenue"), popupmenu);
  connect(afrequenzberechnung, SIGNAL(triggered()), this, SLOT(frequenzuebergabe()));
  popupmenu->addAction(afrequenzberechnung);
  bfrquebergabe=false;
  popupmenu->addSeparator();
  akabellaenge = new QAction(tr("Berechnung der geometrischen Kabellaenge","PopupMenue"), popupmenu);
  connect(akabellaenge, SIGNAL(triggered()), this, SLOT(fkabellaenge()));
  popupmenu->addAction(akabellaenge);
  akabellaenge->setEnabled(false);
  akabeldaten = new QAction(tr("Berechnung der elektrischen Daten des Kabels","PopupMenue"), popupmenu);
  connect(akabeldaten, SIGNAL(triggered()), this, SLOT(fkabeldaten()));
  popupmenu->addAction(akabeldaten);
  akabeldaten->setEnabled(false);
  akabelimpedanz = new QAction(tr("Berechnung der Impedanz des Kabels","PopupMenue"), popupmenu);
  connect(akabelimpedanz, SIGNAL(triggered()), this, SLOT(fkabelimpedanz()));
  popupmenu->addAction(akabelimpedanz);
  akabelimpedanz->setEnabled(false);

  setBackgroundRole(QPalette::Dark);
  setAutoFillBackground(true);
  marker.clrMarker();// alle Marker loeschen
  kabeldaten.ca = 0.0;
  kabeldaten.dk = 0.0;
  kabeldaten.laenge = 0.0;
  kabeldaten.lfrequenz = 0.0;
  kabeldaten.vf = 0.0;
  kabeldaten.z = 0.0;
  bdeltadblinie=false;
  peaklist.plist.clear();
  peaklist.bplist=false;
}

WDisplay::~WDisplay()
{
}

void WDisplay::mouseMoveEvent(QMouseEvent * e)
{
  QPoint pos=e->pos();
  QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);

  //qDebug()<<"WDisplay::mouseMoveEvent(QMouseEvent * e)";
  if(bd)qDebug()<<"WDisplay::mouseMoveEvent(QMouseEvent * e)";
  //qDebug("WidgetWobbeln::mouseMoveEvent()");
  //qDebug()<<pos;
  if(pos.x()<rect.left())pos.setX(rect.left());
  if(pos.x()>rect.right())pos.setX(rect.right());
  if(pos.y()>rect.bottom())pos.setY(rect.bottom());
  if(pos.y()<rect.top())pos.setY(rect.top());
  double y = double(pos.y()-Margin)/(rect.height()-1);
  cursordb= y*(dsettings.minY-dsettings.maxY)+dsettings.maxY+dsettings.Displayshift();
  double x = double(pos.x()-Margin)/(rect.width()-1);
  cursorfrq= x*(dsettings.maxX-dsettings.minX)+dsettings.minX;
  if(bmarkertextmove){
    marker.setMarkertextPos(pos);
    dmesskurve.point=QPoint(0,0);
  }else{
    if(bmarkermove){
      marker.setFrq(cursorfrq);
      marker.setPos(pos);
      dmesskurve.point=QPoint(0,0);
    }else{
      dmesskurve.point=pos;
    }
  }
  //qDebug("SETFrqAnfang");
  //qDebug()<<pos;
  //qDebug("width %i",(rect.width()-1));
  //qDebug("x %f", x);
  //qDebug("Frq: %3.6f MHz", cursorfrq);
  refreshPixmap();
  repaint();
  if(bd)qDebug()<<"WDisplay::mouseMoveEvent(QMouseEvent * e) ENDE";
  //qDebug()<<"WDisplay::mouseMoveEvent(QMouseEvent * e) ENDE";
}

void WDisplay::mousePressEvent(QMouseEvent * e)
{
  QPoint pos=e->pos();
  QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);

  //qDebug()<<"WDisplay::mousePressEvent(QMouseEvent * e)";
  if(bd)qDebug()<<"WDisplay::mousePressEvent(QMouseEvent * e)";
  if(e->button() == Qt::LeftButton){
    //Position einschraenken
    if(pos.x()<rect.left())pos.setX(rect.left());
    if(pos.x()>rect.right())pos.setX(rect.right());
    if(pos.y()>rect.bottom())pos.setY(rect.bottom());
    if(pos.y()<rect.top())pos.setY(rect.top());
    double y = double(pos.y()-Margin)/(rect.height()-1);
    cursordb= y*(dsettings.minY-dsettings.maxY)+dsettings.maxY+dsettings.Displayshift(); //Displayshift dazu addieren
    double x = double(pos.x()-Margin)/(rect.width()-1);
    cursorfrq= x*(dsettings.maxX-dsettings.minX)+dsettings.minX;
    if(bmarkertextmove){
      marker.setMarkertextPos(pos);
      dmesskurve.point=QPoint(0,0);
    }else{
      bmarkermove = marker.isPosition(pos);
      if(bmarkermove){
        setCursor(	Qt::PointingHandCursor);
        marker.setFrq(cursorfrq);
        marker.setPos(pos);
      }else{
        dmesskurve.point=pos;
        if(bmarkerplus){
          marker.setFrq(cursorfrq);
          marker.setPos(pos);
        }
      }
    }
    //qDebug()<<pos;
  }
  if(e->button() == Qt::RightButton){
    popupmenu->popup(QCursor::pos());
  }
  refreshPixmap();
  repaint();
  if(bd)qDebug()<<"WDisplay::mousePressEvent(QMouseEvent * e) ENDE";
  //qDebug()<<"WDisplay::mousePressEvent(QMouseEvent * e) ENDE";
}

void WDisplay::mouseReleaseEvent(QMouseEvent * e)
{
  QPoint pos=e->pos();
  double x;
  QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);


  //qDebug()<<"WDisplay::mouseReleaseEvent(QMouseEvent * e)";
  if(bd)qDebug()<<"WDisplay::mouseReleaseEvent(QMouseEvent * e)";
  if(pos.x()<rect.left())pos.setX(rect.left());//nicht ausserhalb des Diagrammes
  if(pos.x()>rect.right())pos.setX(rect.right());//nicht ausserhalb des Diagrammes
  if(pos.y()>rect.bottom())pos.setY(rect.bottom());
  if(pos.y()<rect.top())pos.setY(rect.top());
  bmarkermove=false;
  double y = double(pos.y()-Margin)/(rect.height()-1);
  cursordb= y*(dsettings.minY-dsettings.maxY)+dsettings.maxY+dsettings.Displayshift(); //Displayshift dazu addieren
  //qDebug()<<"cursordb:"<<cursordb;
  x = double(pos.x()-Margin)/(rect.width()-1);
  cursorfrq= x*(dsettings.maxX-dsettings.minX)+dsettings.minX;
  if(e->button() == Qt::LeftButton){
    //Frequenzinformation setzen
    if(bfrqanfang or bfrqende or bfrqmitte or bfrquebergabe){
      if(bfrqanfang)emit frqanfang2edit(cursorfrq); //zur EditEingabe senden
      if(bfrqende)emit frqende2edit(cursorfrq); //zur EditEingabe senden
      if(bfrqmitte)emit frqmitte2edit(cursorfrq); //zur EditEingabe senden
      if(bfrquebergabe)emit frq2berechnung(cursorfrq); //zur EditEingabe senden
      bfrqanfang=false;
      bfrqende=false;
      bfrqmitte=false;
      bfrquebergabe=false;
    }else{
      if(bmarkerplus){
        marker.setFrq(cursorfrq);
        marker.setPos(pos);
        bmarkerplus=false;
      }else{
        if(bmarkertextmove){
          marker.setMarkertextPos(pos);
          bmarkertextmove=false;
        }
      }
    }
    unsetCursor(); //alten Cursor einschalten
    //qDebug()<<pos;
    //blaues Kreuz loeschen, auf 0 setzen
    dmesskurve.point.setX(0);
    dmesskurve.point.setY(0);
  }
  refreshPixmap();
  repaint();
  if(bd)qDebug()<<"WDisplay::mouseReleaseEvent(QMouseEvent * e) ENDE";
  //qDebug()<<"WDisplay::mouseReleaseEvent(QMouseEvent * e) ENDE";
}

void WDisplay::refreshKurve()
{
  if(bd)qDebug("WDisplay::refreshKurve()");
  QPainter p;
  //Beschriftetes Pixmap holen
  pix = pix1;
  p.begin(&pix);
  //und Kurve einzeichnen
  //wobbeldaten.bhintergrund = false;
  drawKurve(&p);
  p.end();
  //und anzeigen
  update(); //paintEvent erzeugen
  if(bd)qDebug("WDisplay::refreshKurve() ENDE");
}

void WDisplay::refreshPixmap()
{
  if(bd)qDebug("WDisplay::refreshPixmap()");
/*
    pixmap = QPixmap(size());
    pixmap.fill(settings.colorhintergrund);
    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
*/
  pix = QPixmap(size());
  pix.fill(dsettings.colorhintergrund);
  //Groesse anpassen und Pixmap loeschen
  QPainter p;
  p.begin(&pix);
  //Beschriftung erzeugen
  drawBeschriftung(&p);
  //Beschriftetes Pixmap zusaetzlich abspeichern
  //bevor die Kurve gezeichnet wird
  pix1 = pix;
  //Kurve zeichen
  drawKurve(&p);
  p.end();
  //und anzeigen
  update();
  if(bd)qDebug("WDisplay::refreshPixmap() ENDE");
}

void WDisplay::paintEvent(QPaintEvent*)
{
  if(bd)qDebug("WDisplay::paintEvent(QPaintEvent*)");
  QPainter painter(this);
  //Beschriftung und Kurve ins Window kopieren
  //zeichen die Groesse hat sich geaendert
  painter.drawPixmap(0, 0, pix);
  //qDebug()<<"paintEvent(QPaintEvent*)";
  if(bd)qDebug("WDisplay::paintEvent(QPaintEvent*) ENDE");
}

void WDisplay::resizeEvent(QResizeEvent*)
{
  if(bd)qDebug("WDisplay::resizeEvent(QResizeEvent*)");
  //Kurve neu zeichen
  refreshPixmap();
  if(bd)qDebug("WDisplay::resizeEvent(QResizeEvent*) ENDE");
}

void WDisplay::drawBeschriftung(QPainter *p)
{
  if(bd)qDebug("WDisplay::drawBeschriftung(QPainter *p)");
  QString qsa, qsb, qsc;
  int xa, xb;
  int yt=0;
  QString qs, qs1;
  double label;

  QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);
  QPen penredsolid = QPen( Qt::red, 0, Qt::SolidLine);
  QFont f=p->font();
  f.setPixelSize(dsettings.fontsize);
  if(dsettings.bfontsize)p->setFont(f);

  if(!rect.isValid())return;

  //============================================================================================
  //linke Seite X Linie
  label = dsettings.minX;
  qs.sprintf("%3.3f", label);
  p->setPen(dsettings.pensolidline);
  p->drawLine(rect.left(), rect.top(), rect.left(), rect.bottom());
  //rechte Seite X Linie
  label = dsettings.maxX;
  qs.sprintf("%3.3f", label);
  p->drawLine(rect.right(), rect.top(), rect.right(), rect.bottom());
  //============================================================================================
  //Festlegung der X-Achseneinteilung
  double wxanfang = dsettings.minX * 1000000.0;
  double wxende = wxanfang + (double(dsettings.mpunkteX-1)) * dsettings.schrittweiteX * 1000000.0;
  double wxbereich = (wxende - wxanfang) / 5.0;
  double xRaster;
  double faktor=1.0;
  while(wxbereich>10.0){
    wxbereich=wxbereich/10.0;
    faktor=faktor*10.0;
  }
  if((wxbereich <=  2.5) & (wxbereich > 1.0))xRaster = 1.0;
  if((wxbereich <=  5.0) & (wxbereich > 2.5))xRaster = 2.5;
  if((wxbereich <= 10.0) & (wxbereich > 5.0))xRaster = 5.0;
  xRaster = xRaster * faktor;
  int i=1;
  while( (xRaster * i) < wxende){
    double dx = i * xRaster;
    //qDebug()<<"dx:"<<dx;
    if((dx>wxanfang) and (dx<wxende)){
      int x= rect.left() + ((dx-wxanfang) * (rect.width()-1) / (wxende-wxanfang));
      p->setPen(dsettings.pendotline);
      p->drawLine(x, rect.top(), x, rect.bottom());
      p->setPen(dsettings.pensolidline);
      qs = frq2str((dx)/1000000.0);
      if(i%2){yt=15;} else {yt=0;}
      p->drawText(x-75, rect.bottom()+Margin/8+yt,150,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
      p->setPen(dsettings.pensolidline);
      p->drawLine(x, rect.bottom(), x, rect.bottom()+5+yt);
    }
    i++;
  }
  //=============================================================================================
  //Y-Achse Beschriftung und Linien dB
  int xl=0, xr=0;
  for(int j=0; j<= dsettings.numYTicks; ++j){
    int y=rect.bottom() - (j*(rect.height()-1)/dsettings.numYTicks);
    double label = dsettings.minY + dsettings.Displayshift() + (j*dsettings.spanY() / dsettings.numYTicks);
    if(dsettings.isbdBm()){
      qs.sprintf("%2.0fdBm", label);
      xl=3; xr=5;
    }else{
      qs.sprintf("%2.0f dB", label);
      xl=10; xr=0;
    }
    if(label == 0.0 and dsettings.is0dBLine()){
      p->setPen(dsettings.pendashdotdotline);
    }else{
      p->setPen(dsettings.pendotline);
    }
    p->drawLine(rect.left(), y, rect.right(), y);
    p->setPen(dsettings.pensolidline);
    p->drawLine(rect.left()-5, y, rect.left(), y);
    if((label - dsettings.Displayshift()) == 0.0 and dsettings.Displayshift() != 0.0 and !dsettings.isbdBm()){
      QString qs1="[S11 0.0]";
      p->drawText(rect.left()-Margin+3,y-22,Margin-5,20,Qt::AlignRight | Qt::AlignVCenter, qs1);
      p->drawText(rect.right()+3,y-22,Margin-5,20,Qt::AlignLeft | Qt::AlignVCenter, qs1);
    }
    p->setPen(dsettings.pendotline);
    p->drawLine(rect.left(), y, rect.right(), y);
    if(dsettings.Displayshift() != 0.0 or dsettings.Kurvenshift() != 0.0)p->setPen(penredsolid);
    p->drawText(rect.left()-Margin-xl,y-10,Margin-5,20,Qt::AlignRight | Qt::AlignVCenter, qs);
    p->setPen(dsettings.pensolidline);
    p->drawLine(rect.right(), y, rect.right()+5, y);
    if(dsettings.Displayshift() != 0.0 or dsettings.Kurvenshift() != 0.0)p->setPen(penredsolid);
    p->drawText(rect.right()+Margin/4-xr,y-10,Margin-5,20,Qt::AlignLeft | Qt::AlignVCenter, qs);
  }
  //=============================================================================================
  //X-Achse Bezeichnung
  //unterhalb
  p->setPen(dsettings.pensolidline);
  //Start
  qs = frq2str(dsettings.minX);
  p->drawText(rect.left()-100,rect.bottom()+Margin/5*3,200,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
  p->drawLine(rect.left(), rect.bottom(), rect.left(), rect.bottom() + 30);
  //Stop
  qs = frq2str(dsettings.maxX);
  p->drawText(rect.right()-100,rect.bottom()+Margin/5*3,200,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
  p->drawLine(rect.right(), rect.bottom(), rect.right(), rect.bottom() + 30);
  //xRaster
  qs = frq2str(xRaster/1000000.0);
  qs = "Display-Raster: "+qs;
  p->drawText(rect.left()+rect.width()/2-100,rect.bottom()+Margin/5*3,200,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
  //=============================================================================================
  //X-Achse Bezeichnung
  //Oberhalb
  qs=dmesskurve.caption;
  p->setPen(dsettings.penschrift);
  p->drawText(rect.left(),rect.top()-Margin/4*3-10,rect.width(),20,Qt::AlignLeft | Qt::AlignVCenter, qs);
  qs = frq2str(dsettings.schrittweiteX);
  qs = tr("Schrittweite: ")+ qs;
  p->setPen(dsettings.pensolidline);
  p->drawText(rect.left(),rect.top()-Margin/4-10,150,20,Qt::AlignLeft | Qt::AlignVCenter, qs);
  qs = tr("Messpunkte: %1").arg(dsettings.mpunkteX,4);
  p->drawText(rect.left(),rect.top()-Margin/4*2-10,150,20,Qt::AlignLeft | Qt::AlignVCenter, qs);
  p->drawRect(rect.adjusted(0,0,-1,-1));
  qs = frq2str(dsettings.spanX());
  qs = "Span: "+qs;
  p->drawText(rect.left()+rect.width()/2-100,rect.top()-Margin/4*2-10,200,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
  //Center
  qs = frq2str(dsettings.spanX()/2+dsettings.minX);
  qs = "Center: "+qs;
  p->drawText(rect.left()+rect.width()/2-100,rect.top()-Margin/4-10,200,20,Qt::AlignCenter | Qt::AlignVCenter, qs);
  p->drawLine(rect.left()+rect.width()/2, rect.top(), rect.left()+rect.width()/2, rect.top() - 5);
  //=============================================================================================
  //Frequenz, Pegel (SWV) des Cursors anzeigen
  if(dmesskurve.calablauf==0){
    if(!dmesskurve.point.isNull()){
      if(dsettings.isbwatt() and dsettings.isbdBm() and dsettings.isbvolt()){
        xa=300; xb=250;
      }else{
        xa=250; xb=200;
      }
      qs = frq2str(cursorfrq);
      qs = "Cursor: "+qs;
      p->drawText(rect.left()+rect.right()-xa,rect.top()-Margin/4-10,xb,20,Qt::AlignRight | Qt::AlignVCenter, qs);
      //swv Kanal1 oder Kanal2 aktiv und Displayshift == 0
      if((dmesskurve.bswv) and (dsettings.Displayshift() == 0) and (dsettings.Kurvenshift() == 0)){
        double swv;
        if(cursordb<0.0){
          swv=db2swv(cursordb);
        }else{
          swv=0.0;
        }
        if(swv>=1.01){
          qsa.sprintf("%2.2f",cursordb);
          qsb.sprintf("%2.2f", swv);
          qs=tr("Pegel: %1dB, SWV:%2").arg(qsa).arg(qsb);
        }else{
          qsa.sprintf("%2.2f",cursordb);
          qsb.sprintf("%2.3f", swv);
          qs=tr("Pegel: %1dB, SWV:%2").arg(qsa).arg(qsb);
        }
      }else{
        qsa.sprintf("%2.2f",cursordb);
        if(dsettings.isbdBm()){
          double mwatt = pow(10.0 , cursordb/10.0);
          double volt = sqrt(mwatt * 0.05);
          qs=tr("Pegel: %1dBm").arg(qsa);
          if(dsettings.isbwatt()){
            qs1=mwatt2str(mwatt);
            qs=qs+", "+qs1;
          }
          if(dsettings.isbvolt()){
            qs1=volt2str(volt);
            qs=qs+", "+qs1;
          }
        }else{
          qs=tr("Pegel: %1dB").arg(qsa);
        }
      }
      p->drawText(rect.left()+rect.right()-xa,rect.top()-Margin/4*2-10,xb,20,Qt::AlignRight | Qt::AlignVCenter, qs);
    }
    //=============================================================================================
    //Zusaetzliche dB Linie
    if(round(dsettings.dblinie*10.0)!=0){
      if((dsettings.dblinie> dsettings.minY+dsettings.Displayshift()+2.0) and (dsettings.dblinie < dsettings.maxY+dsettings.Displayshift()-2.0)){
        double dydb = dsettings.dblinie - dsettings.minY - dsettings.Displayshift(); dydb *= -1.0;
        int y1= rect.bottom() + (dydb * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        p->setPen(dsettings.pendashline);
        p->drawLine(rect.left(), y1, rect.right(), y1);
        if(dsettings.isbdBm()){
          qs.sprintf("%3.1fdBm", dsettings.dblinie);
        }else{
          qs.sprintf("%3.1fdB", dsettings.dblinie);
        }
        p->setPen(dsettings.pensolidline);
        p->drawText(rect.left()+3, y1-20, 60, 20 ,Qt::AlignLeft | Qt::AlignVCenter, qs);
        p->drawText(rect.right()-63, y1, 60, 20 ,Qt::AlignRight | Qt::AlignVCenter, qs);
      }
    }
    //=============================================================================================
    //dbm Grenzline fuer Peaklist
    if(dsettings.isbdBm() and peaklist.bplist){
      double dydb = dbmgrenze - dsettings.minY - dsettings.Displayshift(); dydb *= -1.0;
      int y1= rect.bottom() + (dydb * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
      p->setPen(dsettings.penfmarken);
      p->drawLine(rect.left(), y1, rect.right(), y1);
      qs.sprintf("%3.1fdBm", dbmgrenze);
      p->setPen(dsettings.penfmarken);
      p->drawText(rect.left()+3, y1-20, 60, 20 ,Qt::AlignLeft | Qt::AlignVCenter, qs);
      p->drawText(rect.right()-63, y1, 60, 20 ,Qt::AlignRight | Qt::AlignVCenter, qs);
    }

    //=============================================================================================
    //Zusaetzliche swv1 Linie
    if(round(dsettings.swvlinie1*10.0)!=0){
      double r=(dsettings.swvlinie1-1.0)/(dsettings.swvlinie1+1.0);
      double db=log10(r)*20.0;
      if(db>dsettings.minY){
        double dyswv1 = db - dsettings.minY; dyswv1 *= -1.0;
        int y1= rect.bottom() + (dyswv1 * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        p->setPen(dsettings.pendashline);
        p->drawLine(rect.left(), y1, rect.right(), y1);
        if(dsettings.swvlinie1 < 1.01){
          qsa.sprintf("%2.3f", dsettings.swvlinie1);
          qs=tr("SWV:%1").arg(qsa);
        }else{
          qsa.sprintf("%2.2f", dsettings.swvlinie1);
          qs=tr("SWV:%1").arg(qsa);
        }
        p->setPen(dsettings.pensolidline);
        p->drawText(rect.left()+3, y1-20, 60, 20 ,Qt::AlignLeft | Qt::AlignVCenter, qs);
        p->drawText(rect.right()-63, y1, 60, 20 ,Qt::AlignRight | Qt::AlignVCenter, qs);
      }
    }
    //=============================================================================================
    //Zusaetzliche swv2 Linie
    if(round(dsettings.swvlinie2*10.0)!=0){
      double r=(dsettings.swvlinie2-1.0)/(dsettings.swvlinie2+1.0);
      double db=log10(r)*20.0;
      if(db>dsettings.minY ){
        double dyswv2 = db - dsettings.minY; dyswv2 *= -1.0;
        int y1= rect.bottom() + (dyswv2 * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        p->setPen(dsettings.pendashline);
        p->drawLine(rect.left(), y1, rect.right(), y1);
        if(dsettings.swvlinie2 < 1.01){
          qsa.sprintf("%2.3f", dsettings.swvlinie2);
          qs=tr("SWV:%1").arg(qsa);
        }else{
          qsa.sprintf("%2.2f", dsettings.swvlinie2);
          qs=tr("SWV:%1").arg(qsa);
        }
        p->setPen(dsettings.pensolidline);
        p->drawText(rect.left()+3, y1-20, 60, 20 ,Qt::AlignLeft | Qt::AlignVCenter, qs);
        p->drawText(rect.right()-63, y1, 60, 20 ,Qt::AlignRight | Qt::AlignVCenter, qs);
      }
    }
    //=============================================================================================
    //Alle Frequenzmarken einblenden
    p->setClipRect(rect);//Zeichenfenster begrenzen
    //Variablen fuer Frequenzmarken
    int x, x1, length, pix;
    QString qs;
    double fsize;

    for(int i=0;i<16;i++){
      p->setPen(dsettings.penfmarken);
      //nur wenn Frequnzmarke aktiv ist
      if(frqmarken.aktiv[i]){
        double dx = frqmarken.frq1[i]-dsettings.minX;
        x= rect.left() + (dx * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        //qDebug()<<x;
        p->drawLine(x, rect.top(), x, rect.bottom());
        dx = frqmarken.frq2[i]-dsettings.minX;
        x1= rect.left() + (dx * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        //qDebug()<<x1;
        p->drawLine(x1, rect.top(), x1, rect.bottom());
        //Berechnung ob "frqmarken.caption" hinein passt
        //laenge von caption feststellen
        qs=frqmarken.caption[i];
        //Schriftgroesse ermitteln
        fsize = (double) dsettings.fontsize;
        //laenge von einem Buchstabe berechnen
        pix = round(fsize / 1.5);
        //laenge des Strings
        length = qs.length() * pix;
        //passt String zwischen den beiden senkrechten Strichen
        if((x1 - x) > length){
          //ja String passt hinein
          p->drawText(x-(length/2)+(x1-x)/2, (rect.top()+((rect.bottom()-rect.top())/2)) , frqmarken.caption[i]);
        }
      }
    }
    //=============================================================================================
    //Marker immmer in den Bereich der Displayfrequenz verschieben
    marker.frq_korrektur(dsettings.minX, dsettings.maxX);
  }
  if(bd)qDebug("WDisplay::drawBeschriftung(QPainter *p) ENDE");
}


void WDisplay::drawKurve(QPainter *p)
{
  if(bd)qDebug("WDisplay::drawKurve(QPainter *p)");

  double x, y1, y3;
  double dxfrq, dydb, dydb_ant;
  int k;
  QString qs, qs1, qs2, qsa, qsb, qsc;
  bool bmarkerh1[6];
  bool bmarkerh2[6];
  bool bmarkerh3[6];
  bool bmarkerh4[6];
  bool bmarkerh5[6];
  QStringList markerlist;
  markerlist.clear();

  for(int i=0;i<6;i++){
    bmarkerh1[i]=true;
    bmarkerh2[i]=true;
    bmarkerh3[i]=true;
    bmarkerh4[i]=true;
    bmarkerh5[i]=true;
  }
  //Zeichenbereich festlegen
  QRect rectm(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);
  QRect rect(rectm);

  if(!rect.isValid())return;
  //rings herum 1 Pixel abziehen
  p->setClipRect(rect.adjusted(1,1,-1,-1));
  //nur wenn keine Kalibrierung werden Kurven gezeichnet
  if(dmesskurve.calablauf==0){
    //zeichnen einer senkrechten und wagerechten Linie bei gedrückten Mousebutton allgemein
    if(!dmesskurve.point.isNull()){
      p->setPen(dsettings.penfmarken);
      p->drawLine(rect.right(), dmesskurve.point.y(), rect.left(), dmesskurve.point.y());
      p->drawLine(dmesskurve.point.x(), rect.top(), dmesskurve.point.x(), rect.bottom());
    }
    QPolygonF polylinedb;//Line Kanal dB ohne Inhalt
    QPolygonF polylinedb_ant;//Line Kanal dB_ant ohne Inhalt
    polylinedb.clear();
    polylinedb_ant.clear();
    QPointF pf1[6];//leerer Point fuer Marker
    QPointF pfh1[6];//leerer Point fuer Marker
    QPointF pfh2[6];//leerer Point fuer Marker
    QPointF pfh3[6];//leerer Point fuer Marker
    QPointF pfh4[6];//leerer Point fuer Marker
    QPointF pfh5[6];//leerer Point fuer Marker
    bool markerzeichnen[6];
    for(k=0;k<6;k++)markerzeichnen[k]=true;//Marker zeichnen anstossen
    for(k=0; k<dmesskurve.mpunkte; ++k){
      dxfrq = dmesskurve.frequenz[k]- dsettings.minX;
      dydb = (dmesskurve.dbk1[k] + dsettings.Kurvenshift()) - dsettings.minY;
      //qDebug()<<"dmesskurve.dbk1[k]: "<<dmesskurve.dbk1[k];
      //qDebug()<<"dydb: "<<dydb;
      dydb_ant=0.0;
      if(dmesskurve.adaempfung>0.0){
        if(dmesskurve.bswv){
          dydb_ant=dydb+dmesskurve.adaempfung*2.0;
        }
      }
      dydb *= -1.0;
      dydb_ant *= -1.0;
      x= rect.left() + (dxfrq * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      y1= rect.bottom() + (dydb * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
      polylinedb << QPointF(x,y1);//Messkurve aufzeichnen
      if(dmesskurve.adaempfung>0.0){
        y3= rect.bottom() + (dydb_ant * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        polylinedb_ant << QPointF(x,y3);//Messkurve aufzeichnen
      }
      //Marker bearbeiten
      for(int i=0;i<6;i++){
        double mfrq;
        if(k > dmesskurve.mpunkte/2){
          //rechte haelfte Display
          mfrq = marker.frq[i]-dsettings.schrittweiteX/2.0;
        }else{
          //linke haelfte Display
          mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
        }
        if((mfrq <= dmesskurve.frequenz[k]) and markerzeichnen[i] and marker.bmarkerein[i]){//Frequenzvergleich bis Frequenz erreicht ist
          pf1[i]=QPointF(x,y1);//PointF für untere Spitze des Markers Kanal1
          QPoint point=QPoint(x,y1);//Point der Markers fuer das Fangen der Maus
          marker.setPosIdx(point,i);//bei ReSize neue Position speichern
          if((dmesskurve.mktyp==mks11) or (dmesskurve.mktyp==mks11var)){
            marker.setdB1(i,dmesskurve.dbk1[k]);//S11 Kurve dB+SWV für Marker Kanal1 ohne displayshift
          }else{
            if(bdeltadblinie){
              marker.setdB1(i,dmesskurve.dbk1[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) -dsettings.dblinie);//Delta zur dB-Line
            }else{
              marker.setdB1(i,dmesskurve.dbk1[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 Kurve
            }
          }
          marker.setFrqIdx(i,dmesskurve.frequenz[k]);//Messpunkt Frequenz in den Marker eintragen
          markerzeichnen[i]=false; //nicht noch einmal PointF merken
        }
      }
    }
    p->setPen(dsettings.penlinek);
    if(dmesskurve.bkanal)p->drawPolyline(polylinedb);
    if(dmesskurve.adaempfung>0.0){
      if(dmesskurve.bswv){
        p->setPen(dsettings.pendashlinek);
        p->drawPolyline(polylinedb_ant);
      }
    }
    //Hintergrund Kurve zeichnen
    if(dhmesskurve1.bkanal){
      QPolygonF hpolyline(dhmesskurve1.mpunkte);//leere Line HintergrundKanal
      for(k=0; k<dhmesskurve1.mpunkte; ++k){
        //qDebug() << dsettings.numXTicks;
        double dxhdb = dhmesskurve1.frequenz[k]-dsettings.minX;
        //qDebug()<<"k:"<<k<<"; dhmesskurve1.frequenz[k]"<<dhmesskurve1.frequenz[k];
        double dy = dhmesskurve1.db[k] + dsettings.Kurvenshift() - dsettings.minY;
        dy *= -1.0;
        x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        hpolyline[k]=QPointF(x,y1);
        for(int i=0;i<6;i++){
          double mfrq;
          if(k > dhmesskurve1.mpunkte/2){
            mfrq = marker.frq[i]-dsettings.schrittweiteX/2;
          }else{
            mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
          }
          if((marker.frq[i] > dhmesskurve1.frequenz[0]) and (mfrq <= dhmesskurve1.frequenz[k]) and bmarkerh1[i] and marker.bmarkerein[i]){
            bmarkerh1[i]=false;
            if(dhmesskurve1.bS11){
              marker.setdBh1(i,dhmesskurve1.db[k]);//S11 dB für Marker Hintergrund
            }else{
              marker.setdBh1(i,dhmesskurve1.db[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 dB für Marker Hintergrund
            }
            pfh1[i]=QPointF(x,y1);
          }
        }
      }
      p->setPen(dsettings.penhlinek1);
      p->drawPolyline(hpolyline);
    }
    if(dhmesskurve2.bkanal){
      QPolygonF hpolyline(dhmesskurve2.mpunkte);//leere Line HintergrundKanal
      for(k=0; k<dhmesskurve2.mpunkte; ++k){
        //qDebug() << dsettings.numXTicks;
        double dxhdb = dhmesskurve2.frequenz[k]-dsettings.minX;
        double dy = dhmesskurve2.db[k] + dsettings.Kurvenshift() - dsettings.minY;
        dy *= -1.0;
        x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        hpolyline[k]=QPointF(x,y1);
        for(int i=0;i<6;i++){
          double mfrq;
          if(k > dhmesskurve2.mpunkte/2){
            mfrq = marker.frq[i]-dsettings.schrittweiteX/2;
          }else{
            mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
          }
          if((marker.frq[i] > dhmesskurve2.frequenz[0]) and (mfrq <= dhmesskurve2.frequenz[k]) and bmarkerh2[i] and marker.bmarkerein[i]){
            bmarkerh2[i]=false;
            if(dhmesskurve2.bS11){
              marker.setdBh2(i,dhmesskurve2.db[k]);//S11 dB für Marker Hintergrund
            }else{
              marker.setdBh2(i,dhmesskurve2.db[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 dB für Marker Hintergrund
            }
            pfh2[i]=QPointF(x,y1);
          }
        }
      }
      p->setPen(dsettings.penhlinek2);
      p->drawPolyline(hpolyline);
    }
    if(dhmesskurve3.bkanal){
      QPolygonF hpolyline(dhmesskurve3.mpunkte);//leere Line HintergrundKanal
      for(k=0; k<dhmesskurve3.mpunkte; ++k){
        //qDebug() << dsettings.numXTicks;
        double dxhdb = dhmesskurve3.frequenz[k]-dsettings.minX;
        double dy = dhmesskurve3.db[k] + dsettings.Kurvenshift() - dsettings.minY;
        dy *= -1.0;
        x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        hpolyline[k]=QPointF(x,y1);
        for(int i=0;i<6;i++){
          double mfrq;
          if(k > dhmesskurve3.mpunkte/2){
            mfrq = marker.frq[i]-dsettings.schrittweiteX/2;
          }else{
            mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
          }
          if((marker.frq[i] > dhmesskurve3.frequenz[0]) and (mfrq <= dhmesskurve3.frequenz[k]) and bmarkerh3[i] and marker.bmarkerein[i]){
            bmarkerh3[i]=false;
            if(dhmesskurve3.bS11){
              marker.setdBh3(i,dhmesskurve3.db[k]);//S11 dB für Marker Hintergrund
            }else{
              marker.setdBh3(i,dhmesskurve3.db[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 dB für Marker Hintergrund
            }
            pfh3[i]=QPointF(x,y1);
          }
        }
      }
      p->setPen(dsettings.penhlinek3);
      p->drawPolyline(hpolyline);
    }
    if(dhmesskurve4.bkanal){
      QPolygonF hpolyline(dhmesskurve4.mpunkte);//leere Line HintergrundKanal
      for(k=0; k<dhmesskurve4.mpunkte; ++k){
        //qDebug() << dsettings.numXTicks;
        double dxhdb = dhmesskurve4.frequenz[k]-dsettings.minX;
        double dy = dhmesskurve4.db[k] + dsettings.Kurvenshift() - dsettings.minY;
        dy *= -1.0;
        x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        hpolyline[k]=QPointF(x,y1);
        for(int i=0;i<6;i++){
          double mfrq;
          if(k > dhmesskurve4.mpunkte/2){
            mfrq = marker.frq[i]-dsettings.schrittweiteX/2;
          }else{
            mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
          }
          if((marker.frq[i] > dhmesskurve4.frequenz[0]) and (mfrq <= dhmesskurve4.frequenz[k]) and bmarkerh4[i] and marker.bmarkerein[i]){
            bmarkerh4[i]=false;
            if(dhmesskurve4.bS11){
              marker.setdBh4(i,dhmesskurve4.db[k]);//S11 dB für Marker Hintergrund
            }else{
              marker.setdBh4(i,dhmesskurve4.db[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 dB für Marker Hintergrund
            }
            pfh4[i]=QPointF(x,y1);
          }
        }
      }
      p->setPen(dsettings.penhlinek4);
      p->drawPolyline(hpolyline);
    }
    if(dhmesskurve5.bkanal){
      QPolygonF hpolyline(dhmesskurve5.mpunkte);//leere Line HintergrundKanal
      for(k=0; k<dhmesskurve5.mpunkte; ++k){
        //qDebug() << dsettings.numXTicks;
        double dxhdb = dhmesskurve5.frequenz[k]-dsettings.minX;
        double dy = dhmesskurve5.db[k] + dsettings.Kurvenshift() - dsettings.minY;
        dy *= -1.0;
        x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        hpolyline[k]=QPointF(x,y1);
        for(int i=0;i<6;i++){
          double mfrq;
          if(k > dhmesskurve5.mpunkte/2){
            mfrq = marker.frq[i]-dsettings.schrittweiteX/2;
          }else{
            mfrq = marker.frq[i]-dsettings.schrittweiteX/1.1;
          }
          if((marker.frq[i] > dhmesskurve5.frequenz[0]) and (mfrq <= dhmesskurve5.frequenz[k]) and bmarkerh5[i] and marker.bmarkerein[i]){
            bmarkerh5[i]=false;
            if(dhmesskurve5.bS11){
              marker.setdBh5(i,dhmesskurve5.db[k]);//S11 dB für Marker Hintergrund
            }else{
              marker.setdBh5(i,dhmesskurve5.db[k] + (dsettings.Displayshift() + dsettings.Kurvenshift()) );//S21 dB für Marker Hintergrund
            }
            pfh5[i]=QPointF(x,y1);
          }
        }
      }
      p->setPen(dsettings.penhlinek5);
      p->drawPolyline(hpolyline);
    }
    //Peaks Nummerierung an Kurve
    //Spur zeichnen
    if(dmesskurve.bspur){
      for(int i=0; i<20; i++){
        if(dmesskurve.bdbsp[i]){
          QPointF sppoint[dmesskurve.mpunkte];
          for(k=0; k<dmesskurve.mpunkte; ++k){
            double dxhdb = dmesskurve.frequenz[k]-dsettings.minX;
            double dy = dmesskurve.dbsp[i][k] + dsettings.Kurvenshift() - dsettings.minY;
            dy *= -1.0;
            x= rect.left() + (dxhdb * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
            y1= rect.bottom() + (dy * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
            sppoint[k]=QPointF(x,y1);
          }
          p->setPen(dsettings.penspur);
          for(k=0; k<dmesskurve.mpunkte; ++k){
            p->drawPoint(sppoint[k]);
          }
        }
      }
    }
    //Marker zeichen
    QPolygonF polylinem(4); //Marker fuer Kanal1
    QPolygonF polylineh(4); //Marker fuer h1
    QSize s=size();//Markergrösse richtet sich nach Windowsgrösse
    int w=s.width();
    int mx=3+w/420;
    int my=7+w/120;
    for(int i=0;i<6;i++){
      if(marker.bmarkerein[i]){
        if(dhmesskurve1.bkanal and !bmarkerh1[i] and dhmesskurve1.bmarker){
          y1=pfh1[i].y();
          x=pfh1[i].x();
          polylineh[0]=QPointF(x,y1);
          polylineh[1]=QPointF(x-mx,y1-my);
          polylineh[2]=QPointF(x+mx,y1-my);
          polylineh[3]=QPointF(x,y1);
          p->setPen(dsettings.penhlinek1);
          p->drawPolyline(polylineh);
          qs.sprintf("%iH1",i+1);
          p->drawText(x-4*mx,y1-my-10,30,10,Qt::AlignRight | Qt::AlignVCenter, qs);
        }
        if(dhmesskurve2.bkanal and !bmarkerh2[i] and dhmesskurve2.bmarker){
          y1=pfh2[i].y();
          x=pfh2[i].x();
          polylineh[0]=QPointF(x,y1);
          polylineh[1]=QPointF(x-mx,y1-my);
          polylineh[2]=QPointF(x+mx,y1-my);
          polylineh[3]=QPointF(x,y1);
          p->setPen(dsettings.penhlinek2);
          p->drawPolyline(polylineh);
          qs.sprintf("%iH2",i+1);
          p->drawText(x-4*mx,y1-my-10,30,10,Qt::AlignRight | Qt::AlignVCenter, qs);
        }
        if(dhmesskurve3.bkanal and !bmarkerh3[i] and dhmesskurve3.bmarker){
          y1=pfh3[i].y();
          x=pfh3[i].x();
          polylineh[0]=QPointF(x,y1);
          polylineh[1]=QPointF(x-mx,y1-my);
          polylineh[2]=QPointF(x+mx,y1-my);
          polylineh[3]=QPointF(x,y1);
          p->setPen(dsettings.penhlinek3);
          p->drawPolyline(polylineh);
          qs.sprintf("%iH3",i+1);
          p->drawText(x-4*mx,y1-my-10,30,10,Qt::AlignRight | Qt::AlignVCenter, qs);
        }
        if(dhmesskurve4.bkanal and !bmarkerh4[i] and dhmesskurve4.bmarker){
          y1=pfh4[i].y();
          x=pfh4[i].x();
          polylineh[0]=QPointF(x,y1);
          polylineh[1]=QPointF(x-mx,y1-my);
          polylineh[2]=QPointF(x+mx,y1-my);
          polylineh[3]=QPointF(x,y1);
          p->setPen(dsettings.penhlinek4);
          p->drawPolyline(polylineh);
          qs.sprintf("%iH4",i+1);
          p->drawText(x-4*mx,y1-my-10,30,10,Qt::AlignRight | Qt::AlignVCenter, qs);
        }
        if(dhmesskurve5.bkanal and !bmarkerh5[i] and dhmesskurve5.bmarker){
          y1=pfh5[i].y();
          x=pfh5[i].x();
          polylineh[0]=QPointF(x,y1);
          polylineh[1]=QPointF(x-mx,y1-my);
          polylineh[2]=QPointF(x+mx,y1-my);
          polylineh[3]=QPointF(x,y1);
          p->setPen(dsettings.penhlinek5);
          p->drawPolyline(polylineh);
          qs.sprintf("%iH5",i+1);
          p->drawText(x-4*mx,y1-my-10,30,10,Qt::AlignRight | Qt::AlignVCenter, qs);
        }
        x=pf1[i].x();//Marker Kanal Position
        y1=pf1[i].y();
        polylinem[0]=QPointF(x,y1);
        polylinem[1]=QPointF(x-mx,y1-my);
        polylinem[2]=QPointF(x+mx,y1-my);
        polylinem[3]=QPointF(x,y1);
        p->setPen(dsettings.penlinekmarker);
        if(dmesskurve.bkanal){
          p->drawPolyline(polylinem);//Marker zeichnen
          qs.sprintf("%i",i+1);
          p->drawText(x-mx,y1-my-10,8,10,Qt::AlignRight | Qt::AlignVCenter, qs);//Beschriftung dazu
        }
      }
    }
    for(int i=0;i<20;i++){//Bis maximal 20 Peaks auflisten
      if(peaklist.peak[i].frq != 0.0 and peaklist.bplist){
        //dxfrq = dmesskurve.frequenz[k]- dsettings.minX;
        //dydb = (dmesskurve.dbk1[k] + dsettings.Kurvenshift()) - dsettings.minY;
        dxfrq = peaklist.peak[i].frq - dsettings.minX;
        dydb = (peaklist.peak[i].dbm - dsettings.Displayshift()) - dsettings.minY;
        dydb *= -1.0;
        x= rect.left() + (dxfrq * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
        y1= rect.bottom() + (dydb * (rect.height()-1) / (dsettings.maxY-dsettings.minY));
        polylinem[0]=QPointF(x,y1);
        polylinem[1]=QPointF(x-mx,y1-my);
        polylinem[2]=QPointF(x+mx,y1-my);
        polylinem[3]=QPointF(x,y1);
        p->setPen(dsettings.penfmarken);
        p->drawEllipse(QPointF(x,y1),mx,mx);
        //p->drawPolyline(polylinem);
        qs.sprintf("%i",i+1);
        p->drawText(x-mx,y1-my-5,8,10,Qt::AlignRight | Qt::AlignVCenter, qs);
      }
    }
    if(dmesskurve.b3db){
      p->setPen(dsettings.pendotlinek);
      double dx3db = dmesskurve.frq3db1-dsettings.minX;
      x= rect.left() + (dx3db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
      dx3db = dmesskurve.frq3db2-dsettings.minX;
      x= rect.left() + (dx3db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
    }
    if(dmesskurve.b6db){
      p->setPen(dsettings.pendashdotdotlinek);
      double dx6db = dmesskurve.frq6db1-dsettings.minX;
      x= rect.left() + (dx6db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
      dx6db = dmesskurve.frq6db2-dsettings.minX;
      x= rect.left() + (dx6db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
    }
    if(dmesskurve.b60db){
      p->setPen(dsettings.pendashdotlinek);
      double dx60db = dmesskurve.frq60db1-dsettings.minX;
      x= rect.left() + (dx60db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
      dx60db = dmesskurve.frq60db2-dsettings.minX;
      x= rect.left() + (dx60db * (rect.width()-1) / (dsettings.maxX-dsettings.minX));
      p->drawLine(x, rect.top(), x, rect.bottom());
    }

    for(int i=0;i<6;i++){
      if(marker.bmarkerein[i]){
        QString qs1, qsf, qsk1, qsk2;
        qsf=frq2str(marker.frq[i]);
        qs.sprintf("M%i: ", i+1);
        qs=qs+qsf;
        if(bdeltadblinie and (dmesskurve.mktyp != mks11) and (dmesskurve.mktyp != mks11var)){
          qsk1=", [D";
        }else{
          qsk1=", [";
        }
        if(dmesskurve.bkanal){
          //dm = abs(marker.db1[i]*100.0);
          if(dmesskurve.bswv){
            if(marker.swv1[i]<1.1){
              if(dmesskurve.adaempfung>0.0){
                double swvant=db2swv(marker.db1[i]+ 2*dmesskurve.adaempfung);
                qsa.sprintf("%3.2f", marker.db1[i]);
                qsb=swv2str(marker.swv1[i]);
                qsc=swv2str(swvant);
                qs1=tr("%1 dB, SWV:%2, SWVant:%3]").arg(qsa).arg(qsb).arg(qsc);
              }else{
                qsa.sprintf("%3.2f", marker.db1[i]);
                qsb=swv2str(marker.swv1[i]);
                qs1=tr("%1 dB, SWV:%2]").arg(qsa).arg(qsb);
              }
            }else{
              if(dmesskurve.adaempfung>0.0){
                double swvant=db2swv(marker.db1[i]+ 2*dmesskurve.adaempfung);
                qsa.sprintf("%3.2f", marker.db1[i]);
                qsb=swv2str(marker.swv1[i]);
                qsc=swv2str(swvant);
                qs1=tr("%1 dB, SWV:%2, SWVant:%3]").arg(qsa).arg(qsb).arg(qsc);
              }else{
                qsa.sprintf("%3.2f", marker.db1[i]);
                qsb=swv2str(marker.swv1[i]);
                qs1=tr("%1 dB, SWV:%2]").arg(qsa).arg(qsb);
              }
            }
          }else{
            //qDebug()<<"dmesskurve.mktypk1:"<<dmesskurve.mktypk1;
            qsa.sprintf("%3.2f", marker.db1[i]);
            if(dsettings.isbdBm()){
              qs1=tr("%1dBm").arg(qsa);
              double mwatt = dbm2mwatt(marker.db1[i]);
              double volt = dbm2volt(marker.db1[i]);
              if(dsettings.isbwatt()){
                qsb=mwatt2str(mwatt);
                qs1=qs1+", "+qsb;
              }
              if(dsettings.isbvolt()){
                qsb=volt2str(volt);
                qs1=qs1+", "+qsb;
              }
            }else{
              qs1=tr("%1 dB").arg(qsa);
            }
            qs1=qs1+"]";
          }
          qs=qs+qsk1+qs1;
        }
        if(dhmesskurve1.bkanal and !bmarkerh1[i] and dhmesskurve1.bmarker){
          if(dsettings.isbdBm()){
            qs1.sprintf(", H1:[%3.2fdBm", marker.dbh1[i]);
            double mwatt = dbm2mwatt(marker.dbh1[i]);
            double volt = dbm2volt(marker.dbh1[i]);
            if(dhmesskurve1.bwatt){
              qsb=mwatt2str(mwatt);
              qs1=qs1+", "+qsb;
            }
            if(dhmesskurve1.bvolt){
              qsb=volt2str(volt);
              qs1=qs1+", "+qsb;
            }
          }else{
            qs1.sprintf(", H1:[%3.2f dB", marker.dbh1[i]);
            qsa="";
            if(dhmesskurve1.bswv){
              double swv= db2swv(marker.dbh1[i]);
              qsa=tr(",SWV:%1").arg(swv2str(swv));
            }
            qs1=qs1+qsa;
          }
          qs=qs+qs1+"]";
        }
        if(dhmesskurve2.bkanal and !bmarkerh2[i] and dhmesskurve2.bmarker){
          if(dsettings.isbdBm()){
            qs1.sprintf(", H2:[%3.2fdBm", marker.dbh2[i]);
            double mwatt = dbm2mwatt(marker.dbh2[i]);
            double volt = dbm2volt(marker.dbh2[i]);
            if(dhmesskurve2.bwatt){
              qsb=mwatt2str(mwatt);
              qs1=qs1+", "+qsb;
            }
            if(dhmesskurve2.bvolt){
              qsb=volt2str(volt);
              qs1=qs1+", "+qsb;
            }
          }else{
            qs1.sprintf(", H2:[%3.2f dB", marker.dbh2[i]);
            qsa="";
            if(dhmesskurve2.bswv){
              double swv= db2swv(marker.dbh2[i]);
              qsa=tr(",SWV:%1").arg(swv2str(swv));
            }
            qs1=qs1+qsa;
          }
          qs=qs+qs1+"]";
        }
        if(dhmesskurve3.bkanal and !bmarkerh3[i] and dhmesskurve3.bmarker){
          if(dsettings.isbdBm()){
            qs1.sprintf(", H3:[%3.2fdBm", marker.dbh3[i]);
            double mwatt = dbm2mwatt(marker.dbh3[i]);
            double volt = dbm2volt(marker.dbh3[i]);
            if(dhmesskurve3.bwatt){
              qsb=mwatt2str(mwatt);
              qs1=qs1+", "+qsb;
            }
            if(dhmesskurve3.bvolt){
              qsb=volt2str(volt);
              qs1=qs1+", "+qsb;
            }
          }else{
            qs1.sprintf(", H3:[%3.2f dB", marker.dbh3[i]);
            qsa="";
            if(dhmesskurve3.bswv){
              double swv= db2swv(marker.dbh3[i]);
              qsa=tr(",SWV:%1").arg(swv2str(swv));
            }
            qs1=qs1+qsa;
          }
          qs=qs+qs1+"]";
        }
        if(dhmesskurve4.bkanal and !bmarkerh4[i] and dhmesskurve4.bmarker){
          if(dsettings.isbdBm()){
            qs1.sprintf(", H4:[%3.2fdBm", marker.dbh4[i]);
            double mwatt = dbm2mwatt(marker.dbh4[i]);
            double volt = dbm2volt(marker.dbh4[i]);
            if(dhmesskurve4.bwatt){
              qsb=mwatt2str(mwatt);
              qs1=qs1+", "+qsb;
            }
            if(dhmesskurve4.bvolt){
              qsb=volt2str(volt);
              qs1=qs1+", "+qsb;
            }
          }else{
            qs1.sprintf(", H4:[%3.2f dB", marker.dbh4[i]);
            qsa="";
            if(dhmesskurve4.bswv){
              double swv= db2swv(marker.dbh4[i]);
              qsa=tr(",SWV:%1").arg(swv2str(swv));
            }
            qs1=qs1+qsa;
          }
          qs=qs+qs1+"]";
        }
        if(dhmesskurve5.bkanal and !bmarkerh5[i] and dhmesskurve5.bmarker){
          if(dsettings.isbdBm()){
            qs1.sprintf(", H5:[%3.2fdBm", marker.dbh5[i]);
            double mwatt = dbm2mwatt(marker.dbh5[i]);
            double volt = dbm2volt(marker.dbh5[i]);
            if(dhmesskurve5.bwatt){
              qsb=mwatt2str(mwatt);
              qs1=qs1+", "+qsb;
            }
            if(dhmesskurve5.bvolt){
              qsb=volt2str(volt);
              qs1=qs1+", "+qsb;
            }
          }else{
            qs1.sprintf(", H5:[%3.2f dB", marker.dbh5[i]);
            qsa="";
            if(dhmesskurve5.bswv){
              double swv= db2swv(marker.dbh5[i]);
              qsa=tr(",SWV:%1").arg(swv2str(swv));
            }
            qs1=qs1+qsa;
          }
          qs=qs+qs1+"]";
        }
        markerlist.append(qs);
      }
    }
    int px, py, pa=15;//pa=Zeilenabstand
    px = marker.textPosX();//Textposition oben links beginnend
    py = marker.textPosY();
    QFont font = p->font();
    font.setPixelSize(dsettings.markerfontsize);
    //qDebug()<<dsettings.markerfontsize;
    if(dsettings.bfontsize)p->setFont(font);
    QStringList list;
    list.clear();
    list=marker.strlist;
    if(dsettings.isbdBm() and peaklist.bplist){
      setpeaklist();
      list += peaklist.plist;
    }
    list += markerlist;
    p->setPen(dsettings.penmarkerschrift);
    for(int i=0; i<list.size(); i++){
      qs = list.at(i);
      p->drawText(px, py, rect.width(), 20,Qt::AlignLeft | Qt::AlignVCenter, qs);
      py+=pa;
    }
    emit sendinfolist(list);
  }else{
    //Kalibrierablaeufe
    if(dmesskurve.calablauf==4){
      qs=dmesskurve.calstring;
      p->setPen(dsettings.penschrift);
      QFont font = p->font();
      font.setPixelSize(20);
      if(dsettings.bfontsize)p->setFont(font);
      p->drawText(Margin, rect.height()/2, rect.width(), 150,Qt::AlignCenter | Qt::AlignVCenter, qs);
    }else{
      int calidxmax=dmesskurve.mpunkte-1;
      qs=tr("Kalibrieren!");
      switch (dmesskurve.calablauf) {
      case 1:qsa=tr("Wobbeln Messpunkt1");break;
      case 2:qsa=tr("Wobbeln Messpunkt2");break;
      case 3:qsa=tr("Wobbeln VFO-Frequenzgang");break;
      default:
        break;
      }
      qs2=tr("Siehe auch Fortschrittbalken unten!");
      qs1=tr("Datensatz: %1/%2").arg(calidxmax).arg(dmesskurve.calindex);
      p->setPen(dsettings.penschrift);
      QFont font = p->font();
      font.setPixelSize(20);
      if(dsettings.bfontsize)p->setFont(font);
      p->drawText(Margin, rect.height()/2-75, rect.width(), 40,Qt::AlignCenter | Qt::AlignVCenter, qs);
      p->drawText(Margin, rect.height()/2-25, rect.width(), 40,Qt::AlignCenter | Qt::AlignVCenter, qsa);
      p->drawText(Margin, rect.height()/2+25, rect.width(), 40,Qt::AlignCenter | Qt::AlignVCenter, qs1);
      p->drawText(Margin, rect.height()/2+75, rect.width(), 40,Qt::AlignCenter | Qt::AlignVCenter, qs2);
    }
  }
  p->setClipRect(rect.adjusted(0,Margin*-1,Margin,0));
  p->setClipRect(rect);
  if(bd)qDebug("WDisplay::drawKurve(QPainter *p) ENDE");
}

void WDisplay::setPlotSettings(const PlotSettings &settings){
  if(bd)qDebug("WDisplay::setPlotSettings(const PlotSettings &settings)");
  double dshift=0.0;
  PlotSettings asettings=settings;
  if(dsettings.Displayshift() != asettings.Displayshift()){
    //markerclrall();
    dshift=dsettings.Displayshift()-asettings.Displayshift();
    //qDebug("dshift= %f",dshift);
    //qDebug("dbmgrenze= %f",dbmgrenze);
    if(peaklist.bplist)dbmgrenze-=dshift;
    //qDebug("dbmgrenze= %f",dbmgrenze);
  }
  dsettings=settings;
  if(dsettings.isbdBm()){
    apeak_liste->setVisible(true);
    amarker_clr_all->setText("CLR all Frequenz Marker/Peaklist");
  }else{
    apeak_liste->setVisible(false);
    amarker_clr_all->setText("CLR all Frequenz Marker");
  }
  QFont font = this->font();
  if(dsettings.bfontsize){
    font.setPixelSize(dsettings.fontsize);
    this->setFont(font);
    popupmenu->setFont(font);
  }
  refreshPixmap();
  if(bd)qDebug("WDisplay::setPlotSettings(const PlotSettings &settings) ENDE");
}

void WDisplay::setMessKurve(const TMessKurve &messkurve){
  if(bd)qDebug("WDisplay::setMessKurve(const TMessKurve &messkurve)");
  dmesskurve=messkurve;
  if(dmesskurve.bkurvegueltig){
    amarker_plus->setEnabled(true);
    apeak_liste->setEnabled(true);
    akurveclr->setEnabled(true);
    akabellaenge->setEnabled(true);
    akabeldaten->setEnabled(true);
    akabelimpedanz->setEnabled(true);
  }else{
    amarker_plus->setEnabled(false);
    apeak_liste->setEnabled(false);
    akurveclr->setEnabled(false);
  }
  //qDebug()<<"1dmesskurve.mktypk1:"<<dmesskurve.mktypk1;
  //qDebug()<<"1dmesskurve.mktypk2:"<<dmesskurve.mktypk2;
  refreshPixmap();
  refreshKurve();
  if(bd)qDebug("WDisplay::setMessKurve(const TMessKurve &messkurve) ENDE");
}

void WDisplay::setHMessKurve(THMessKurve dhm, int idx){
  if(bd)qDebug("WDisplay::setHMessKurve(THMessKurve dhm, int idx)");
  switch(idx){
  case 1:dhmesskurve1=dhm;break;
  case 2:dhmesskurve2=dhm;break;
  case 3:dhmesskurve3=dhm;break;
  case 4:dhmesskurve4=dhm;break;
  case 5:dhmesskurve5=dhm;break;
  default:dhmesskurve1=dhm;break;
  }
  refreshKurve();
  if(bd)qDebug("WDisplay::setHMessKurve(THMessKurve dhm, int idx) ENDE");
}


void WDisplay::setFrqMarken(const TFrqmarken &f){
  if(bd)qDebug("WDisplay::setFrqMarken(const TFrqmarken &f)");
  frqmarken=f;
  refreshPixmap();
  if(bd)qDebug("WDisplay::setFrqMarken(const TFrqmarken &f) ENDE");
}


QString WDisplay::frq2str(double d){
  if(bd)qDebug("WDisplay::frq2str(double d)");
  QString me=" GHz";
  QString qs;
  double w=d/1000.0;
  int l;

  qs.sprintf("%3.9f",w);
  if(w<1.0){
    me=" MHz";
    w=w*1000.0;
    qs.sprintf("%3.6f",w);
  }
  if(w<1.0){
    me=" kHz";
    w=w*1000.0;
    qs.sprintf("%3.3f",w);
  }
  if(w<1.0){
    me=" Hz";
    w=w*1000.0;
    qs.sprintf("%3.0f",w);
  }
  l=qs.length();
  while(qs.at(l-1)=='0'){
    qs=qs.left(l-1);
    l=qs.length();
  }
  if(qs.at(l-1)=='.')qs=qs.left(l-1);
  qs=qs+me;
  if(bd)qDebug("WDisplay::frq2str(double d) ENDE");
  return(qs);
}

void WDisplay::fkabellaenge(){
  QString beschr(tr("<b>Ermittlung der Kabell&auml;nge</b><br>"
                    "<br>"
                    "Es gibt 2 M&ouml;glichkeiten:<br>"
                    "1. Das Kabelende offen lassen und einen Serienwiderstand<br>"
                    "   von 50 Ohm zwischen Ausgang (DUT) des NWT2 und Seele<br>"
                    "   des Kabels einfuegen.<br>"
                    "2. Das Kabelende kurzschlie&szlig;en und am Ausgang (DUT) des NWT2<br>"
                    "   ein T-St&uuml;ck mit 50 Ohm Abschluss und das zu messende Kabel<br>"
                    "   anstecken.<br>"
                    "<br>"
                    "Nach Auswahl und entsprechender Beschaltung wird mit dem<br> "
                    "SWV-Messkopf die S11 Kurve aufgenommen. In jedem dB-Minimum,<br> "
                    "beginnend bei der niedrigsten Frequenz, wird je ein Marker gesetzt.<br>"
                    "Aus dem Frequenzpunkt des ersten Markers und den Frequenzabstand<br>"
                    "eventuell weiterer Marker wird die \"L&auml;ngenfrequenz\"<br>"
                    "berechnet. Bei gr&ouml;&szlig;eren Kabell&auml;ngen ist es erforderlich<br> "
                    "die Endfrequenz zu verringern um genaue Messergebnisse zu erhalten. Es k&ouml;nnen<br>"
                    "bis zu 6 Marker gesetzt werden."
                    "<br>"
                    "<br>"
                    "Verk&uuml;rzungsfaktor des Kabels (Vorgabe = PE-Kabel):","InputDialog in der Grafik"));
  QString qs, qer, qf;
  double er;
  bool ok=false;
  bool bmreihe=true;

  double cfrq=0.0;
  int zaehler=0;
  if(marker.bmarkerein[0]){
    cfrq=marker.frq[0];
    zaehler++;
    ok=true;
  }
  for(int i=1;i<6;i++){
    if(marker.bmarkerein[i]){
      if(marker.frq[i]<marker.frq[i-1])bmreihe=false;
      cfrq=cfrq + (marker.frq[i]-marker.frq[i-1])/2.0;
      zaehler++;
      ok=true;
    }
  }
  if(!bmreihe){
    QMessageBox::warning( this, tr("Marker Reihenfolge!","InfoBox in der Grafik"), tr("Bitte die Reihenfolge beim Setzen der Marker-Nummern einhalten!","InfoBox in der Grafik"));
  }else{
    if(ok){
      double v=kabeldaten.vf;
      if(v==0.0)v=0.67;
      v = QInputDialog::getDouble(this, tr("Verkuerzungsfaktor","InputDialog in der Grafik"),
                                         beschr, v, -10000, 10000, 3, &ok);
      if(!ok)return;
      cfrq = cfrq/ double(zaehler);
      er = v / cfrq * 75.0;
      qer.sprintf("%1.3f",er);
      qf.sprintf("%1.3f MHz",cfrq*2.0);
      qs = "<B>Errechnung der Kabell&auml;nge</B><BR>"
           "L&auml;ngenfrequenz: " + qf +"<BR>"+
           "Geometrische L&auml;nge des Kabels: " + qer + " m";
      QMessageBox::information (this, tr("Geometrische Laenge","InfoBox in der Grafik"), qs);
      //akabelimpedanz->setEnabled(true);
      kabeldaten.laenge = er;
      kabeldaten.lfrequenz = cfrq*2.0;
      kabeldaten.vf = v;
      emit kdaten(kabeldaten);
    }else{
      QMessageBox::warning( this, tr("Marker nicht gesetzt!","InfoBox in der Grafik"), tr("Bitte zuerst bei jedem dB-Minimum einen Marker setzen. Beginnend bei der niedrigsten Frequenz."));
    }
  }
}

void WDisplay::fkabelimpedanz(){
  QString beschr(tr("<b>Berechnung der Kabelimpedanz</b><br>"
                    "<br>"
                    "Es gibt 2 M&ouml;glichkeiten:<br>"
                    "1. Das Kabelende offen lassen und einen Serienwiderstand<br>"
                    "   von 50 Ohm zwischen Ausgang (DUT) des NWT2 und Seele<br>"
                    "   des Kabels einfuegen.<br>"
                    "2. Das Kabelende kurzschlie&szlig;en und am Ausgang (DUT) des NWT2<br>"
                    "   ein T-St&uuml;ck mit 50 Ohm Abschluss und das zu messende Kabel<br>"
                    "   anstecken.<br>"
                    "<br>"
                    "Nach Auswahl und entsprechender Beschaltung wird mit dem<br> "
                    "SWV-Messkopf die S11 Kurve aufgenommen. In jedem dB-Minimum,<br> "
                    "beginnend bei der niedrigsten Frequenz, wird je ein Marker gesetzt.<br>"
                    "Aus dem Frequenzpunkt des ersten Markers und den Frequenzabstand<br>"
                    "eventuell weiterer Marker wird die \"L&auml;ngenfrequenz\"<br>"
                    "berechnet. Bei gr&ouml;&szlig;eren Kabell&auml;ngen ist es erforderlich<br> "
                    "die Endfrequenz zu verringern um genaue Messergebnisse zu erhalten. Es k&ouml;nnen<br>"
                    "bis zu 6 Marker gesetzt werden."
                    "<br>"
                    "<br>"
                    "Bitte mit einem Kapazit&auml;tsmessger&auml;t<br>"
                    "die Kapazit&auml;t des Kabels (Zweidrahtleitung)<br>"
                    "messen.<br>"
                    "-------------------------------------------------------<br>"
                    "<br><br>"
                    "Kapazit&auml;t des Kabels (pF):","Inputdialog in der Grafik"));
  QString qs, qer, qf;
  //double er;
  bool ok=false;
  bool bmreihe=true;

  double cfrq=0.0;
  int zaehler=0;
  if(marker.bmarkerein[0]){
    cfrq=marker.frq[0];
    zaehler++;
    ok=true;
  }
  for(int i=1;i<6;i++){
    if(marker.bmarkerein[i]){
      if(marker.frq[i]<marker.frq[i-1])bmreihe=false;
      cfrq=cfrq + (marker.frq[i]-marker.frq[i-1])/2.0;
      zaehler++;
      ok=true;
    }
  }
  if(!bmreihe){
    QMessageBox::warning( this, tr("Marker Reihenfolge!","InfoBox in der Grafik"), tr("Bitte die Reihenfolge beim Setzen der Marker-Nummern einhalten!","InfoBox in der Grafik"));
  }else{
    if(ok){
      QString qs, qimp;
      double c, imp;
      bool bok=false;
      cfrq = cfrq/ double(zaehler);
      kabeldaten.lfrequenz = cfrq*2.0;
      c = QInputDialog::getDouble(this, tr("Kapazit&auml;t (pF):","Inputdialog in der Grafik"),
                                  beschr, kabeldaten.ca, 0, 10000, 1, &bok);
      if(!bok)return;
      imp = 500000.0 / (c * kabeldaten.lfrequenz);
      qimp.sprintf("%1.1f Ohm",imp);
      qs = tr("<B>Die Kabel-Impedanz betr&auml;gt:</B><BR>"
           "|Z|: %1").arg(qimp);
      QMessageBox::information( this, tr("Impedanz","InfoBox in der Grafik"), qs);
      kabeldaten.z = imp;
      kabeldaten.ca = c;
      emit kdaten(kabeldaten);
    }else{
      QMessageBox::warning( this, tr("Marker nicht gesetzt!","InfoBox in der Grafik"), tr("Bitte zuerst bei jedem dB-Minimum einen Marker setzen. Beginnend bei der niedrigsten Frequenz."));
    }
  }
}

void WDisplay::fkabeldaten(){
  QString beschr(tr("<b>Ermittlung der Kabeldaten</b><br>"
                    "<br>"
                    "Es gibt 2 M&ouml;glichkeiten:<br>"
                    "1. Das Kabelende offen lassen und einen Serienwiderstand<br>"
                    "   von 50 Ohm zwischen Ausgang (DUT) des NWT2 und Seele<br>"
                    "   des Kabels einfuegen.<br>"
                    "2. Das Kabelende kurzschlie&szlig;en und am Ausgang (DUT) des NWT2<br>"
                    "   ein T-St&uuml;ck mit 50 Ohm Abschluss und das zu messende Kabel<br>"
                    "   anstecken.<br>"
                    "<br>"
                    "Nach Auswahl und entsprechender Beschaltung wird mit dem<br> "
                    "SWV-Messkopf die S11 Kurve aufgenommen. In jedem dB-Minimum,<br> "
                    "beginnend bei der niedrigsten Frequenz, wird je ein Marker gesetzt.<br>"
                    "Aus dem Frequenzpunkt des ersten Markers und den Frequenzabstand<br>"
                    "eventuell weiterer Marker wird die \"L&auml;ngenfrequenz\"<br>"
                    "berechnet. Bei gr&ouml;&szlig;eren Kabell&auml;ngen ist es erforderlich<br> "
                    "die Endfrequenz zu verringern um genaue Messergebnisse zu erhalten. Es k&ouml;nnen<br>"
                    "bis zu 6 Marker gesetzt werden."
                    "<br>"
                    "<br>"
                    "Gemessene L&auml;nge des Kabels (m):","Inputdialog in der Grafik"));
  QString qs, qv, qe, qf;
  double v, e;
  bool ok=false;
  double cfrq=0.0;
  int zaehler=0;
  bool bmreihe=true;

  if(marker.bmarkerein[0]){
    cfrq=marker.frq[0];
    zaehler++;
    ok=true;
  }
  for(int i=1;i<6;i++){
    if(marker.bmarkerein[i]){
      if(marker.frq[i]<marker.frq[i-1])bmreihe=false;
      cfrq=cfrq + (marker.frq[i]-marker.frq[i-1])/2.0;
      zaehler++;
      ok=true;
    }
  }
  if(!bmreihe){
    QMessageBox::warning( this, tr("Marker Reihenfolge!","InfoBox in der Grafik"), tr("Bitte die Reihenfolge beim Setzen der Marker-Nummern einhalten!"));
  }else{
    if(ok){
      double l = QInputDialog::getDouble(this, tr("Kabellaenge","Inputdialog in der Grafik"),
                                         beschr, kabeldaten.laenge, 0, 10000, 3, &ok);
      if(!ok)return;
      cfrq = cfrq/ double(zaehler);
      v = 0.0133 * l * (cfrq);
      e = 1 / (v * v);
      qv.sprintf("%1.3f",v);
      qe.sprintf("%1.3f",e);
      qf.sprintf("%1.6f MHz",cfrq*2.0);
      qs = tr("<B>Errechnung der Kabeldaten</B><BR>"
              "L&auml;ngenfrequenz: %1<BR>"
              "Verk&uuml;rzungsfaktor: %2<BR>"
              "Dielektrizit&auml;tszahl: %3").arg(qf).arg(qv).arg(qe);
      QMessageBox::information( this, tr("Kabeldaten","InfoBox in der Grafik"), qs);
      akabelimpedanz->setEnabled(true);
      kabeldaten.dk = e;
      kabeldaten.lfrequenz = cfrq*2.0;
      kabeldaten.vf = v;
      kabeldaten.laenge = l;
      emit kdaten(kabeldaten);
    }else{
      QMessageBox::warning( this, tr("Marker nicht gesetzt!","InfoBox in der Grafik"), tr("Bitte zuerst bei jedem dB-Minimum einen Marker setzen. Beginnend bei der niedrigsten Frequenz."));
    }
  }
}

void WDisplay::frequenzuebergabe(){
  if(bd)qDebug("WDisplay::frequenzuebergabe()");
  bfrquebergabe=true;
  setCursor(Qt::CrossCursor);
  if(bd)qDebug("WDisplay::frequenzuebergabe() ENDE");
}

void WDisplay::dsetfrqanfang(){
  if(bd)qDebug("WDisplay::dsetfrqanfang()");
  bfrqanfang=true;
  setCursor(Qt::CrossCursor);
  if(bd)qDebug("WDisplay::dsetfrqanfang() ENDE");
}

void WDisplay::dsetfrqende(){
  if(bd)qDebug("WDisplay::dsetfrqende()");
  bfrqende=true;
  setCursor(Qt::CrossCursor);
  if(bd)qDebug("WDisplay::dsetfrqende() ENDE");
}

void WDisplay::dsetfrqmitte(){
  if(bd)qDebug("WDisplay::dsetfrqmitte()");
  bfrqmitte=true;
  setCursor(Qt::CrossCursor);
  if(bd)qDebug("WDisplay::dsetfrqmitte() ENDE");
}

void WDisplay::kurveclr(){
  if(bd)qDebug("WDisplay::kurveclr()");
  emit wkurveclr();
  dmesskurve.bkurvegueltig=false;
  refreshPixmap();
  repaint();
  if(bd)qDebug("WDisplay::kurveclr() ENDE");
}

void WDisplay::markerplus(){
  if(bd)qDebug("WDisplay::markerplus()");
  if(marker.addMarker()){
    setCursor(Qt::CrossCursor);
    amarker_delta_dblinie->setEnabled(true);
    amarker_clr_all->setEnabled(true);
    bmarkerplus=true;
  }else{
    bmarkerplus=false;
    amarker_plus->setEnabled(false);
  }
  if(bd)qDebug("WDisplay::markerplus() ENDE");
}

void WDisplay::markerdeltadblinie(){
  if(bd)qDebug("WDisplay::markerdeltadblinie()");
  if(bdeltadblinie){
    bdeltadblinie=false;
    amarker_delta_dblinie->setChecked(false);
  }else{
    bdeltadblinie=true;
    amarker_delta_dblinie->setChecked(true);
  }
  setTitleDeltamenue();
  refreshPixmap();
  repaint();
  if(bd)qDebug("WDisplay::markerdeltadblinie() ENDE");
}

void WDisplay::setpeaklist(){
  double d, maxfrq, maxdbm=dbmgrenze, f=0.0;
  bool banf=false, bende=true;
  QString qs,qsfrq, qsdbm, qsidx;
  int idx1=0, idx2=0, i, j;
  double frq1array[20];
  double frq2array[20];
  double dbm1array[20];
  double dbm2array[20];

  peaklist.plist.clear();//Stringliste loeschen
  peaklist.bplist=true;
  maxdbm=dbmgrenze;
  for(i=0; i<20; i++){//alles auf NULL setzen
    frq1array[i]=0.0;
    frq2array[i]=0.0;
    dbm1array[i]=0.0;
    dbm2array[i]=0.0;
    peaklist.peak[i].frq=0.0;
    peaklist.peak[i].dbm=0.0;
    peaklist.peak[i].pos.setX(0);
    peaklist.peak[i].pos.setY(0);
  }

  for(i=0; i<dmesskurve.mpunkte; i++){//die ganze Messkurve durchschauen
    d=dmesskurve.dbk1[i]+dsettings.Displayshift()+dsettings.Kurvenshift();
    if(dbmgrenze < d){//Pegel > dBm-Grenze
      if(banf==false and bende==true){
        banf=true;
        bende=false;
      }
      if(banf==true and bende==false){
        if(maxdbm < d){//Pegel > dBm max
          maxdbm=d;//dBm-max = Pegel
          maxfrq=dmesskurve.frequenz[i];//frq-max = Frequenz
        }
      }
    }else{//Pegel <= dBm-Grenze
      bende=true;//
      if(banf==true and bende==true){//Pegel fällt wieder ab
        //qs.sprintf("%f, %3.2fdBm",maxfrq,maxdbm);
        //qDebug()<<qs;
        banf=false;
        dbm1array[idx1]=maxdbm;//Peak dBm merken
        frq1array[idx1]=maxfrq;//Peak frq merken
        maxdbm=dbmgrenze;//dBm-max = dBm-Grenze
        idx1++;//index++
        if(idx1>19)idx1=19;//index zu gross max=19
      }
    }
  }
  //ganze Kurve wurde durchforstet
  for(i=0; i<19; i++){
    d=dbm1array[i+1]-dbm1array[i];
    f=frq1array[i+1]-frq1array[i];
    if(d<0.0)d*=-1.0;
    //qDebug("Differenz= %f dB",d);
    //qDebug("Differenz= %f MHz",f);
    if((f < 0.8) and (f > 0.0) and d < 3.0){//Frequenzabstand zwischen 0 und 700kHz, Pegeldifferenz kleiner 3dB
      frq1array[i]=0.0;//linken Peak auf frq=0.0 setzen
      dbm1array[i]=0.0;//linken Peak auf dBm=0.0 setzen
    }
  }
  idx2=0;//index2 = 0
  for(i=0; i<20; i++){
    if(frq1array[i] != 0.0){//Peak umspeichern
      frq2array[idx2]=frq1array[i];//frq2 ist frq1
      dbm2array[idx2]=dbm1array[i];//dBm2 ist dBm1
      idx2++;//index2++
      if(idx2>19)idx2=19;//index2 maximum erreicht
    }
  }
  //sortieren
  TPeak peaktemp;
  for(i=0; i<idx2; i++){
    for(j=idx2-1; j>=i; j--){
      if(dbm2array[j] > dbm2array[j-1]){
        peaktemp.dbm=dbm2array[j];
        peaktemp.frq=frq2array[j];
        dbm2array[j]=dbm2array[j-1];
        frq2array[j]=frq2array[j-1];
        dbm2array[j-1]=peaktemp.dbm;
        frq2array[j-1]=peaktemp.frq;
      }
    }
  }
  for(int i=0; i<20; i++){//array2 anzeigen
    maxfrq=frq2array[i];
    maxdbm=dbm2array[i];
    if(maxfrq!=0.0){//Peak vorhanden
      double mwatt = dbm2mwatt(maxdbm);
      double volt = dbm2volt(maxdbm);
      qsfrq=frq2str(maxfrq);
      qsdbm.sprintf("%3.2f dBm",maxdbm);
      qsidx.sprintf("%i",i+1);
      QString qsmwatt=mwatt2str(mwatt);
      QString qsvolt=volt2str(volt);
      qs=tr("[%1] %2, %3").arg(qsidx).arg(qsfrq).arg(qsdbm);
      if(dsettings.isbwatt())qs+=tr(", %1").arg(qsmwatt);
      if(dsettings.isbvolt())qs+=tr(", %1").arg(qsvolt);
      //qDebug()<<qs;
      peaklist.plist.append(qs);
      peaklist.peak[i].frq=maxfrq;//wird gebraucht fuer Markerposition
      peaklist.peak[i].dbm=maxdbm;
    }
  }
}

void WDisplay::sdBmGrenze(double d){
  dbmgrenze=d;
  setpeaklist();
  amarker_clr_all->setEnabled(true);
  refreshPixmap();
  repaint();
}

void WDisplay::setdbmgrenze(){
  double d, dmax, dmin;

  if(!peaklist.bplist){
    dmax=-100.0; dmin=100.0; d=0.0;
    for(int i=0; i<dmesskurve.mpunkte; i++){
      d=dmesskurve.dbk1[i]+dsettings.Displayshift()+dsettings.Kurvenshift();
      if(d>dmax)dmax=d;
      if(d<dmin)dmin=d;
    }
    //qDebug("min:%f dBm, max:%f dBm", dmin, dmax);
    dbmgrenze=round(dmin+((dmax-dmin)*0.2));
  }

  QPeakListDlg *pldlg = new QPeakListDlg(this);
  //Verbindungen dlg->Display hestellen
  QObject::connect( pldlg, SIGNAL( edBmGrenze(double)), this, SLOT(sdBmGrenze(double)));
  pldlg->setdBmGrenze(dbmgrenze);
  int r=pldlg->exec();
  if(r == QDialog::Accepted){
    dbmgrenze=pldlg->getdBmGrenze();
    setpeaklist();
    amarker_clr_all->setEnabled(true);
    refreshPixmap();
    repaint();
  }
  QObject::disconnect( pldlg, SIGNAL( edBmGrenze(double)), this, SLOT(sdBmGrenze(double)));
  delete pldlg;
}

void WDisplay::setTitleDeltamenue(){
  QString mtitle="";
  int c=0;
  if(amarker_delta_dblinie->isChecked())c=c+1;
  if(c==0){
    mtitle="Delta";
  }else{
    mtitle="Delta [";
    if(amarker_delta_dblinie->isChecked()){
      if(c==1){
        mtitle=mtitle+"dB-Linie";
      }else{
        mtitle=mtitle+", dB-Linie";
      }
    }
    mtitle=mtitle+"]";
  }
  popupmenu->setTitle(mtitle);
}

void WDisplay::markerclrall(){
  if(bd)qDebug("WDisplay::markerclrall()");
  marker.clrMarker();
  amarker_delta_dblinie->setEnabled(false);
  amarker_clr_all->setEnabled(false);
  amarker_plus->setEnabled(true);
  peaklist.bplist=false;
  refreshPixmap();
  repaint();
  if(bd)qDebug("WDisplay::markerclrall() ENDE");
}

void WDisplay::markertextmove(){
  if(bd)qDebug("WDisplay::markertextmove()");
  bmarkertextmove=true;
  setCursor(Qt::PointingHandCursor);
  if(bd)qDebug("WDisplay::markertextmove() ENDE");
}

void WDisplay::writeSettings()
{
  if(bd)qDebug("WDisplay::writeSettings()");
  //QSettings settings("AFU", "NWT2");
  QSettings settings("AFU", dsettings.settingspath);
  settings.setValue("markertextpos", marker.MarkertextPos());
  if(bd)qDebug("WDisplay::writeSettings() ENDE");
}

void WDisplay::readSettings()
{
  if(bd)qDebug("WDisplay::readSettings()");
  //QSettings settings("AFU", "NWT2");
  QSettings settings("AFU", dsettings.settingspath);
  QPoint pos = settings.value("markertextpos", QPoint(70, 100)).toPoint();
  marker.setMarkertextPos(pos);
  if(bd)qDebug("WDisplay::readSettings() ENDE");
}

void WDisplay::addMarkerStrList(QString s){
  //qDebug("WDisplay::addMarkerStrList(QString s)");
  //qDebug()<<s;
  if(bd)qDebug("WDisplay::addMarkerStrList(QString s)");
  marker.addMarkerStrList(s);
  if(bd)qDebug("WDisplay::addMarkerStrList(QString s) ENDE");
}

void WDisplay::clrMarkerStrList(){
  //qDebug("WDisplay::clrMarkerStrList()");
  if(bd)qDebug("WDisplay::clrMarkerStrList()");
  marker.clrMarkerStrList();
  if(bd)qDebug("WDisplay::clrMarkerStrList() ENDE");
}

void WDisplay::bildspeichern(QString str)
{
  if(bd)qDebug("WDisplay::bildspeichern(QString str)");
  QString qs, qs1;
  QString s=str;


  QSettings settings("AFU", dsettings.settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir imgpath;
  //Dateiname uebernommen vom "Caption" der Wobbelkurve
  if(!s.contains(".png"))s+=".png";
  imgpath.setPath(settings.value("imgpath", QDir::homePath()).toString());
  s = QFileDialog::getSaveFileName(this, tr("Bild Speichern","Grafik Filedialog"),
                                   imgpath.filePath(s),"PNG (*.png *.PNG);;BMP (*.bmp *.BMP);;JPG (*.jpg *.JPG)", &qs1);

  if(!s.isEmpty()){
    //qDebug("save");
    //Datei ueberpruefen ob Sufix vorhanden
    if(!s.contains(".")){
      if(qs1.contains("PNG"))s += ".png";
      if(qs1.contains("BMP"))s += ".bmp";
      if(qs1.contains("JPG"))s += ".jpg";
    }
    QPixmap pixb = QPixmap(width(),height()+15);
    pixb.fill(dsettings.colorhintergrund);
    QPainter p;
    p.begin(&pixb);//Beginn der Zeichnungen im Pixma
    QFont f=p.font();
    f.setPixelSize(10);
    p.setFont(f);
    p.setPen(dsettings.pensolidline);
    QString qsv;
    switch(dsettings.hwvariante){
    case vnwt4_1:qsv="NWT4000-1"; break;
    case vnwt4_2:qsv="NWT4000-2"; break;
    case vnwt6:qsv="NWT6000"; break;
    case vnwt_ltdz:qsv="LTDZ 35-4400M"; break;
    case vnwt_nn:qsv="NWT noname"; break;
    default:qsv="NWT noname"; break;
    }
    QString sdt;
    QDateTime dtime=QDateTime::currentDateTime();
    QString swname;
#ifdef Q_OS_WIN
    swname ="NWT4000win";
#else
    swname ="NWT4000lin";
#endif
    sdt=dtime.toString("d.MMMM yyyy hh:mm");
    qs=tr("%1").arg(sdt);
    p.drawText(pixb.rect().left()+10, pixb.rect().bottom()-20, 150, 20, Qt::AlignLeft | Qt::AlignVCenter, qs);
    qs=tr("HW: %1, %2 Version: %3, (c)DL4JAL").arg(qsv).arg(swname).arg(dsettings.infoversion);
    p.drawText(pixb.rect().right()-280, pixb.rect().bottom()-20, 280, 20, Qt::AlignLeft | Qt::AlignVCenter, qs);
    drawBeschriftung(&p);//Beschriftung Wobbelfenster erzeugen
    drawKurve(&p);//Kurve Wobbelfenster zeichen
    p.end();//Ende der Zeichnungen im Paintfenster
    QImage img(pixb.size(), QImage::Format_RGB32);//neues Image erzeugen
    img=pixb.toImage();
    QImageWriter iw(s);
    iw.write(img);
    imgpath.setPath(s);
    s=imgpath.dirName();//Filename ermitteln
    qs=imgpath.absolutePath();//Path mit Filename
    qs.remove(s);//Filename aus Path entfernen
    settings.setValue("imgpath",qs);
  }
  if(bd)qDebug("WDisplay::bildspeichern(QString str) ENDE");
}

void WDisplay::setEnabled(bool b){
  if(bd)qDebug("WDisplay::setEnabled(bool b)");
  popupmenu->setEnabled(b);
  if(bd)qDebug("WDisplay::setEnabled(bool b) ENDE");
}

double WDisplay::db2swv(double d){
  if(bd)qDebug("WDisplay::db2swv(double d)");
  double db=d;
  double ref;
  double swv;
  ref=pow(10.0,db/20.0);
  swv=(1.0+ref)/(1.0-ref);
  if(bd)qDebug("WDisplay::db2swv(double d) ENDE");
  return swv;
}

double WDisplay::dbm2mwatt(double d){
  double mwatt = pow(10.0 , d/10.0);
  return mwatt;
}

double WDisplay::dbm2volt(double d){
  double volt = sqrt(pow(10.0 , d/10.0) * 0.05);
  return volt;
}

QString WDisplay::mwatt2str(double d){
  QString qs="0.0 ";
  QString mh="";
  double w=0.0;
  bool g=true;

  if(g and d>1000000.0){w=d/1000000.0;mh="kW";g=false;}
  if(g and d>1000.0){w=d/1000.0;mh="W";g=false;}
  if(g and d>1){w=d;mh="mW";g=false;}
  if(g and d>0.001){w=d*1000.0;mh="uW";g=false;}
  if(g and d>0.000001){w=d*1000000.0;mh="nW";g=false;}
  if(g and d>0.000000001){w=d*1000000000.0;mh="pW";g=false;}
  if(g and d>0.000000000001){w=d*1000000000000.0;mh="fW";g=false;}
  if(g and d>0.000000000000001){w=d*1000000000000000.0;mh="aW";g=false;}

  if(w>100){
    qs.sprintf("%3.1f ",w);
    return qs+mh;
  }
  if(w>10){
    qs.sprintf("%2.2f ",w);
    return qs+mh;
  }
  if(w>1){
    qs.sprintf("%1.3f ",w);
    return qs+mh;
  }
  return qs+mh;
}

QString WDisplay::volt2str(double d){
  QString qs="0.0 ";
  QString mh="";
  double w=0.0;
  bool g=true;

  if(g and d>1){w=d;mh="V";g=false;}
  if(g and d>0.001){w=d*1000.0;mh="mV";g=false;}
  if(g and d>0.000001){w=d*1000000.0;mh="uV";g=false;}
  if(g and d>0.000000001){w=d*1000000000.0;mh="nV";g=false;}
  if(w>100){
    qs.sprintf("%3.1f ",w);
    return qs+mh;
  }
  if(w>10){
    qs.sprintf("%2.2f ",w);
    return qs+mh;
  }
  if(w>1){
    qs.sprintf("%1.3f ",w);
    return qs+mh;
  }
  return qs+mh;
}

QString WDisplay::swv2str(double d){
  QString qs;
  if(d < 1.1){
    qs.sprintf("%2.3f",d);
  }else{
    qs.sprintf("%2.2f",d);
  }
  return qs;
}

//--------------------------------------------------------------------------------------------------------------------------------
QMarker::QMarker(){
  clrMarker();
  strlist.clear();
  textpoint.setX(70);
  textpoint.setY(70);
}

bool QMarker::addMarker(){
  m++;
  if(m>5){
    m=5;
    return false;
  }else{
    bmarkerein[m]=true;
    return true;
  }
  bkeinmarker=false;
}

void QMarker::clrMarker(){
  for(int i=0;i<6;i++){
    frq[i]=0.0;
    db1[i]=-100.0;
    db2[i]=-100.0;
    swv1[i]=0.0;
    swv2[i]=0.0;
    dbh1[i]=-100.0;
    dbh2[i]=-100.0;
    dbh3[i]=-100.0;
    dbh4[i]=-100.0;
    dbh5[i]=-100.0;
    swvh1[i]=0.0;
    swvh2[i]=0.0;
    swvh3[i]=0.0;
    swvh4[i]=0.0;
    swvh5[i]=0.0;
    point[i].setX(0);
    point[i].setY(0);
    bmarkerein[i]=false;
  }
  m=-1;
  bkeinmarker=true;
}

void QMarker::setFrq(double f){
  if(m!=-1)frq[m]=f;
}


void QMarker::setFrqIdx(int idx,double f){
  if(idx>=0 and idx<6)frq[idx]=f;
}

void QMarker::setPosIdx(QPoint pos, int idx){
  if(idx>=0 and idx<6) point[idx]=pos;
}

void QMarker::setPos(QPoint pos){
  if(m!=-1)point[m]=pos;
}

int QMarker::posX(){
  return point[m].x();
}

void QMarker::setMarkertextPos(QPoint pos){
  textpoint=pos;
}

void QMarker::setdB1(int idx,double d){
  if(m!=-1){
    db1[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swv1[idx]=(1.0+w)/(1.0-w);
    }else{
      swv1[idx]=0.0;
    }
  }
}

void QMarker::setdB2(int idx,double d){
  if(m!=-1){
    db2[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swv2[idx]=(1.0+w)/(1.0-w);
    }else{
      swv2[idx]=0.0;
    }
  }
}

void QMarker::setdBh1(int idx,double d){
  if(m!=-1){
    dbh1[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swvh1[idx]=(1.0+w)/(1.0-w);
    }else{
      swvh1[idx]=0.0;
    }
  }
}

void QMarker::setdBh2(int idx,double d){
  if(m!=-1){
    dbh2[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swvh2[idx]=(1.0+w)/(1.0-w);
    }else{
      swvh2[idx]=0.0;
    }
  }
}

void QMarker::setdBh3(int idx,double d){
  if(m!=-1){
    dbh3[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swvh3[idx]=(1.0+w)/(1.0-w);
    }else{
      swvh3[idx]=0.0;
    }
  }
}

void QMarker::setdBh4(int idx,double d){
  if(m!=-1){
    dbh4[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swvh4[idx]=(1.0+w)/(1.0-w);
    }else{
      swvh4[idx]=0.0;
    }
  }
}

void QMarker::setdBh5(int idx,double d){
  if(m!=-1){
    dbh5[idx]=d;
    if(d<0.0){
      double w=pow(10.0,(d/20));
      swvh5[idx]=(1.0+w)/(1.0-w);
    }else{
      swvh5[idx]=0.0;
    }
  }
}

bool QMarker::isPosition(QPoint pos){
  bool ok=false;
  for(int i=0;i<6;i++){
    if(abs(pos.x()-point[i].x()) < 5){
      //qDebug()<<"abs(pos.x()-point[i].x()) < 5"<<abs(pos.x()-point[i].x());
      m=i;
      ok = true;
    }
  }
  return ok;
}

int QMarker::textPosX(){
  return textpoint.x();
}

int QMarker::textPosY(){
  return textpoint.y();
}

QPoint QMarker::MarkertextPos(){
  return textpoint;
}

void QMarker::addMarkerStrList(QString s){
  strlist.append(s);
}

void QMarker::clrMarkerStrList(){
  strlist.clear();
}

void QMarker::frq_korrektur(double fa, double fb){
  //qDebug()<<"fa:"<<fa;
  //qDebug()<<"fb:"<<fb;
  //Marker Schrift immer im Frequenzbereich des Displays verschieben
  for(int i=0; i<6; i++){
    if(bmarkerein[i]){
      if(frq[i]<fa)frq[i]=fa;
      if(frq[i]>fb)frq[i]=fb;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------------


PlotSettings::PlotSettings(){
}

void PlotSettings::setminX(double d){
  minX = d;
}

void PlotSettings::setmaxX(double d){
  maxX = d;
}
void PlotSettings::setmpunkteX(int i){
  mpunkteX = i;
}

void PlotSettings::setschrittweiteX(double d){
  schrittweiteX = d;
}

void PlotSettings::setminY(double d){
  minY = d;
}

void PlotSettings::setmaxY(double d){
  maxY = d;
}

void PlotSettings::setdbline(double d){
  dblinie = d;
}


void PlotSettings::setnumXTicks(int i){
  numXTicks = i;
}

void PlotSettings::setnumYTicks(int i){
  numYTicks = i;
}

void PlotSettings::setxeinheit(const QString &s){
  xeinheit = s;
}

void PlotSettings::setyeinheit(const QString &s){
  yeinheit = s;
}

void PlotSettings::setzeinheit(const QString &s){
  zeinheit = s;
}

void PlotSettings::scroll(int dx, int dy){

  double stepX = spanX() / numXTicks;
  minX += dx * stepX;
  maxX += dx * stepX;

  double stepY = spanY() / numYTicks;
  minY += dy * stepY;
  maxY += dy * stepY;
}

void PlotSettings::setcolorfmarken(QColor c){
  penfmarken = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhintergrund(QColor c){
  colorhintergrund = c;
}

void PlotSettings::setcolorspur(QColor c){
  penspur = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorschrift(QColor c){
  penschrift = QPen( c, 0, Qt::SolidLine);
  pensolidline = QPen( c, 0, Qt::SolidLine);
  pendotline = QPen( c, 0, Qt::DotLine);
  pendashline = QPen( c, 0, Qt::DashLine);
  pendashdotline = QPen( c, 0, Qt::DashDotLine);
  pendashdotdotline = QPen( c, 0, Qt::DashDotDotLine);
}

void PlotSettings::setcolormarkerschrift(QColor c){
  penmarkerschrift = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorlinek(QColor c){
  penlinek = QPen( c, 0, Qt::SolidLine);
  pendotlinek = QPen( c, 0, Qt::DotLine);
  pendashlinek = QPen( c, 0, Qt::DashLine);
  pendashdotlinek = QPen( c, 0, Qt::DashDotLine);
  pendashdotdotlinek = QPen( c, 0, Qt::DashDotDotLine);
}

void PlotSettings::setcolorlinekmarker(QColor c){
  penlinekmarker = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhline1(QColor c){
  penhlinek1 = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhline2(QColor c){
  penhlinek2 = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhline3(QColor c){
  penhlinek3 = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhline4(QColor c){
  penhlinek4 = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setcolorhline5(QColor c){
  penhlinek5 = QPen( c, 0, Qt::SolidLine);
}

void PlotSettings::setmarkerfontsize(int i){
  markerfontsize=i;
}

void PlotSettings::setfontsize(int i){
  fontsize=i;
}

void PlotSettings::setEnabledfontsize(bool b){
  bfontsize=b;
}

void PlotSettings::setbdBm(bool b){
  bdBm=b;
}

bool PlotSettings::isbdBm(){
  return bdBm;
}

void PlotSettings::setbwatt(bool b){
  bwatt=b;
}

bool PlotSettings::isbwatt(){
  return bwatt;
}

void PlotSettings::setbvolt(bool b){
  bvolt=b;
}

bool PlotSettings::isbvolt(){
  return bvolt;
}

void PlotSettings::setPopupPoint(QPoint p){
  popuppoint = p;
}

void PlotSettings::setDisplayshift(double d){
  dshift=d;
}

double PlotSettings::Displayshift(){
  return dshift;
}

void PlotSettings::setKurvenshift(double d){
  kshift=d;
}

double PlotSettings::Kurvenshift(){
  return kshift;
}

void PlotSettings::setSettingPath(const QString &s){
  settingspath=s;
}

bool PlotSettings::is0dBLine(){
  return b0dBLine;
}

void PlotSettings::set0dBLine(bool b){
  b0dBLine=b;
}

void PlotSettings::setInfoversion(const QString & s){
  infoversion=s;
}
