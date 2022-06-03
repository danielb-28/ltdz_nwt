//
// C++ Interface: konstdef
//
// Description: 
//
//
// Author: Andreas Lindenau <DL4JAL@darc.de>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
// * Edit dc5pi, 2016-03-24 /dev/ttypACM[0:3]
//
#ifndef KONSTDEF_H
#define KONSTDEF_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>
/*
#include <QPen>
#include <QColor>
#include <QDir>
#include <QPoint>
*/


const int maxmesspunkte = 2000+1; // Messpunkte von 0 - 2001 = 2000 Frequenzabschnitte
const int maxrxbuffer = 200000; // RS232 Buffer maximale Laenge
const int spurmax = 20;

const char schnittstelle1[] ="/dev/ttyS0";
const char schnittstelle2[] ="/dev/ttyS1";
const char schnittstelle3[] ="/dev/ttyS2";
const char schnittstelle4[] ="/dev/ttyS3";
const char schnittstelle5[] ="/dev/ttyUSB0";
const char schnittstelle6[] ="/dev/ttyUSB1";
const char schnittstelle7[] ="/dev/ttyUSB2";
const char schnittstelle8[] ="/dev/ttyUSB3";

enum emktyp {mks21, mks11, mks21var, mks11var};
enum evariante {vnwt4_1, vnwt4_2, vnwt_ltdz, vnwt6, vnwt_nn};
enum eS11cal {vrefopen, vref75, vref25, vref100, vrefudef};

typedef struct{
  double lfrequenz;//Laengenfrequenz
  double vf;       //Verkuerzungsfaktor
  double laenge;   //Laenge des Kabel
  double ca;       //Kabelkapazitaet
  double dk;       //Dielektrizitaetszahl:
  double z;      //Impedanz
}Tkabeldaten;

typedef struct{
  double adc1;//ADC Wert Punkt 1
  double adc2;//ADC Wert Punkt 2
  double adcmw1;//ADC Wert mW-Meter Punkt 1
  double adcmw2;//ADC Wert mW-Meter Punkt 2
  double att;//Attenuator beim Kalibrieren
  bool bmkneu;//Kalibriervorgang JA/NEIN
}TCalDaten;

typedef struct {
  QString spath;//Verzeichnis der Konfiguration
  QDir  homedir;//Verzeichnis aller wichtigen Dateien
  QString formatfilename;//Format des default Filename
  QColor colorhintergrund;//Hintergrundfarbe des Display
  QColor colorschrift;//Farbe der Displaybeschriftung
  QColor colormarkerschrift;//Farbe der Markerschrift
  QColor colorlinie;//Farbe der Kurvenlinie Kanal
  QColor colorliniemarker;//Farbe der Marken an der Kurvenlinie
  QColor colorfmarken;//Farbe der Frequnzmarken und Kursorkreuz
  QColor colorwobbelspur;//Farbe der Wobbelspur
  double nwttakt;//Takt des DDS ICs
  //double vfofrqcal;//Frequenz zum Kalibrieren der Taktfrequenz
  double nwttaktcalfrq;//Frequenz zum Kalibrieren der Taktfrequenz
  int hangmw;//Peak&Hold Zeit mW-Meter Messkanal1
  int precisionmw;//mW-Meter Anzeige die Kommastellen
  bool bset0hz;//nach Wobbeln keine DDS-Frequenzausgabe
  bool bmwmeter;//Anzeige mW-Meter
  int calablauf;//Kalibrierablauf 0..3
  int version;
  evariante variante;//Variante des NWTx
  bool bfontsize; //Schriftgroesse vom Programm EIN/AUS
  int fontsize;// Für das ganze Programm
  int markerfontsize;//Schriftgroesse der Markertexte im Display
  int mwfontsize;//Schriftgroesse Werte im mW-Meter
  int vfofontsize;//Schriftgroesse Werte im VFO
  int berfontsize;//Schriftgroesse in Berechnungen
  int grberfontsize;//Schriftgroesse Ueberschrift in Berechnungen
  QString filemk;// Name der Messkopfdatei Kanal 1
  bool bflatcal;//TRUE wenn Flatnes Kalibrierung durchgefuehrt
  bool bnozwtime;//TRUE wenn keine Zwischenzeit verwendet werden darf
}TGrunddaten;

typedef struct {
  bool aktiv[16];
  double frq1[16];
  double frq2[16];
  QString caption[16];
}TFrqmarken;

typedef struct{
  QString caption;//Beschreibung der Kurve
  QPoint point;//Point für senkrechten Strich an Mousezeiger
  int mpunkte;//Anzahl der Frequenzschritte
  double frequenz[maxmesspunkte];//Frequenz des Messpunktes
  double dbk1[maxmesspunkte];//db Wert Kanal1
  double dbsp[spurmax][maxmesspunkte];//Array db Werte Spur
  bool bdbsp[spurmax];//Spur gueltig
  bool bspur;// Spur EIN/AUS
  int dbspidx; //Index der Spur
  bool bdelta;//Deltaanzeige Kanal1 und Kanal2 in dB EIN/AUS
  bool bswv;//swv Kanal1 Anzeige EIN/AUS
  bool bkanal;//Kanal1 EIN/AUS
  bool bdbmax;//dbmaxanzeige EIN/AUS
  bool bdbmin;//dbminanzeige EIN/AUS
  bool b3db;//3dB Bandbreite EIN/AUS
  bool bguete;//3dB Bandbreite EIN/AUS
  double frq3db1;//3dB Bandbreite untere Frequenz
  double frq3db2;//3dB Bandbreite obere Frequenz
  bool b6db;//6dB Bandbreite EIN/AUS
  double frq6db1;//6dB Bandbreite untere Frequenz
  double frq6db2;//3dB Bandbreite obere Frequenz
  bool b60db;//60dB Bandbreite EIN/AUS
  bool bshape;//60dB Bandbreite EIN/AUS
  double frq60db1;//60dB Bandbreite untere Frequenz
  double frq60db2;//3dB Bandbreite obere Frequenz
  bool b3dbinv;//3dB Bandbreite invers EIN/AUS
  bool bkurvegueltig;//Messkurve ist gueltig
  double adaempfung;//Kabeldaempfung fuer swv Berechnung ohne Speiseleitung
  int calablauf;//Kalibrierablauf 0..3, 4 Flatnes Calibration
  int calindex;//Nummer des Datenpaketes fuer Anzeige
  QString calstring;//fuer Flatnes Calibration
  emktyp mktyp;//Messkurve gemessen mit MK-Typ ...
}TMessKurve;

typedef struct{
  QString caption;//Beschreibung der Kurve
  int mpunkte;//Anzahl der Frequenzschritte
  double frequenz[maxmesspunkte];//Frequenz des Messpunktes
  double db[maxmesspunkte];//db Wert Kanal1
  bool bkanal;//Kanal1 EIN/AUS
  bool bmarker;//Marker EIN/AUS
  bool bS11;
  bool bdbm;
  bool bswv;
  bool bwatt;
  bool bvolt;
}THMessKurve;

typedef struct{
  int mpunkte; //Anzahl der Messpunkte
  int ztime;//Zeit zwischen den Messpunkten
  double frequenzschritt;//Schrittweite in MHz
  double frequenzanfang;//Wobbelanfang in MHz
  double dbmin;//dB auf Y-Achse im Display
  double dbmax;//dB auf Y-Achse im Display
  double dbline;//dB Zusatzlinie im Display
  double dshift;//Displayverschiebung in der dB Beschriftung 10er Schritte
  double kshift;//Displayverschiebung in der dB Beschriftung 0..9 dB
  double calfrqmin;//Minimale KalibrierFrequenz in MHz
  double calfrqmax;//Maximale KalibrierFrequenz in MHz
  bool bdBm; //Spektrumanzeige in dBm
  bool bwatt; //Spektrumanzeige in Watt
  bool bvolt; //Spektrumanzeige in Volt
  bool b0dBline;//OdB Linie wird hervor gehoben
}TWobbelGrundDaten;

typedef struct{
  QString sstart;
  QString sstop;
  QString ssteps;
  QString sstep;
  QString scenter;
  QString sspan;
  bool banfende;
  bool bspanmitte;
}TWobbelFrqStr;

typedef struct{
  QPoint pos;
  double frq;
  double dbm;
}TPeak;

typedef struct{
  bool bplist;
  QStringList plist;
  TPeak peak[20];
}TPeaklist;

#endif
