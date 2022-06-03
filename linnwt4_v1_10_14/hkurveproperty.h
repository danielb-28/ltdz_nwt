#ifndef HKURVEPROPERTY_H
#define HKURVEPROPERTY_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "konstdef.h"

class QHKurveDlg : public QDialog{
  Q_OBJECT
public:
  QHKurveDlg(QWidget *parent = 0);
  void tip(bool);

public slots:
  void setHMesskurve(THMessKurve);
  THMessKurve getHMesskurve();

private:
  QPushButton *ok;
  QPushButton *save;
  QPushButton *caption;

  QGroupBox *grtyp;
  QRadioButton *rbs21;
  QRadioButton *rbs11;

  QGroupBox *grrb;
  QCheckBox *cbswv;
  QCheckBox *cbwatt;
  QCheckBox *cbvolt;

  THMessKurve hmesskurve;
  TMessKurve messkurve;
  void setAnzeige();

private slots:
  void setSWV(bool);
  void setWatt(bool);
  void setVolt(bool);
  void bsave();
  void setcaption();
  void setS11();
  void setS21();

signals:
  void savemk(TMessKurve);
};

#endif // HKURVEPROPERTY_H
