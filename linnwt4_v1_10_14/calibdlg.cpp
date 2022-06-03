//
// C++ Implementation: sondedlg
//
// Description:
//
//
// Author: Andreas Lindenau <DL4JAL@darc.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
// C++ Implementation: optiondlg
//
// Description:
//
//
// Author: Andreas Lindenau <DL4JAL@darc.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
/*
#include <QWidget>
#include <QDialog>
*/

#include "calibdlg.h"

class QDialog;

//#####################################################################################
// Class fuer Option
//#####################################################################################
calibdlg::calibdlg(QWidget *parent) :  QDialog(parent)
{
  int h=230;
  int w=650;

  this->resize(w,h);
  this->setMinimumSize(w,h);
  this->setMaximumSize(w,h);
  this->setWindowTitle(tr("Kalibrieren des Messkopfes","Calibrieren Dialog Titel"));
  weiter = new QPushButton(tr("Weiter","Calibrieren Dialog"), this);
  weiter->setGeometry(w-110,20,100,30);
  connect(weiter, SIGNAL(clicked()), SLOT(fweiter()));
  cancel = new QPushButton(tr("Abbruch","Calibrieren Dialog"), this);
  cancel->setGeometry(w-110,60,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));

  grmkauswahl = new QGroupBox(tr("Messkopf-Auswahl","GroupBox Messkopf Auswahl"), this);
  cbmesskopf = new QComboBox(grmkauswahl);
  QObject::connect( cbmesskopf, SIGNAL( currentIndexChanged(int)), this, SLOT(setmesskopf(int)));
  lmesskopf = new QLabel(grmkauswahl);

  int wgr=w-30-100;
  int hgr=h-20;

  grmkauswahl->setGeometry(10,10,wgr,hgr);

  int wcb=wgr-20;

  cbmesskopf->setGeometry(10,30,wcb,30);
  lmesskopf->setGeometry(10,70,wcb,hgr-70);
};

calibdlg::~calibdlg()
{
}


void calibdlg::setdaten(const QMesskopf &messk){
  QString s;
  messkopf = messk;
};

QMesskopf calibdlg::getdaten(){
  int index=cbmesskopf->currentIndex();
  switch (index){
  case 1:messkopf.setMkTyp(mks21);break;
  case 2:messkopf.setMkTyp(mks21var);break;
  case 3:messkopf.setMkTyp(mks11);break;
  case 4:messkopf.setMkTyp(mks11var);break;
  default:break;
  }
  return messkopf;
}

void calibdlg::init(){
  switch(hwvariante){
  case vnwt4_1:
    grmkauswahl->setTitle(tr("HW NWT4000-1, Messkopf-Auswahl","GroupBox MK Auswahl Titel"));
    break;
  case vnwt4_2:
    grmkauswahl->setTitle(tr("HW NWT4000-2, Messkopf-Auswahl","GroupBox MK Auswahl Titel"));
    break;
  case vnwt_ltdz:
    grmkauswahl->setTitle(tr("HW LTDZ 35-4400M, Messkopf-Auswahl","GroupBox MK Auswahl Titel"));
    break;
  case vnwt6:
    grmkauswahl->setTitle(tr("HW NWT6000, Messkopf-Auswahl","GroupBox MK Auswahl Titel"));
    break;
  case vnwt_nn:
    grmkauswahl->setTitle(tr("HW NWT noname, Messkopf-Auswahl","GroupBox MK Auswahl Titel"));
    break;
  }
  cbmesskopf->addItem(tr("kein Messkopf ausgewaehlt","ComboBox Item"));//0
  cbmesskopf->addItem(tr("S21-Messung, Spektrumanalyse, mW-Messkopf, Kalibfrq. fest vom SETUP","ComboBox Item"));//1
  cbmesskopf->addItem(tr("S21-Messung, Spektrumanalyse, mW-Messkopf, Kalibfrq. individuell","ComboBox Item"));//2
  cbmesskopf->addItem(tr("S11-Messung, SWV-Richtkoppler extern,  Kalibfrequenzbereich fest vom SETUP","ComboBox Item"));//3
  cbmesskopf->addItem(tr("S11-Messung, SWV-Richtkoppler extern, Kalibrierfrequenz individuell","ComboBox Item"));//4
}

void calibdlg::setHWVariante(evariante hw){
  hwvariante=hw;
}

void calibdlg::setTitel(QString &titel){
  this->setWindowTitle(titel);
}

void calibdlg::fweiter(){
  if(cbmesskopf->currentIndex()==0){
    //qDebug()<<"reject()";
    reject();
  }else{
    accept();
  }
}

void calibdlg::tip(bool atip){
  bool btip=atip;
  if(btip){
  }else{
  }
}

void calibdlg::setmesskopf(int m){
  QString log_int1 = tr(
        "<b>Logarithmischer Messkopf intern im NWT4000.</b> <br>"
        "Eingebaut ist der IC von Analog Devices AD8307<br>"
        "Er dient zum Wobbeln von S21-Kurven.<br>"
        "Zum Beispiel Durchlasskurven von Bandfiltern usw.<br>"
        "Spektrumanalyse ist auch m&ouml;glich. BW etwa 300kHz.<br>"
        "Der Frequenzbereich zum Kalibrieren wird aus dem SETUP genommen.",
        "Beschreibung Combobox MK Auswahl");
  QString log_int1_fvar = tr(
        "<b>Logarithmischer Messkopf intern im NWT4000.</b> <br>"
        "Eingebaut ist der IC von Analog Devices AD8307<br>"
        "Er dient zum Wobbeln von S21-Kurven.<br>"
        "Zum Beispiel Durchlasskurven von Bandfiltern usw.<br>"
        "Spektrumanalyse ist auch m&ouml;glich. BW etwa 300kHz.<br>"
        "Der Frequenzbereich zum Kalibrieren ist individuell.",
        "Beschreibung Combobox MK Auswahl");
  QString log_int1_swv = tr(
        "<b>Messkopf intern im NWT4000. SWV-Richtkoppler extern.</b><br>"
        "Wird am Messeingang des NWT eine externe SWV-Messbr&uuml;cke<br>"
        "angeschlossen, wird diese Kalibrierung verwendet. Der Messskopf<br>"
        "wird in diesem Fall nur f&uuml;r S11-Messungen, SWV, verwendet.<br>"
        "Der Frequenzbereich zum Kalibrieren wird aus dem SETUP genommen.",
        "Beschreibung Combobox MK Auswahl");
  QString log_int1_swv_fvar = tr(
        "<b>Messkopf intern im NWT4000. SWV-Richtkoppler extern.</b><br>"
        "Wird am Messeingang des NWT eine externe SWV-Messbr&uuml;cke<br>"
        "angeschlossen, wird diese Kalibrierung verwendet. Der Messskopf<br>"
        "wird in diesem Fall nur f&uuml;r S11-Messungen, SWV, verwendet.<br>"
        "Der Frequenzbereich zum Kalibrieren ist individuell.",
        "Beschreibung Combobox MK Auswahl");

  switch(m){
  case 1:lmesskopf->setText(log_int1);break;
  case 2:lmesskopf->setText(log_int1_fvar);break;
  case 3:lmesskopf->setText(log_int1_swv);break;
  case 4:lmesskopf->setText(log_int1_swv_fvar);break;
  default:lmesskopf->setText("");break;
  }
}
