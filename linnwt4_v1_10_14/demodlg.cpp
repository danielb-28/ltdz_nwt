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

#include "demodlg.h"

class QDialog;

//#####################################################################################
// Class fuer Option
//#####################################################################################
demodlg::demodlg(QWidget *parent) :  QDialog(parent)
{
  int h=200;
  int w=400;

  this->resize(w,h);
  this->setMinimumSize(w,h);
  this->setMaximumSize(w,h);
  this->setWindowTitle(tr("Erzeugen von Demo-Dateien","Demo Dialog Titel"));
  ok = new QPushButton(tr("OK","Demofile Dialog Button"), this);
  ok->setGeometry(w-110,20,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Abbruch","Demofile Dialog Button"), this);
  cancel->setGeometry(w-110,60,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));

  grdemofile = new QGroupBox(tr("Configurationspunkte","Demofile Dialog GroupBox"), this);
  cbmaxy = new QCheckBox(tr("SET Y(dbmax)","demodlg CheckBox"),grdemofile);
  cbminy = new QCheckBox(tr("SET Y(dbmin)","demodlg CheckBox"),grdemofile);
  cbdshift = new QCheckBox(tr("SET Display-Shift","demodlg CheckBox"),grdemofile);
  cbswrk1 = new QCheckBox(tr("SWV-Anzeige Kanal 1","demodlg CheckBox"),grdemofile);
  cbswrk2 = new QCheckBox(tr("SWV-Anzeige Kanal 2","demodlg CheckBox"),grdemofile);

  int wgr=w-30-110;
  int hgr=h-20;

  grdemofile->setGeometry(10,10,wgr,hgr);

  int hcb=20;

  cbmaxy->setGeometry(10,hcb,wgr-20,30); hcb+=30;
  cbminy->setGeometry(10,hcb,wgr-20,30); hcb+=30;
  cbdshift->setGeometry(10,hcb,wgr-20,30); hcb+=30;
  cbswrk1->setGeometry(10,hcb,wgr-20,30); hcb+=30;
  cbswrk2->setGeometry(10,hcb,wgr-20,30);
  demofile.bmaxy=false;
  demofile.bminy=false;
  demofile.bdshift=false;
  demofile.bswrk1=false;
  demofile.bswrk2=false;
  demofile.maxy=10.0;
  demofile.miny=-90.0;
};

demodlg::~demodlg()
{
}


void demodlg::setdaten(const TDemoFile &demof){
  QString s, s1;
  demofile = demof;
  cbmaxy->setChecked(demofile.bmaxy);
  s = cbmaxy->text();
  s1.sprintf(" (%2.0fdB)",demofile.maxy);
  s = s+s1;
  cbmaxy->setText(s);
  cbminy->setChecked(demofile.bminy);
  s = cbminy->text();
  s1.sprintf(" (%2.0fdB)",demofile.miny);
  s = s+s1;
  cbminy->setText(s);
  cbdshift->setChecked(demofile.bminy);
  s = cbdshift->text();
  s1.sprintf(" (%2.0fdB)",demofile.dshift);
  s = s+s1;
  cbdshift->setText(s);
  cbdshift->setChecked(demofile.bdshift);
  cbswrk1->setChecked(demofile.bswrk1);
  cbswrk2->setChecked(demofile.bswrk2);
};

TDemoFile demodlg::getdaten(){
  demofile.bmaxy=cbmaxy->isChecked();
  demofile.bminy=cbminy->isChecked();
  demofile.bdshift=cbdshift->isChecked();
  demofile.bswrk1=cbswrk1->isChecked();
  demofile.bswrk2=cbswrk2->isChecked();
  return demofile;
};


void demodlg::tip(bool atip){
  bool btip=atip;
  if(btip){
  }else{
  }
}

