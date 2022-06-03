#ifndef QPEAKLIST_H
#define QPEAKLIST_H
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "konstdef.h"

class QPeakListDlg : public QDialog{
  Q_OBJECT
public:
  QPeakListDlg(QWidget *parent = 0);
  QLabel *beschr;

public slots:
  void changedBmGrenze(double);
  void setdBmGrenze(double);
  double getdBmGrenze();

private:
  QDoubleSpinBox *spdBmGrenze;
  QPushButton *ok;
  double dBmGrenze;

private slots:

signals:
  void edBmGrenze(double);
};
#endif // QPEAKLIST_H
