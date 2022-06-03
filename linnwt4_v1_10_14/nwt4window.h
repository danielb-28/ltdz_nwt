#ifndef nwt4window_H
#define nwt4window_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

#include "wdisplay.h"
#include "configfile.h"
#include "konstdef.h"
#include "qsetupdlg.h"
//#include "demodlg.h"


QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN
#include "win_modem.h"
#else
#include "lin_modem.h"
#endif


class QMesskopf
{
public:
  QMesskopf();

  void init();//Anfangseinstellungen setzen mit Kanalzuordnung
  void setCalPegel(TMessKurve);//Uebertragung der Messkurve in die Pegelkalibrierung
  void setCalPegelvonbis(TMessKurve,double,double);//Uebertragung der Messkurve in die Pegelkalibrierung nur einen Frequenzbereich

  double getCalPegel(double frq);//Holen der Pegelabweichung per Frequenz
  double adc2Pegel(int adc,double frq);//Input ADC-Wert, Output db mit Pegelabweichung per Frequenz
  double adc2mWMeter(int adc,double frq);//Input ADC-Wert, Output db mit Pegelabweichung per Frequenz
  void setMkTyp(emktyp);
  void setMkXY(double x,double y);
  void setMkXYmWMeter(double x,double y);
  double calpegelwert(int);
  void clrcalpegel();
  emktyp MkTyp();
  double CalX();
  double CalY();
  double CalXmWMeter();
  double CalYmWMeter();
  void setcalpegelwert(int,double);
  evariante HWVariante();
  void setHWVariante(evariante);
  void setNoFrqCal(bool);//Frequenzgangkorr. AUS/EIN
  QString defDateiName();
  void setCalFrqmin(double);
  void setCalFrqmax(double);
  double CalFrqmin();
  double CalFrqmax();
  void setCalFrqPunkt(double);
  double CalFrqPunkt();
  void setCalKorrdBmPegel(double);
  double CalKorrdBmPegel();
  void setCalFrqSchritt(double);
  double CalFrqSchritt();
  void setCalAtt(double);
  double CalAtt();
  void setS11cal(eS11cal);
  eS11cal getS11cal();

private:
  evariante hwvariante;//NWT4000-1, NWT4000-2
  emktyp typ;//mklog, mkswv, mklogvar, mkswvvar
  eS11cal S11cal;//Kalibriervariante Offen, 100/25 Ohm, 75 Ohm, unbekannt
  bool bnofrqcal;
  double calx; //Kalibrierung X Wert
  double caly; //Kalibrierung Y Wert
  double calxmw; //Kalibrierung X Wert mW Meter
  double calymw; //Kalibrierung Y Wert mW Meter
  double calpegel[maxmesspunkte]; //Array von Kalibrierwerten 0Hz bis 200MHz
  double calfrqmin;
  double calfrqmax;
  double calfrqpunkt; //Frequenz Kalibrierung Wobbeln mW-Meter
  double calkorrdbm;
  double calfrqschritt;
  double calatt;
};

/*==================================================================================================*/

class Nwt4Window : public QMainWindow
{
  Q_OBJECT

public:
  Nwt4Window(QWidget *parent = 0);
  ~Nwt4Window();
  void setProgramPath(const QString&);// wird von main.cpp benutzt
  void ProgramInit();// wird von main.cpp benutzt
  void cfgladen();// wird von main.cpp benutzt
  void setConfigPath(const QString&);// Pfadname für die Konfiguration

public slots:
  void frqanfangdisplay2edit(double);
  void frqendedisplay2edit(double);
  void frqmittedisplay2edit(double);
  void frq2berechnung(double);
  void setNWTTakt(double);
  void setVFONWT(double);
  void kdaten(Tkabeldaten);

private:
  QString infoversion;
  QString infodatum;
  QString infotext;
  QString settingspath;//Pfadname fuer Konfiguration BS

  QMenu *menuDatei;
  QMenu *menuEinstellung;
  QMenu *menuKurven;
  QMenu *menuMesskopf;
  QMenu *menuHilfe;

  //Datei
  QAction *mBeenden;
  QAction *mSpeichernpng;
  //Einstellung
  QAction *mSetup;
  //Kurven
  QAction *mKurvenLaden;
  QAction *mKurvenSichern;
  QAction *mKurveCaption;
  QAction *mFrqMarken;
  QAction *mKabeldaten;
  QAction *mDemoFile;
  QAction *mKurvenSpur;
  QAction *mKurvenAnalyse;
  //Wobbeln Messkopf
  QAction *mWobbeln;
  QAction *mNeuK1;
  QAction *mCalibK1;
  QAction *mSondeLadenK1;
  QAction *mSondeSpeichernK1;
  QAction *mSondeClr;
  QAction *mNoFrqCalK1;
  QAction *mCalibFlat;
  //Hilfe
  QAction *mTooltip;
  QAction *mInfo;
  QAction *mFwVersion;
  QAction *mMkInfoK1;
  QAction *mCaldBm;
  QAction *mCalmWm;
  //RS232 suche Abbruch
  QAction *mRS232Stop;
  //Datei
  QToolBar *fileToolBar;
  //Einstellungen
  QToolBar *einstellungToolBar;
  //Sonden
  QToolBar *sondenLadenToolBar;
  QToolBar *sondenKalibToolBar;
  //Kurven
  QToolBar *kurvenToolBar;



  QTabWidget *tabwidget;//TabWidget vom Programm
  //=====================================================================
  QWidget *nwt4widget;  //Wobbelfenster
  QScrollArea *scrollArea;
  WDisplay *display;    //Wobbeldisplay
  QPushButton *buttonwobbeln;
  QPushButton *buttoneinzeln;
  QPushButton *buttonstop;
  QPushButton *buttonloadmessk;
  QPushButton *buttoninfomessk;

  //QToolButton *toolbutton;
  QProgressBar *rlauf;

  QTabWidget *tabwidgetwobbeln;//TabWidget vom Wobbeln
  //--------------------------------------------------------------
  QWidget *widgetwobbelfrq;  //Fenster Frequenzeinstellung
  QGroupBox *grwobbel;
  QLineEdit *lineEdit_frq_anfang;
  QLineEdit *lineEdit_frq_ende;
  QLineEdit *lineEdit_frq_mpunkte;
  QLineEdit *lineEdit_schrittweite;
  QLineEdit *lineEdit_frq_mitte;
  QLineEdit *lineEdit_frq_span;
  QSpinBox *boxztime;
  QLabel *label_frq_anfang;
  QLabel *label_frq_ende;
  QLabel *label_mpunkte;
  QLabel *label_frq_schrittweite;
  QLabel *label_frq_mitte;
  QLabel *label_frq_span;
  QLabel *label_ztime;

  QGroupBox *grygrenzen;
  QLabel *labelboxydbmax;
  QLabel *labelboxydbmin;
  QSpinBox *boxydbmax;
  QSpinBox *boxydbmin;

  QGroupBox *grkanal;
  QCheckBox *checkboxkanal;
  QLabel *labelkanal;
  QCheckBox *checkboxspeki;

  //--------------------------------------------------------------
  QWidget *widgetdb;
  QGroupBox *grbandbreite;
  QCheckBox *checkboxdbmin;
  QCheckBox *checkboxdbmax;

  QCheckBox *checkbox3db;
  QCheckBox *checkbox3dbinv;
  QCheckBox *checkboxguete;
  QCheckBox *checkbox6db;
  QCheckBox *checkbox60db;
  QCheckBox *checkboxshape;
  QCheckBox *checkboxswvkanal;

  QCheckBox *checkboxwatt;
  QCheckBox *checkboxvolt;

  QGroupBox *grzusatzlinien;
  QLabel *labelboxdblinie;
  QDoubleSpinBox *boxdblinie;
  QCheckBox *checkboxdblinie;
  QDoubleSpinBox *boxswvlinie1;
  QLabel *labelboxswvlinie1;
  QCheckBox *checkboxswvlinie1;
  QDoubleSpinBox *boxswvlinie2;
  QLabel *labelboxswvlinie2;
  QCheckBox *checkboxswvlinie2;

  //--------------------------------------------------------------
  QWidget *widgetdb2;

  QGroupBox *grswvant;
  QDoubleSpinBox *boxalaenge;
  QLabel *labelalaenge;
  QDoubleSpinBox *boxa_100;
  QLabel *labela_100;

  QGroupBox *grdshift;
  QDoubleSpinBox *boxdshift;
  QLabel *labeldshift;

  //--------------------------------------------------------------
  QWidget *widgetwkm;
  QGroupBox *grh1;
  QPushButton *buttone1;
  QPushButton *buttonh1;
  QPushButton *buttonl1;
  QPushButton *buttonc1;
  QPushButton *buttoni1;
  QCheckBox *checkboxh1;
  QCheckBox *checkboxh1m;
  QGroupBox *grh2;
  QPushButton *buttone2;
  QPushButton *buttonh2;
  QPushButton *buttonl2;
  QPushButton *buttonc2;
  QPushButton *buttoni2;
  QCheckBox *checkboxh2;
  QCheckBox *checkboxh2m;
  QGroupBox *grh3;
  QPushButton *buttone3;
  QPushButton *buttonh3;
  QPushButton *buttonl3;
  QPushButton *buttonc3;
  QPushButton *buttoni3;
  QCheckBox *checkboxh3;
  QCheckBox *checkboxh3m;
  QGroupBox *grh4;
  QPushButton *buttone4;
  QPushButton *buttonh4;
  QPushButton *buttonl4;
  QPushButton *buttonc4;
  QPushButton *buttoni4;
  QCheckBox *checkboxh4;
  QCheckBox *checkboxh4m;
  QGroupBox *grh5;
  QPushButton *buttone5;
  QPushButton *buttonh5;
  QPushButton *buttonl5;
  QPushButton *buttonc5;
  QPushButton *buttoni5;
  QCheckBox *checkboxh5;
  QCheckBox *checkboxh5m;

  //=====================================================================
  QWidget *widgetmwatt;  //Fenster mW-Meter VFO
  QFrame *whline;  //

  QGroupBox *grmesskanal;
  QProgressBar *progressbarkanal;
  QSpinBox *spbarminkanal;
  QSpinBox *spbarmaxkanal;
  QLabel *labeldbmkanal;
  QLabel *labelvoltkanal;
  QLabel *labelwattkanal;
  QLabel *labeladckanal;
  QSpinBox *sphangkanal;
  QLabel *labelhangkanal;
  QCheckBox *checkkanal;
  QSpinBox *spattextkanal;
  QLabel *labelattextkanal;
  QSpinBox *spdbmdigitkanal;
  QLabel *labeldbmdigitkanal;

  QGroupBox *grmessvfo;
  QSpinBox *sp1hz;
  QSpinBox *sp10hz;
  QSpinBox *sp100hz;
  QSpinBox *sp1khz;
  QSpinBox *sp10khz;
  QSpinBox *sp100khz;
  QSpinBox *sp1mhz;
  QSpinBox *sp10mhz;
  QSpinBox *sp100mhz;
  QSpinBox *sp1ghz;
  QLabel *labelsphz;
  QLabel *labelspkhz;
  QLabel *labelspmhz;
  QPushButton *bvfo2sp;
  QPushButton *bsp2vfo;

  QGroupBox *grlcda;
  QLCDNumber *LCD1;
  QRadioButton *rb1;
  QLCDNumber *LCD2;
  QRadioButton *rb2;
  QLCDNumber *LCD3;
  QRadioButton *rb3;
  QLCDNumber *LCD4;
  QRadioButton *rb4;
  QLCDNumber *LCD5;
  QRadioButton *rb5;
  //=====================================================================
  //////////////////////////////////////////////////////////////////////////////
  //Objecte fuer Berechnungen
  QWidget *wgberechnung1;  //Berechnungen

  QGroupBox *groupschwingkreis;
  QLineEdit *editf1;
  QLabel *labelf1;
  QLineEdit *editc1;
  QLabel *labelc1;
  QLineEdit *editl1;
  QLabel *labell1;
  QLabel *ergebnisxc;
  QLabel *labelxc;
  QLabel *ergebnisxl;
  QLabel *labelxl;

  QLineEdit *editn;
  QLabel *labeln;
  QLineEdit *edital;
  QLabel *labelal;

  QGroupBox *groupwindungen;
  QLineEdit *edital3;
  QLabel *labeledital3;
  QLineEdit *editl3;
  QLabel *labeleditl3;
  QLabel *ergebnisw;
  QLabel *labelergebnisw;

  QGroupBox *grdbmvw;
  QDoubleSpinBox *spdbm;
  QCheckBox *cbdbm01;
  QLabel *edvolt;
  QLabel *edwatt;

  QGroupBox *grrloss;
  QDoubleSpinBox *sprloss;
  QCheckBox *cbrloss01;
  QLabel *labelswv;
  QLabel *labelrfaktor;
  QLabel *labelzlow;
  QLabel *labelzhigh;

  //Objecte fuer Berechnungen
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  //Objecte fuer Impedanz
  QWidget *wgberechnung2;  //Berechnungen

  QGroupBox *gimp;
  QRadioButton *rbr;
  QRadioButton *rblc;

  QGroupBox *gzr;
  QLabel *labeleditz1;
  QLineEdit *editz1;
  QLabel *labeleditz2;
  QLineEdit *editz2;
  QLabel *labeleditz3;
  QLineEdit *editz3;
  QLabel *labeleditz4;
  QLineEdit *editz4;
  QLabel *lr1;
  QLabel *lbeschrr1;
  QLabel *lr2;
  QLabel *lbeschrr2;
  QLabel *lr3;
  QLabel *lbeschrr3;
  QLabel *lr4;
  QLabel *lbeschrr4;
  QLabel *ldaempfung;
  QLabel *lbeschrdaempfung;
  QLabel *bild1;

  QGroupBox *gzlc;
  QLabel *labeleditzlc1;
  QLineEdit *editzlc1;
  QLabel *labeleditzlc2;
  QLineEdit *editzlc2;
  QLabel *labeleditzlc3;
  QLineEdit *editzlc3;
  QLabel *ll;
  QLabel *lbeschrl;
  QLabel *lc;
  QLabel *lbeschrc;
  QLabel *bild2;
  //Objecte fuer Impedanz Ende
  //////////////////////////////////////////////////////////////////////////////

  void setSpeki(bool);
  void testFrqMinMax();
  double frunden(double, double, double);

  void createMenus();
  void createActions();
  void createToolBars();
  void createStatusBar();

  void writeSettings();
  void readSettings();
  void grafiksetzen();

  void cfgspeichern();
  void clrrxbuffer();
  void sendbefwobbeln();
  void setInfoText();
  void loadMk(QString);
  void setMkInfo();
  void delay(int);
  void KurveInHintergrund(THMessKurve &);
  QString mwatt2str(double);
  QString volt2str(double);

  double linenormalisieren(const QString &line);
  void WobbelGrundDatenToEdit();
  void WobbelGrundDatenToDisplay();
  QString frq2str(double, char);
  double str2frq(const QString &);
  void wobnormalisieren();
  TWobbelFrqStr wfrqstr;
  void wobedit2str();
  void korrKurveGeladen();
  void setMessVfoFrq(double);
  double getMessVfoFrq();
  void setMessLabel1();
  double db2swv(double);
  QString datetime2filename();
  QString fnamenormal(QString &);
  void  set0hz();
  void setFonts();
  void messkurveClr();
  void spurClr();
  void messkurveSetSpur();
  void kalibrierende();


  Modem *picmodem;
  QString srs232;
  bool rs232isopen;
  bool bwstart;

  QTimer *repainttimer;
  QTimer *openttytimer;
  QTimer *befantworttimer;
  QTimer *delaytimer;
  QTimer *wobwiederholtimer;
  bool tstop;
  QTimer *ddstakttimer;
  QTimer *svfotimer;
  QTimer *messvfotimer;
  QTimer *mhangtimer;
  QTimer *wachtimer;
  QTimer *rs232stoptimer;

  QTime twobbel;

  QFont fontapp;

  int adckanal;
  int synclevel;
  int abbruchlevel;
  int messattext;
  double frqcalpunkt1;
  int idlezaehler;
  unsigned char rxbuffer[maxmesspunkte*4]; //maximale Anzahl
  int idxbufferwrite;
  int idxbufferread;
  QString qsbefehl;
  bool befehlanfang;
  bool befehlende;
  bool bmwcalp1;
  bool bmwcalp2;
  int widx;
  int index;
  char c,c1;
  //bool bdemo;
  Tkabeldaten kabeldaten;

  QStringList listtty;
  int idtty;
  bool bvantwort;
  bool bnwtgefunden;
  QString geraet;
  PlotSettings ksettings;
  QStringList mkinfokanal;
  QStringList merkmkinfokanal;
  QString kurvebeschr;

  QColor colorhline1;
  QColor colorhline2;
  QColor colorhline3;
  QColor colorhline4;
  QColor colorhline5;

  double ymkwert2;
  double ymkwert1;
  double ymkwert2mw;
  double ymkwert1mw;
  double mwcalpegel1;
  double mwcalpegel2;


  double mdbmK1;
  double messreiheK1[100];
  double messreiheADC[100];
  double mwadcwert;
  double mdbmK1anzeige;
  int mdbmk1idx;

  bool wobbeinmal;
  bool syncwobbel;
  bool wiederholen;

  TWobbelGrundDaten wobbelgrunddaten;
  TWobbelGrundDaten wgrunddatenmerk;
  TMessKurve messkurve;
  TMessKurve messkurvemerk;
  THMessKurve hmesskurve1;
  THMessKurve hmesskurve2;
  THMessKurve hmesskurve3;
  THMessKurve hmesskurve4;
  THMessKurve hmesskurve5;
  TFrqmarken frqmarken;
  TGrunddaten grunddaten;
  TCalDaten caldaten;

  QPoint markertextpos;
  QMesskopf messkopf;

  QDir kurvendir;

  QString programpath;

  bool bkurvegeladen;//fuer Kurven laden usw.
  double frqpegel;
  double frqpegelalt;
  double swvline1alt;
  double swvline2alt;
  double a_laenge;//Kabellaenge
  double a_100;//Kabeldaempfung pro 100m
  double a_daempfung;//Kabeldaempfung

  bool bbefwobbeln;//fuer befantworttimerstop Daten vom PIC
  int  idxmesskurve;//Index in der Messkurve

  bool btip;
  bool bd; //BOOL Variable zum Debugen
  bool Mkneu;

  bool bmessenmw;
  int hangtk1; //Hangtime Messkanal 1
  double offsetk1;
  bool bdbmk1;//Umschaltung dBm/dBV

  bool bsetup;//Ob ich im Setup bin
  enum eschwingkreis{induktiv, kapazitiv, kein};
  eschwingkreis schwingkreis;

  double pegel1;
  double pegel2;
  int lcdspeicher;

  QStringList infolist;

  unsigned char catbefehl;
  int cal60counter;
  int cal60index;

private slots:
  void DisplaySetSpur(int);
  void fRS232Stop();
  void fCaldBm();
  void fCalmWm();
  void clickbvfo2sp();
  void clickbsp2vfo();
  void lcd1clicked();
  void lcd2clicked();
  void lcd3clicked();
  void lcd4clicked();
  void lcd5clicked();
  void setspattext(int);
  void DmesskurveClr();
  void bildspeichern();
  void beenden();
  void fopentty();
  void setAText(QString);
  void actrepainttimer();
  void clickbuttonwobbeln();
  void clickbuttoneinzeln();
  void clickbuttonstop();
  void befantworttimerstop();

  void normfanfangende();
  void normmittespan();
  void set_ztime(int);

  void setDisplayYmax(int i);
  void setDisplayYmin(int i);

  void checkboxkanal_checked(bool);
  void checkboxspeki_checked(bool);
  void checkboxwatt_checked(bool);
  void checkboxvolt_checked(bool);
  void checkbox3db_checked(bool);
  void checkbox3dbinv_checked(bool);
  void checkboxguete_checked(bool);
  void checkbox6db_checked(bool);
  void checkbox60db_checked(bool);
  void checkboxshape_checked(bool);
  void checkboxswvkanal_checked(bool);
  void checkboxdbmax_checked(bool);
  void checkboxdbmin_checked(bool);
  void setboxdblinie(double);
  void checkboxdblinie_checked(bool);
  void setboxswvlinie1(double);
  void checkboxswvlinie1_checked(bool);
  void setboxswvlinie2(double);
  void checkboxswvlinie2_checked(bool);

  void setboxdshift(double);
  void setswvEnabled();

  void sondeNeu();
  void setFlatCal();
  void setKurvenSpur(bool);
  void fKurvenAnalyse();
  void nachkalib();
  void kalibrierenMk();
  void speichernMesskopf();
  void saveMk(QString);
  void ladenMesskopf1();
  void ClrMesskopf();

  void KurvenLaden();
  void KurvenSichern(TMessKurve);
  void SaveKurve();
  void kurveCaption();
  void delaytimerstop();
  void wobwiederholtimerstop();
  void Info();
  void MesskopfInfoK1();
  void fwversion();
  void tip(bool);
  void setFrequenzmarken();
  void setColorH1();
  void clickbuttone1();
  void clickbuttonh1();
  void clickbuttonl1();
  void checkboxh1_checked(bool);
  void checkboxh1m_checked(bool);
  void setColorH2();
  void clickbuttone2();
  void clickbuttonh2();
  void clickbuttonl2();
  void checkboxh2_checked(bool);
  void checkboxh2m_checked(bool);
  void setColorH3();
  void clickbuttone3();
  void clickbuttonh3();
  void clickbuttonl3();
  void checkboxh3_checked(bool);
  void checkboxh3m_checked(bool);
  void setColorH4();
  void clickbuttone4();
  void clickbuttonh4();
  void clickbuttonl4();
  void checkboxh4_checked(bool);
  void checkboxh4m_checked(bool);
  void setColorH5();
  void clickbuttone5();
  void clickbuttonh5();
  void clickbuttonl5();
  void checkboxh5_checked(bool);
  void checkboxh5m_checked(bool);
  void infoh1();
  void infoh2();
  void infoh3();
  void infoh4();
  void infoh5();
  void setSetup();
  void ddstakttimerstop();
  void svfotimerstop();
  void setmessvfo();
  void messvfotimerstop();
  void tabumschalten( int );
  void wachtimerstop();
  void setbarkanal();
  void mhangk1timerstop();
  void setsphang(int);
  void setdbmdigit(int);
  //void checkk1_checked(bool);
  void checkmwkanal_checked(bool);
  void setalaenge(double);
  void seta_100(double);

  void schwingkreisf();
  void schwingkreisl();
  void schwingkreisc();
  void schwingkreisal();
  void alwindungen();

  void setimp();
  void zlcausrechnen();
  void zrausrechnen();
  void setvw(double);
  void cbdbm01_checked(bool);
  void setrz(double);
  void cbrloss01_checked(bool);
  void setNoFrqCalK1(bool);

  void fKabeldaten();
  void getinfolist(QStringList);

protected:
  virtual void resizeEvent( QResizeEvent *);
  virtual void repaintEvent( QPaintEvent *);
  void closeEvent(QCloseEvent *event);

signals:
  void dbildspeichern(QString);
};



#endif // nwt4window_H
