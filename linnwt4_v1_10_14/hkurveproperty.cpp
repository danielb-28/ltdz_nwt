#include "hkurveproperty.h"

QHKurveDlg::QHKurveDlg(QWidget *parent) :  QDialog(parent){
  this->resize(250,220);
  this->setMinimumSize(0,0);
  this->setWindowTitle(tr("Hintergrund-Kurve","Kurven Eigenschaften Dialog"));
  save = new QPushButton(tr("Speichern","Kurven Eigenschaften Dialog"), this);
  save->setGeometry(150,10,80,30);
  connect(save, SIGNAL(clicked()), SLOT(bsave()));
  caption = new QPushButton(tr("Beschr.","Kurven Eigenschaften Dialog"), this);
  caption->setGeometry(150,45,80,30);
  connect(caption, SIGNAL(clicked()), SLOT(setcaption()));
  ok = new QPushButton(tr("OK","Kurven Eigenschaften Dialog"), this);
  ok->setGeometry(150,180,80,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  ok->setFocus();

  grtyp = new QGroupBox(tr("Kurve Typ"),this);
  grtyp->setGeometry(10,10,120,80);
  rbs21 = new QRadioButton("S21",grtyp);
  rbs21->setGeometry(10,20,100,20);
  connect(rbs21, SIGNAL(clicked(bool)), SLOT(setS21()));
  rbs11 = new QRadioButton("S11",grtyp);
  rbs11->setGeometry(10,50,100,20);
  connect(rbs11, SIGNAL(clicked(bool)), SLOT(setS11()));

  grrb = new QGroupBox(tr("Anzeige"),this);
  grrb->setGeometry(10,100,120,110);
  cbswv = new QCheckBox(tr("SWV"),grrb);
  cbswv->setGeometry(10,20,100,20);
  cbswv->setChecked(false);
  connect(cbswv, SIGNAL(clicked(bool)), SLOT(setSWV(bool)));
  cbwatt = new QCheckBox(tr("Watt"),grrb);
  cbwatt->setGeometry(10,50,100,20);
  cbwatt->setChecked(false);
  connect(cbwatt, SIGNAL(clicked(bool)), SLOT(setWatt(bool)));
  cbvolt = new QCheckBox(tr("Volt"),grrb);
  cbvolt->setGeometry(10,80,100,20);
  cbvolt->setChecked(false);
  connect(cbvolt, SIGNAL(clicked(bool)), SLOT(setVolt(bool)));
}

void QHKurveDlg::tip(bool atip){
  QString tip_save = tr(
        "<b>Nachtraegliches Speicher der Kurvendatei</b>"
        ,"tooltip text");
  QString tip_caption = tr(
        "<b>Beschreibung der Kurvendatei Editieren</b>"
        ,"tooltip text");

  bool btip=atip;

  if(btip){
    save->setToolTip(tip_save);
    caption->setToolTip(tip_caption);
  }else{
    save->setToolTip("");
    caption->setToolTip("");
  }
}

void QHKurveDlg::setHMesskurve(THMessKurve mk){
  hmesskurve=mk;
  cbswv->setChecked(hmesskurve.bswv);
  cbwatt->setChecked(hmesskurve.bwatt);
  cbvolt->setChecked(hmesskurve.bvolt);
  //qDebug()<<hmesskurve.bdbm;
  if(hmesskurve.bS11){
    rbs11->setChecked(true);
    rbs21->setChecked(false);
  }else{
    rbs11->setChecked(false);
    rbs21->setChecked(true);
  }
  setAnzeige();
}

THMessKurve QHKurveDlg::getHMesskurve(){
  hmesskurve.bswv=cbswv->isChecked();
  hmesskurve.bwatt=cbwatt->isChecked();
  hmesskurve.bvolt=cbvolt->isChecked();
  return hmesskurve;
}

void QHKurveDlg::setSWV(bool b){
  if(b){
    cbwatt->setChecked(false);
    cbvolt->setChecked(false);
  }
}

void QHKurveDlg::setWatt(bool b){
  if(b){
    cbswv->setChecked(false);
  }
}

void QHKurveDlg::setVolt(bool b){
  if(b){
    cbswv->setChecked(false);
  }
}

void QHKurveDlg::bsave(){
  messkurve.bkanal=hmesskurve.bkanal;
  messkurve.caption=hmesskurve.caption;
  messkurve.mpunkte=hmesskurve.mpunkte;
  if(hmesskurve.bS11){
    messkurve.mktyp=mks11;
  }else{
    messkurve.mktyp=mks21;
  }
  for(int i=0;i<messkurve.mpunkte;i++){
    messkurve.frequenz[i]=hmesskurve.frequenz[i];
    messkurve.dbk1[i]=hmesskurve.db[i];
  }
  emit savemk(messkurve);
  this->accept();
}

void QHKurveDlg::setcaption(){
  bool ok;
  QString qs=hmesskurve.caption;

  QString text = QInputDialog::getText(this, tr("Kurve Beschreibung"),
                                       tr("Beschreibung:"), QLineEdit::Normal,
                                       qs, &ok);
  if (ok){
    text.replace(' ', '_');
    hmesskurve.caption=text;//Caption in Messkurve eintragen
  }
  this->accept();
}
void QHKurveDlg::setS11(){
  hmesskurve.bS11=true;
  setAnzeige();
}

void QHKurveDlg::setS21(){
  hmesskurve.bS11=false;
  setAnzeige();
}

void QHKurveDlg::setAnzeige(){
  if(hmesskurve.bS11){
    cbswv->setEnabled(true);
    cbwatt->setEnabled(false);
    cbvolt->setEnabled(false);
  }else{
    cbswv->setEnabled(false);
    cbswv->setChecked(false);
    cbwatt->setEnabled(hmesskurve.bdbm);
    cbvolt->setEnabled(hmesskurve.bdbm);
  }
}

