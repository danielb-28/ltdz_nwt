#ifndef QSETUPDLG_H
#define QSETUPDLG_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "konstdef.h"

/*==================================================================================================*/

class setupdlg : public QDialog{
    Q_OBJECT

public:
  setupdlg(QWidget *parent = 0);
  ~setupdlg();
  void setgrunddaten(const TGrunddaten &);
  void setwobdaten(const TWobbelGrundDaten &);
  TGrunddaten getgrunddaten();
  TWobbelGrundDaten getwobdaten();
  void setHWVariante(evariante);
  void tip(bool);

public slots:

private:
  QPushButton *ok;
  QPushButton *cancel;
  QTabWidget *tabwg;
  //====================================================
  QWidget *wgblatt1;
  //----------------------------------------------------
  QGroupBox *grddstakt;
  QDoubleSpinBox *sboxddstakt;
  QDoubleSpinBox *sboxvfo;
  QCheckBox *cbtaktx10;
  QLabel *labelddsvfo;
  //----------------------------------------------------
  QGroupBox *grnwtauswahl;
  QRadioButton *rbnwt4000_1;
  QRadioButton *rbnwt4000_2;
  QRadioButton *rbnwt_ltdz;
  QRadioButton *rbnwt6000;
  QRadioButton *rbnwt_nn;
  //----------------------------------------------------
  QGroupBox *grcalfrq;
  QLineEdit *lecalfrqmin;
  QLabel *lacalfrqmin;
  QLineEdit *lecalfrqmax;
  QLabel *lacalfrqmax;
  //----------------------------------------------------
  QGroupBox *grwobbel0hz;
  QCheckBox *cbset0hz;
  //----------------------------------------------------
  QGroupBox *grhwcall;
  QCheckBox *cberledigt;
  //----------------------------------------------------
  QGroupBox *grnozwtime;
  QCheckBox *cbnozwtime;
  //----------------------------------------------------
  QGroupBox *gr0dBLine;
  QCheckBox *cb0dBLine;
  //====================================================
  QWidget *wgblatt2;
  //----------------------------------------------------
  QGroupBox *grcolor;
  QPushButton *buttonhintergrund;
  QLabel *labelhintergrund;
  QPushButton *buttonschrift;
  QLabel *labelschrift;
  QPushButton *buttonmarkerschrift;
  QLabel *labelmarkerschrift;
  QPushButton *buttonliniek1;
  QLabel *labelliniek1;
  QPushButton *buttonlinekmarker;
  QLabel *labellinekmarker;
  QPushButton *buttonfmarken;
  QLabel *labelfmarken;
  QPushButton *buttoncolorspur;
  QLabel *labelcolorspur;
  QPushButton *buttonsetdefault;
  QLabel *labelsetdefault;
  //----------------------------------------------------
  QGroupBox *grfnformat;
  QLineEdit *leditfnformat;
  QLabel *labelfnformat;
  QPushButton *bfnformat;
  //====================================================
  QWidget *wgblatt4;
  //----------------------------------------------------
  QGroupBox *grfontsize;
  QCheckBox *cbfontext;
  QSpinBox *sboxmfsize;
  QLabel *labelmfsize;
  QSpinBox *sboxfsize;
  QLabel *labelfsize;
  QSpinBox *sboxmwfsize;
  QLabel *labelmwfsize;
  QSpinBox *sboxvfofsize;
  QLabel *labelvfofsize;
  QSpinBox *sboxberfsize;
  QLabel *labelberfsize;
  QSpinBox *sboxgrberfsize;
  QLabel *labelgrberfsize;
  //====================================================
  //====================================================
  //====================================================
  evariante hwvariante;
  TGrunddaten grunddaten;
  TWobbelGrundDaten wobbelgrunddaten;
  void setColorButton();
  double pegelatt;
  double pegelvfomax;
  bool buebern;
  bool cbvfosperre;
  double linenormalisieren(const QString &);
  double str2frq(const QString &);
  QString frq2str(double);
  double dca, dce;
  void set_lea_lee();

private slots:
  void  setNWT4000_1_CalFrq();
  void  setNWT4000_2_CalFrq();
  void  setNWT_ltdz_CalFrq();
  void  setNWT6000_CalFrq();
  void  setNWT_nn_CalFrq();
  void  setVFO(double);
  void  setTakt(double);
  void  cbtaktx10_checked(bool);
  void  setbuttonhintergrund();
  void  setbuttoncolorspur();
  void  setbuttonschrift();
  void  setbuttonmarkerschrift();
  void  setbuttonliniek1();
  void  setbuttonlinekmarker();
  void  setbuttonfmarken();
  void  setbuttonsetdefault();
  void  setmFontSize(int);
  void  setFontSize(int);
  void  setmwFontSize(int);
  void  setvfoFontSize(int);
  void  setberFontSize(int);
  void  setgrberFontSize(int);
  void  cbfontextern_checked(bool);
  void  setfndef();
  void  cbset0hz_checked(bool);
  void  cberledigt_checked(bool);
  void  cbnozwtime_checked(bool);
  void  cb0dBLine_checked(bool);

signals:
  void  setVfo(double);
  void  setNWTTakt(double);
};


#endif // QSETUPDLG_H
