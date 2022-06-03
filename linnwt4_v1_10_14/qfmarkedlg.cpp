#include "qfmarkedlg.h"

QFmarkeDlg::QFmarkeDlg(QWidget *parent) :  QDialog(parent){
  int a = 32;

  this->resize(500,10+a*18);
  this->setMinimumSize(0,0);
  this->setWindowTitle(tr("Frequenzmarken setzen","Frequenzmarkierung Dialog"));
  ok = new QPushButton(tr("OK","Frequenzmarkierung Dialog"), this);
  ok->setGeometry(80,a*17,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  uebernehmen = new QPushButton(tr("Uebernahme","Frequenzmarkierung Dialog"), this);
  uebernehmen->setGeometry(190,a*17,100,30);
  connect(uebernehmen, SIGNAL(clicked()), SLOT(setuebernahme()));
  cancel = new QPushButton(tr("Abbruch","Frequenzmarkierung Dialog"), this);
  cancel->setGeometry(300,a*17,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));

  for(int i=0; i<16; i++){
    check[i] = new QCheckBox(this);
    check[i]->setGeometry(10,10+i*a,20,30);
    ecaption[i] = new QLineEdit(this);
    ecaption[i]->setGeometry(30,10+i*a,210,30);
    efrq1[i] = new QLineEdit(this);
    QObject::connect(efrq1[i], SIGNAL(returnPressed ()), this, SLOT(normalisieren()));
    efrq1[i]->setGeometry(250,10+i*a,100,30);
    efrq2[i] = new QLineEdit(this);
    QObject::connect(efrq2[i], SIGNAL(returnPressed ()), this, SLOT(normalisieren()));
    efrq2[i]->setGeometry(360,10+i*a,100,30);
  }
}

void QFmarkeDlg::setFrqmarken(TFrqmarken &f){
  frqm=f;
  for(int i=0;i<16;i++){
    efrq1[i]->setText(frq2str(frqm.frq1[i]));
    efrq2[i]->setText(frq2str(frqm.frq2[i]));
    ecaption[i]->setText(frqm.caption[i]);
    check[i]->setChecked(frqm.aktiv[i]);
  }
}

TFrqmarken QFmarkeDlg::getFrqmarken(){
  QString qs;

  normalisieren();
  for(int i=0;i<16;i++){
    qs=efrq1[i]->text();
    frqm.frq1[i]=str2frq(qs);
    qs=efrq2[i]->text();
    frqm.frq2[i]=str2frq(qs);
    frqm.caption[i]=ecaption[i]->text();
    frqm.aktiv[i]=check[i]->isChecked();
  }
  return frqm;
}

QString QFmarkeDlg::frq2str(double d){
  QString me=" GHz";
  QString qs;
  double w=d;
  int l;

  w=w/1000.0;
  qs.sprintf("%1.9f",w);
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
  if(w==0.0)me="";
  l=qs.length();
  while(qs.at(l-1)=='0'){
    qs=qs.left(l-1);
    l=qs.length();
  }
  if(qs.at(l-1)=='.')qs=qs.left(l-1);
  qs=qs+me;
  return(qs);
}

double QFmarkeDlg::str2frq(const QString &s){
  QString qs=s;
  bool ok;
  double d,r;

  //qDebug("str2frq");
  //qDebug()<<qs;
  qs.remove(" ");
  //qDebug()<<qs;
  if(qs.contains("GHz")){
    qs.remove("GHz");
    d=1000000000.0;
  }
  if(qs.contains("MHz")){
    qs.remove("MHz");
    d=1000000.0;
  }
  if(qs.contains("kHz")){
    qs.remove("kHz");
    d=1000.0;
  }
  if(qs.contains("Hz")){
    qs.remove("Hz");
    d=1.0;
  }
  r=qs.toDouble(&ok);
  //qDebug()<<qs;
  if(ok){
    d=d*r;
    //qDebug()<<d;
    //qDebug("str2frq ENDE");
    return d/1000000.0;
  }else{
    return (0.0);
  }
}

double QFmarkeDlg::linenormalisieren(const QString &line)
{
  bool ok;
  double faktor=1.0;
  double ergebnis = 0.0;
  double d=0.0, r=0.0;
  bool bremove=false;

  QString aline(line);

  //qDebug("linenormalisieren");
  qDebug()<<aline;
  if(aline.contains("GHz")){
    aline.remove("GHz");
    d=1000.0;
    bremove=true;
  }
  if(aline.contains("MHz")){
    aline.remove("MHz");
    d=1.0;
    bremove=true;
  }
  if(aline.contains("kHz")){
    aline.remove("kHz");
    d=0.001;
    bremove=true;
  }
  if(aline.contains("Hz")){
    aline.remove("Hz");
    d=0.000001;
    bremove=true;
  }
  //qDebug()<<aline;
  if(bremove){
    r=aline.toDouble(&ok);
    if(ok){
      ergebnis=d*r;
    }else{
      ergebnis=0.0;
    }
  }else{
    faktor=0.000001;
    aline = aline.toLower();	//alles auf Kleinschreibung
    aline.remove(QChar(' ')); // Leerzeichen entfernen
    if(aline.isEmpty()){
      ergebnis= 0.0;
    }else{
      if(aline.contains('g')){
        if(aline.contains(',') or aline.contains('.')){
          aline.replace('g',' ');
        }else{
          aline.replace('g','.');
        }
        faktor=1000.0;
      }
      if(aline.contains('m')){
        if(aline.contains(',') or aline.contains('.')){
          aline.replace('m',' ');
        }else{
          aline.replace('m','.');
        }
        faktor=1.0;
      }
      if(aline.contains('k')){
        if(aline.contains(',') or aline.contains('.')){
          aline.replace('k',' ');
        }else{
          aline.replace('k','.');
        }
        faktor = 0.001;
      }
      ergebnis = aline.toDouble(&ok);
      if(!ok)ergebnis = 0.0;
      ergebnis *= faktor;
      if(ergebnis < 0.0)ergebnis = 0.0; // negative Zahl verhindern
    }
  }
  //qDebug()<<ergebnis;
  //qDebug("linenormalisieren ENDE");
  return ergebnis;
}

void QFmarkeDlg::normalisieren(){
  double d;
  QString qs;
  for(int i=0;i<16;i++){
    qs=efrq1[i]->text();
    d=linenormalisieren(qs);
    qs=frq2str(d);
    efrq1[i]->setText(qs);
    qs=efrq2[i]->text();
    d=linenormalisieren(qs);
    qs=frq2str(d);
    efrq2[i]->setText(qs);
  }
}

void QFmarkeDlg::setuebernahme(){
  normalisieren();
}
