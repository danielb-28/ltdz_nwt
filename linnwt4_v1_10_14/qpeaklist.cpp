#include "qpeaklist.h"

QPeakListDlg::QPeakListDlg(QWidget *parent) :  QDialog(parent){
  this->resize(250,130);
  this->setMinimumSize(0,0);
  this->setWindowTitle(tr("dBm Linie Display","Kurven Eigenschaften Dialog"));
  beschr = new QLabel(this);
  beschr->setGeometry(10,10,230,70);
  QString b(tr("<center><b><u>Peak-Liste erstellen</u></b><br>"
               "Bitte die untere dBm-Grenze festlegen.<br>"
               "--------------------------------------------</center>"
               "Pegel in (dBm)","Inputdialog in der Grafik"));
  beschr->setText(b);
  spdBmGrenze = new QDoubleSpinBox(this);
  spdBmGrenze->setGeometry(10,90,80,20);
  spdBmGrenze->setMinimum(-100.0);
  spdBmGrenze->setMaximum(100.0);
  QObject::connect(spdBmGrenze, SIGNAL(valueChanged(double)), SLOT(changedBmGrenze(double)));
  ok = new QPushButton(tr("OK","Kurven Eigenschaften Dialog"), this);
  ok->setGeometry(100,90,80,30);
  QObject::connect(ok, SIGNAL(clicked()), SLOT(accept()));
  spdBmGrenze->setFocus();
}

void QPeakListDlg::setdBmGrenze(double d){
  spdBmGrenze->setValue(d);
  dBmGrenze=d;
}

void QPeakListDlg::changedBmGrenze(double d){
  dBmGrenze=d;
  emit edBmGrenze(d);
}

double QPeakListDlg::getdBmGrenze(){
  return dBmGrenze;
}
