#include "qkurvenanalyse.h"

QKurvenAnalyseDlg::QKurvenAnalyseDlg(QWidget *parent) :  QDialog(parent){
  this->resize(300,100);
  this->setMinimumSize(0,0);
  this->setWindowTitle(tr("Analyse der Wobbelkurven-Spur","Kurvenspur Dialog"));
  buttonok = new QPushButton("SET",this);
  buttonok->setGeometry(200,50,80,30);
  QObject::connect(buttonok, SIGNAL(clicked()),this, SLOT(accept()));
  buttonabbruch = new QPushButton("Abbruch",this);
  buttonabbruch->setGeometry(200,20,80,30);
  QObject::connect(buttonabbruch, SIGNAL(clicked()),this, SLOT(close()));
  sbkurven = new QSpinBox(this);
  QObject::connect(sbkurven, SIGNAL(valueChanged(int)),this, SLOT(setSpur()));
  sbkurven->setGeometry(10,35,50,30);
  sbkurven->setRange(0,spurmax-1);
  labelkurven = new QLabel(tr("Spur Nummer","Kurvenspur Dialog"),this);
  labelkurven->setGeometry(70,35,150,30);
  spur = 0;
}

int QKurvenAnalyseDlg::getSpurNr(){
  return spur;
}

void QKurvenAnalyseDlg::setSpur(){
  spur = sbkurven->value();
  qDebug()<<"Spur:"<<spur;
  emit setSpurNr(spur);
}

void QKurvenAnalyseDlg::setSpurMax(int s){
  spur = s;
  sbkurven->setMaximum(spur);
  sbkurven->setValue(spur);
}
