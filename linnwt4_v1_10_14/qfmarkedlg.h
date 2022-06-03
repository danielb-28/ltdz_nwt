#ifndef QFMARKEDLG_H
#define QFMARKEDLG_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "konstdef.h"

class QFmarkeDlg : public QDialog{
  Q_OBJECT
public:
  QFmarkeDlg(QWidget *parent = 0);

signals:

public slots:
  void setFrqmarken(TFrqmarken &);
  TFrqmarken getFrqmarken();

private:
  QPushButton *ok;
  QPushButton *cancel;
  QPushButton *uebernehmen;

  TFrqmarken frqm;

  QLineEdit *efrq1[16];
  QLineEdit *efrq2[16];
  QLineEdit *ecaption[16];
  QCheckBox *check[16];

  QString frq2str(double d);
  double str2frq(const QString &);
  double linenormalisieren(const QString &line);

private slots:
  void normalisieren();
  void setuebernahme();

};

#endif // QFMARKEDLG_H
