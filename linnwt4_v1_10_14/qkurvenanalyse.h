#ifndef QKurvenAnalyse_H
#define QKurvenAnalyse_H
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "konstdef.h"

class QKurvenAnalyseDlg : public QDialog{
  Q_OBJECT
public:
  QKurvenAnalyseDlg(QWidget *parent = 0);
  QPushButton *buttonok;
  QPushButton *buttonabbruch;
  QSpinBox *sbkurven;
  QLabel *labelkurven;
  QLabel *labelbeschreibung;
  void setSpurMax(int);

public slots:
  int getSpurNr();
  void setSpur();

private:
  int spur;

private slots:

signals:
  void setSpurNr(int);

};
#endif // QKurvenAnalyse_H
