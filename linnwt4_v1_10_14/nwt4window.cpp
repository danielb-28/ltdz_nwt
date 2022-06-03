/*
#include <QtGui>
#include <QtCore>
*/

#include "nwt4window.h"
#include "calibdlg.h"
#include "qfmarkedlg.h"
#include "qsetupdlg.h"
#include "hkurveproperty.h"
#include "qkurvenanalyse.h"

Nwt4Window::Nwt4Window(QWidget *parent):QMainWindow(parent)
{
  bd=false; // nur zum Debugen

  if(bd)qDebug("Nwt4Window::Nwt4Window");
  infoversion = "1.10.14";
  infodatum = "21.06.2021";
  //==========================================
  QString infoqt, infotarget;
#if QT_VERSION >= 0x050000
  infoqt="QT5";
#else
  infoqt="QT4";
#endif
#ifdef Q_OS_WIN
  infotarget="NWT4000win";
#else
  infotarget="NWT4000lin";
#endif
  //==========================================
  infotext = tr("<h2>%1</h2>"
                "<p><B>Version %2</B>\t\t%3 </p>"
                "<p>Entwickelt unter Linux mit %4</p>"
                "<p>(c) Andreas Lindenau DL4JAL<br>"
                "DL4JAL@t-online.de<br>"
                "http://www.dl4jal.eu</p>").arg(infotarget).arg(infoversion).arg(infodatum).arg(infoqt);
  //qDebug()<<"SW-Version:"<<infoversion<<", Datum:"<<infodatum;
  /*****************************************************************************/
  picmodem = new Modem();
  QObject::connect( picmodem, SIGNAL(setTtyText(QString)), this, SLOT(setAText(QString)));
  /*****************************************************************************/
  grunddaten.version=0;//FW Version
  grunddaten.variante=vnwt4_1;//FW Variante an HW angepasst

  wobbelgrunddaten.dbline = 0.0; //0dB = Zusatzlinie ausgeblendet
  wobbelgrunddaten.dbmax  = 10.0;//oberer dB Wert im Wobbeldisplay
  wobbelgrunddaten.dbmin  = -90.0;//unterer dB Wert im Wobbeldisplay
  wobbelgrunddaten.frequenzanfang = 138.0;//Frq Start 35.0 MHz
  wobbelgrunddaten.frequenzschritt = 8.524;//Schrittweite 8,73 MHz
  wobbelgrunddaten.mpunkte = 501;// Messpunkte
  wobbelgrunddaten.dshift=0.0;
  wobbelgrunddaten.kshift=0.0;
  wobbelgrunddaten.calfrqmin=35.0;
  wobbelgrunddaten.calfrqmax=4400.0;
  wobbelgrunddaten.ztime=0;
  wobbelgrunddaten.bdBm=false;//kein Spektrumanalyser
  wobbelgrunddaten.b0dBline=true;

  grunddaten.homedir.setPath(QDir::homePath());//homepath voreinstellen
  grunddaten.formatfilename="yyMMdd_hhmmss";
  grunddaten.spath="nwt4";//Pfad Voreinstellung fuer Konfiguration
  settingspath="NWT4";//Pfad fuer QSettings
  grunddaten.nwttaktcalfrq=1000000000.0;//1Ghz
  grunddaten.nwttakt=1000000000.0;//1Ghz
  grunddaten.bflatcal=false;//es wurde noch keine Flatnes Kalibrierung durchgefuehrt

  caldaten.bmkneu=false;
  caldaten.adc1=0.0;
  caldaten.adc2=0.0;
  caldaten.adcmw1=0.0;
  caldaten.adcmw2=0.0;
  caldaten.att=0.0;

  ksettings.setminY(wobbelgrunddaten.dbmin);
  ksettings.setmaxY(wobbelgrunddaten.dbmax);//in Display uebertragen
  ksettings.setmaxX(4400.0);//X-Achse 150MHz Stop
  ksettings.setminX(35.0);//X-Achse 100kHz Start
  ksettings.swvlinie1=0.0;//keine swv Linie
  ksettings.swvlinie2=0.0;//keine swv Linie
  ksettings.markerfontsize=12;//Schriftgroesse des Markertextes
  ksettings.fontsize=12;//Schriftgroesse der Programmtexte
  ksettings.setInfoversion(infoversion);

  tabwidget = new QTabWidget();
  // Tabumschalten steuern

  nwt4widget = new QWidget();
  setCentralWidget(tabwidget);
  tabwidget->addTab(nwt4widget, tr("Wobbeln","TabWindow"));
  display = new WDisplay(nwt4widget);
  QObject::connect( display, SIGNAL( frqanfang2edit(double)), this, SLOT(frqanfangdisplay2edit(double)));
  QObject::connect( display, SIGNAL( frqende2edit(double)), this, SLOT(frqendedisplay2edit(double)));
  QObject::connect( display, SIGNAL( frqmitte2edit(double)), this, SLOT(frqmittedisplay2edit(double)));
  QObject::connect( display, SIGNAL( frq2berechnung(double)), this, SLOT(frq2berechnung(double)));
  QObject::connect( display, SIGNAL( kdaten(Tkabeldaten)), this, SLOT(kdaten(Tkabeldaten)));
  QObject::connect( display, SIGNAL( wkurveclr()), this, SLOT(DmesskurveClr()));
  QObject::connect( display, SIGNAL( sendinfolist(QStringList)), this, SLOT(getinfolist(QStringList)));

  QObject::connect( this, SIGNAL( dbildspeichern(QString)), display, SLOT(bildspeichern(QString)));

  buttonwobbeln = new QPushButton(nwt4widget);
  buttonwobbeln->setText(tr("Wobbeln","Tab Wobbeln"));
  QObject::connect( buttonwobbeln, SIGNAL( clicked()), this, SLOT(clickbuttonwobbeln()));
  buttonwobbeln->setEnabled(false);
  buttonwobbeln->setStatusTip(tr("Dauerhaftes Wobbeln", "Statustip Tab Wobbeln"));

  buttoneinzeln = new QPushButton(nwt4widget);
  buttoneinzeln->setText(tr("Einzeln","Tab Wobbeln"));
  QObject::connect( buttoneinzeln, SIGNAL( clicked()), this, SLOT(clickbuttoneinzeln()));
  buttoneinzeln->setEnabled(false);
  buttoneinzeln->setStatusTip(tr("Einmaliger Wobbeldurchlauf", "Statustip Tab Wobbeln"));

  buttonstop = new QPushButton(nwt4widget);
  buttonstop->setText(tr("Stop","Tab Wobbeln"));
  QObject::connect( buttonstop, SIGNAL( clicked()), this, SLOT(clickbuttonstop()));
  buttonstop->setEnabled(false);
  buttonstop->setStatusTip(tr("STOP dauerhaftes Wobbeln", "Statustip Tab Wobbeln"));

  rlauf = new QProgressBar(nwt4widget); //Anzeigebalken der RS232 Daten beim Wobbeln
  rlauf->setTextVisible(true);
  //########################################################################################
  tabwidgetwobbeln = new QTabWidget(nwt4widget);
  tabwidgetwobbeln->setTabPosition(QTabWidget::South);

  widgetwobbelfrq = new QWidget();
  widgetwobbelfrq->setStatusTip(tr("Wobbeln. Frequenzeinstellungen", "Statustip Wobbeln. Tab Frequenzeinstellungen"));

  /*
  QScrollArea* scrollArea = new QScrollArea;
  scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  scrollArea->setWidgetResizable( true );
  scrollArea->setWidget( widgetwobbelfrq );
  */

  tabwidgetwobbeln->addTab(widgetwobbelfrq, tr("Frq.","TabWindow Frq Einstellungen"));

  grwobbel = new QGroupBox(tr("SET-Wobbel","Frq Wobbeln"),widgetwobbelfrq);
  lineEdit_frq_anfang = new QLineEdit(grwobbel);
  label_frq_anfang = new QLabel(grwobbel);
  label_frq_anfang->setText(tr("Start","Frq Wobbeln"));

  lineEdit_frq_ende = new QLineEdit(grwobbel);
  label_frq_ende = new QLabel(grwobbel);
  label_frq_ende->setText(tr("Stop","Frq Wobbeln"));

  lineEdit_frq_mpunkte = new QLineEdit(grwobbel);
  label_mpunkte = new QLabel(grwobbel);
  label_mpunkte->setText(tr("Steps","Frq Wobbeln"));

  lineEdit_schrittweite = new QLineEdit(grwobbel);
  lineEdit_schrittweite->setEnabled(false);
  label_frq_schrittweite = new QLabel(grwobbel);
  label_frq_schrittweite->setText(tr("Step","Frq Wobbeln"));
  label_frq_schrittweite->setEnabled(false);

  lineEdit_frq_mitte = new QLineEdit(grwobbel);
  label_frq_mitte = new QLabel(grwobbel);
  label_frq_mitte->setText(tr("Center","Frq Wobbeln"));

  lineEdit_frq_span = new QLineEdit(grwobbel);
  label_frq_span = new QLabel(grwobbel);
  label_frq_span->setText(tr("Span","Frq Wobbeln"));

  boxztime = new QSpinBox(grwobbel);
  boxztime->setRange(0,9);
  boxztime->setSingleStep(1);
  boxztime->setValue(0);
  label_ztime = new QLabel(grwobbel);
  label_ztime->setText(tr("Zwischenzeit","Frq Wobbeln"));
  QObject::connect(boxztime, SIGNAL(valueChanged(int)), this, SLOT(set_ztime(int)));

  QObject::connect(lineEdit_frq_anfang, SIGNAL(returnPressed ()), this, SLOT(normfanfangende()));
  QObject::connect(lineEdit_frq_ende, SIGNAL(returnPressed ()), this, SLOT(normfanfangende()));
  QObject::connect(lineEdit_frq_mpunkte, SIGNAL(returnPressed ()), this, SLOT(normfanfangende()));
  QObject::connect(lineEdit_frq_mitte, SIGNAL(returnPressed ()), this, SLOT(normmittespan()));
  QObject::connect(lineEdit_frq_span, SIGNAL(returnPressed ()), this, SLOT(normmittespan()));

  grygrenzen = new QGroupBox(tr("Display Y Grenzen","GroupBox"),widgetwobbelfrq);
  labelboxydbmax = new QLabel(tr("max(dB)","Label Display Y Grenzen"),grygrenzen);
  labelboxydbmin = new QLabel(tr("min(dB)","Label Display Y Grenzen"),grygrenzen);
  boxydbmax = new QSpinBox(grygrenzen);
  boxydbmax->setRange(-70,50);
  boxydbmax->setSingleStep(10);
  boxydbmax->setValue(10);
  boxydbmin = new QSpinBox(grygrenzen);
  boxydbmin->setRange(-120,-10);
  boxydbmin->setSingleStep(10);
  boxydbmin->setValue(-90);
  QObject::connect( boxydbmax, SIGNAL(valueChanged ( int )), this, SLOT(setDisplayYmax(int)));
  QObject::connect( boxydbmin, SIGNAL(valueChanged ( int )), this, SLOT(setDisplayYmin(int)));

  grkanal = new QGroupBox(tr("Wobbelkanal","GroupBox Wobbelkanal"),widgetwobbelfrq);
  checkboxkanal = new QCheckBox(grkanal);
  checkboxkanal->setText(tr("Messk.","CheckBox Kanal"));
  checkboxkanal->setChecked(true);
  labelkanal = new QLabel("",grkanal);
  labelkanal->setWordWrap(true);
  QObject::connect( checkboxkanal, SIGNAL( toggled (bool)), this, SLOT(checkboxkanal_checked(bool)));
  buttonloadmessk = new QPushButton(grkanal);
  buttonloadmessk->setText(tr("Mk. Laden","Button Mk. Laden"));
  QObject::connect( buttonloadmessk, SIGNAL( clicked()), this, SLOT(ladenMesskopf1()));
  buttonloadmessk->setEnabled(true);
  buttonloadmessk->setStatusTip(tr("Datendatei eines Messkopfes laden", "Statustip Mk. Laden"));
  buttoninfomessk = new QPushButton(grkanal);
  buttoninfomessk->setText("i");
  QObject::connect( buttoninfomessk, SIGNAL( clicked()), this, SLOT(MesskopfInfoK1()));
  buttoninfomessk->setStatusTip(tr("Informationen ueber den Messkopf", "Statustip Informationen ueber den Messkopf"));
  checkboxspeki = new QCheckBox(grkanal);
  checkboxspeki->setText(tr("Spektrumanalyse"));
  QObject::connect( checkboxspeki, SIGNAL( toggled (bool)), this, SLOT(checkboxspeki_checked(bool)));

  //########################################################################################
  widgetdb = new QWidget();
  widgetdb->setStatusTip(tr("Wobbeln: Bandbreiten ", "Statustip Widget"));
  tabwidgetwobbeln->addTab(widgetdb, tr("dB 1","TabWindow Bandbreiten"));

  grbandbreite = new QGroupBox(tr("Bandbreiten / Werte","GroupBox Bandbreiten / Werte"),widgetdb);

  checkboxwatt = new QCheckBox(grbandbreite);
  checkboxwatt->setText(tr("Watt","CheckBox Bandbreiten / Werte"));
  checkboxwatt->setChecked(false);
  QObject::connect( checkboxwatt, SIGNAL( toggled (bool)), this, SLOT(checkboxwatt_checked(bool)));

  checkboxvolt = new QCheckBox(grbandbreite);
  checkboxvolt->setText(tr("Volt","CheckBox Bandbreiten / Werte"));
  checkboxvolt->setChecked(false);
  QObject::connect( checkboxvolt, SIGNAL( toggled (bool)), this, SLOT(checkboxvolt_checked(bool)));

  checkboxdbmin = new QCheckBox(grbandbreite);
  checkboxdbmin->setText(tr("dB Minimum","CheckBox Bandbreiten / Werte"));
  checkboxdbmin->setChecked(false);
  QObject::connect( checkboxdbmin, SIGNAL( toggled (bool)), this, SLOT(checkboxdbmin_checked(bool)));

  checkboxdbmax = new QCheckBox(grbandbreite);
  checkboxdbmax->setText(tr("dB Maximum","CheckBox Bandbreiten / Werte"));
  checkboxdbmax->setChecked(false);
  QObject::connect( checkboxdbmax, SIGNAL( toggled (bool)), this, SLOT(checkboxdbmax_checked(bool)));

  checkbox3db = new QCheckBox(grbandbreite);
  checkbox3db->setText(tr("3dB Bandbreite","CheckBox Bandbreiten / Werte"));
  checkbox3db->setChecked(false);
  QObject::connect( checkbox3db, SIGNAL( toggled (bool)), this, SLOT(checkbox3db_checked(bool)));

  checkbox3dbinv = new QCheckBox(grbandbreite);
  checkbox3dbinv->setText(tr("Invers","CheckBox Bandbreiten / Werte"));
  checkbox3dbinv->setChecked(false);
  QObject::connect( checkbox3dbinv, SIGNAL( toggled (bool)), this, SLOT(checkbox3dbinv_checked(bool)));

  checkboxguete = new QCheckBox(grbandbreite);
  checkboxguete->setText(tr("Guete Q","CheckBox Bandbreiten / Werte"));
  checkboxguete->setChecked(false);
  QObject::connect( checkboxguete, SIGNAL( toggled (bool)), this, SLOT(checkboxguete_checked(bool)));

  checkbox6db = new QCheckBox(grbandbreite);
  checkbox6db->setText(tr("6dB Bandbreite","CheckBox Bandbreiten / Werte"));
  checkbox6db->setChecked(false);
  QObject::connect( checkbox6db, SIGNAL( toggled (bool)), this, SLOT(checkbox6db_checked(bool)));

  checkbox60db = new QCheckBox(grbandbreite);
  checkbox60db->setText(tr("60dB Bandbreite","CheckBox Bandbreiten / Werte"));
  checkbox60db->setChecked(false);
  QObject::connect( checkbox60db, SIGNAL( toggled (bool)), this, SLOT(checkbox60db_checked(bool)));

  checkboxshape = new QCheckBox(grbandbreite);
  checkboxshape->setText(tr("Shape-Faktor","CheckBox Bandbreiten / Werte"));
  checkboxshape->setChecked(false);
  QObject::connect( checkboxshape, SIGNAL( toggled (bool)), this, SLOT(checkboxshape_checked(bool)));

  checkboxswvkanal = new QCheckBox(grbandbreite);
  checkboxswvkanal->setText(tr("SWV Anzeige","CheckBox Bandbreiten / Werte"));
  checkboxswvkanal->setChecked(false);
  QObject::connect( checkboxswvkanal, SIGNAL( toggled (bool)), this, SLOT(checkboxswvkanal_checked(bool)));

  grzusatzlinien = new QGroupBox(tr("Zusatzlinien","GroupBox Zusatzlinien"),widgetdb);
  labelboxdblinie = new QLabel(tr("dB Linie","Label Zusatzlinien"),grzusatzlinien);
  boxdblinie = new QDoubleSpinBox(grzusatzlinien);
  boxdblinie->setDecimals(1);
  boxdblinie->setMaximum(100.0);
  boxdblinie->setMinimum(-100.0);
  boxdblinie->setSingleStep(0.1);
  QObject::connect( boxdblinie, SIGNAL( valueChanged ( double)), this, SLOT(setboxdblinie(double)));
  checkboxdblinie = new QCheckBox(grzusatzlinien);
  checkboxdblinie->setText("0,1dB");
  checkboxdblinie->setChecked(true);
  QObject::connect( checkboxdblinie, SIGNAL( toggled (bool)), this, SLOT(checkboxdblinie_checked(bool)));

  labelboxswvlinie1 = new QLabel(tr("SWV Linie","Label SWV Linie"),grzusatzlinien);
  labelboxswvlinie1->setEnabled(false);
  boxswvlinie1 = new QDoubleSpinBox(grzusatzlinien);
  boxswvlinie1->setDecimals(3);
  boxswvlinie1->setMaximum(5.0);
  boxswvlinie1->setMinimum(1.001);
  boxswvlinie1->setSingleStep(0.001);
  boxswvlinie1->setEnabled(false);
  QObject::connect( boxswvlinie1, SIGNAL( valueChanged ( double)), this, SLOT(setboxswvlinie1(double)));
  checkboxswvlinie1 = new QCheckBox(grzusatzlinien);
  checkboxswvlinie1->setText(tr("aktiv","CheckBox SWV Linie"));
  checkboxswvlinie1->setChecked(false);
  QObject::connect( checkboxswvlinie1, SIGNAL( toggled (bool)), this, SLOT(checkboxswvlinie1_checked(bool)));

  labelboxswvlinie2 = new QLabel(tr("SWV Linie","Label SWV Linie"),grzusatzlinien);
  labelboxswvlinie2->setEnabled(false);
  boxswvlinie2 = new QDoubleSpinBox(grzusatzlinien);
  boxswvlinie2->setDecimals(3);
  boxswvlinie2->setMaximum(5.0);
  boxswvlinie2->setMinimum(1.001);
  boxswvlinie2->setValue(1.001);
  boxswvlinie2->setSingleStep(0.001);
  boxswvlinie2->setEnabled(false);
  QObject::connect( boxswvlinie2, SIGNAL( valueChanged ( double)), this, SLOT(setboxswvlinie2(double)));
  checkboxswvlinie2 = new QCheckBox(grzusatzlinien);
  checkboxswvlinie2->setText(tr("aktiv","CheckBox SWV Linie"));
  checkboxswvlinie2->setChecked(false);
  QObject::connect( checkboxswvlinie2, SIGNAL( toggled (bool)), this, SLOT(checkboxswvlinie2_checked(bool)));


  //########################################################################################
  widgetdb2 = new QWidget();
  widgetdb2->setStatusTip(tr("Wobbeln: Bandbreiten ", "Statustip Wobbeln: Bandbreiten"));
  tabwidgetwobbeln->addTab(widgetdb2, "dB 2");

  grswvant = new QGroupBox(tr("SWV ohne Kabeldaempfung","GroupBox SWV ohne Kabeldaempfung"),widgetdb2);
  boxa_100 = new QDoubleSpinBox(grswvant);
  boxa_100->setDecimals(1);
  boxa_100->setMinimum(0.0);
  boxa_100->setMaximum(99.9);
  boxa_100->setValue(0.0);
  boxa_100->setSuffix(" db/100m");
  QObject::connect( boxa_100, SIGNAL( valueChanged ( double)), this, SLOT(seta_100(double)));
  labela_100 = new QLabel(tr("Daempf.","QDoubleSpinBox Daempf."),grswvant);
  boxalaenge = new QDoubleSpinBox(grswvant);
  boxalaenge->setDecimals(1);
  boxalaenge->setMinimum(0.0);
  boxalaenge->setMaximum(1000.0);
  boxalaenge->setValue(0.0);
  boxalaenge->setSuffix(" m");
  QObject::connect( boxalaenge, SIGNAL( valueChanged ( double)), this, SLOT(setalaenge(double)));
  labelalaenge = new QLabel(tr("Kabellaenge","QDoubleSpinBox Kabellaenge"),grswvant);

  grdshift = new QGroupBox(tr("S21 Messdaempfung"),widgetdb2);
  labeldshift = new QLabel("(dB)",grdshift);
  boxdshift = new QDoubleSpinBox(grdshift);
  boxdshift->setDecimals(0);
  boxdshift->setMaximum(90.0);
  boxdshift->setMinimum(0.0);
  boxdshift->setSingleStep(1.0);
  QObject::connect( boxdshift, SIGNAL( valueChanged ( double)), this, SLOT(setboxdshift(double)));

  //########################################################################################
  widgetwkm = new QWidget();
  widgetwkm->setStatusTip(tr("Wobbel Kurven Manager", "Statustip Widget Wobbel Kurven Manager"));
  tabwidgetwobbeln->addTab(widgetwkm, "WKM");

  grh1 = new QGroupBox(tr("Kurve 1","GroupBox Kurve 1"),widgetwkm);
  buttone1 = new QPushButton(grh1);
  buttone1->setText("...");
  QObject::connect( buttone1, SIGNAL( clicked()), this, SLOT(clickbuttone1()));
  buttone1->setStatusTip(tr("Messkurve Eigenschaften", "Statustip Messkurve Eigenschaften"));
  buttone1->setEnabled(false);
  buttonh1 = new QPushButton(grh1);
  buttonh1->setText(tr("Holen","Button Holen"));
  QObject::connect( buttonh1, SIGNAL( clicked()), this, SLOT(clickbuttonh1()));
  buttonh1->setStatusTip(tr("Messkurve in den Hintergrund laden", "Statustip Messkurve in den Hintergrund laden"));
  buttonh1->setEnabled(false);
  buttonl1 = new QPushButton(grh1);
  buttonl1->setText(tr("Laden","Button Messkurve in den Hintergrund laden"));
  QObject::connect( buttonl1, SIGNAL( clicked()), this, SLOT(clickbuttonl1()));
  buttonl1->setStatusTip(tr("Messkurve aus Datei in den Hintergrund laden", "Statustip Messkurve aus Datei in den Hintergrund laden"));
  buttonc1 = new QPushButton(grh1);
  buttonc1->setText("Color");
  QObject::connect( buttonc1, SIGNAL( clicked()), this, SLOT(setColorH1()));
  buttonc1->setStatusTip(tr("Color der Hintergrundkurve setzen", "Statustip Color der Hintergrundkurve setzen"));
  buttoni1 = new QPushButton(grh1);
  buttoni1->setText(tr("i"));
  QObject::connect( buttoni1, SIGNAL( clicked()), this, SLOT(infoh1()));
  buttoni1->setStatusTip(tr("Beschreibung der Kurve lesen", "Statustip Beschreibung der Kurve lesen"));
  checkboxh1 = new QCheckBox(grh1);
  checkboxh1->setText(tr("Kurve 1","CheckBox Kurve 1"));
  checkboxh1->setEnabled(false);
  checkboxh1->setChecked(false);
  QObject::connect( checkboxh1, SIGNAL( toggled (bool)), this, SLOT(checkboxh1_checked(bool)));
  checkboxh1m = new QCheckBox(grh1);
  checkboxh1m->setText(tr("Frequenz-Marker sichtbar","CheckBox Frequenz-Marker sichtbar"));
  checkboxh1m->setEnabled(false);
  checkboxh1m->setChecked(false);
  QObject::connect( checkboxh1m, SIGNAL( toggled (bool)), this, SLOT(checkboxh1m_checked(bool)));

  grh2 = new QGroupBox(tr("Kurve 2","GroupBox Kurve 2"),widgetwkm);
  buttone2 = new QPushButton(grh2);
  buttone2->setText("...");
  QObject::connect( buttone2, SIGNAL( clicked()), this, SLOT(clickbuttone2()));
  buttone2->setStatusTip(tr("Messkurve Eigenschaften", "Statustip Messkurve Eigenschaften"));
  buttone2->setEnabled(false);
  buttonh2 = new QPushButton(grh2);
  buttonh2->setText(tr("Holen","Button Holen"));
  QObject::connect( buttonh2, SIGNAL( clicked()), this, SLOT(clickbuttonh2()));
  buttonh2->setStatusTip(tr("Messkurve in den Hintergrund laden", "Statustip Messkurve in den Hintergrund laden"));
  buttonh2->setEnabled(false);
  buttonl2 = new QPushButton(grh2);
  buttonl2->setText(tr("Laden"));
  QObject::connect( buttonl2, SIGNAL( clicked()), this, SLOT(clickbuttonl2()));
  buttonl2->setStatusTip(tr("Messkurve aus Datei in den Hintergrund laden", "Statustip Messkurve aus Datei in den Hintergrund laden"));
  buttonc2 = new QPushButton(grh2);
  buttonc2->setText("Color");
  QObject::connect( buttonc2, SIGNAL( clicked()), this, SLOT(setColorH2()));
  buttonc2->setStatusTip(tr("Color der Hintergrundkurve setzen", "Statustip Color der Hintergrundkurve setzen"));
  buttoni2 = new QPushButton(grh2);
  buttoni2->setText(tr("i"));
  QObject::connect( buttoni2, SIGNAL( clicked()), this, SLOT(infoh2()));
  buttoni2->setStatusTip(tr("Beschreibung der Kurve lesen", "Statustip Beschreibung der Kurve lesen"));
  checkboxh2 = new QCheckBox(grh2);
  checkboxh2->setText(tr("Kurve 2","CheckBox Kurve 2"));
  checkboxh2->setEnabled(false);
  checkboxh2->setChecked(false);
  QObject::connect( checkboxh2, SIGNAL( toggled (bool)), this, SLOT(checkboxh2_checked(bool)));
  checkboxh2m = new QCheckBox(grh2);
  checkboxh2m->setText(tr("Frequenz-Marker sichtbar","CheckBox Frequenz-Marker sichtbar"));
  checkboxh2m->setEnabled(false);
  checkboxh2m->setChecked(false);
  QObject::connect( checkboxh2m, SIGNAL( toggled (bool)), this, SLOT(checkboxh2m_checked(bool)));

  grh3 = new QGroupBox(tr("Kurve 3","GroupBox Kurve 3"),widgetwkm);
  buttone3 = new QPushButton(grh3);
  buttone3->setText("...");
  QObject::connect( buttone3, SIGNAL( clicked()), this, SLOT(clickbuttone3()));
  buttone3->setStatusTip(tr("Messkurve Eigenschaften", "Statustip Messkurve Eigenschaften"));
  buttone3->setEnabled(false);
  buttonh3 = new QPushButton(grh3);
  buttonh3->setText(tr("Holen","Button Holen"));
  QObject::connect( buttonh3, SIGNAL( clicked()), this, SLOT(clickbuttonh3()));
  buttonh3->setStatusTip(tr("Messkurve in den Hintergrund laden", "Statustip Messkurve in den Hintergrund laden"));
  buttonh3->setEnabled(false);
  buttonl3 = new QPushButton(grh3);
  buttonl3->setText(tr("Laden"));
  QObject::connect( buttonl3, SIGNAL( clicked()), this, SLOT(clickbuttonl3()));
  buttonl3->setStatusTip(tr("Messkurve aus Datei in den Hintergrund laden", "Statustip Messkurve aus Datei in den Hintergrund laden"));
  buttonc3 = new QPushButton(grh3);
  buttonc3->setText("Color");
  QObject::connect( buttonc3, SIGNAL( clicked()), this, SLOT(setColorH3()));
  buttonc3->setStatusTip(tr("Color der Hintergrundkurve setzen", "Statustip Color der Hintergrundkurve setzen"));
  buttoni3 = new QPushButton(grh3);
  buttoni3->setText(tr("i"));
  QObject::connect( buttoni3, SIGNAL( clicked()), this, SLOT(infoh3()));
  buttoni3->setStatusTip(tr("Beschreibung der Kurve lesen", "Statustip Beschreibung der Kurve lesen"));
  checkboxh3 = new QCheckBox(grh3);
  checkboxh3->setText(tr("Kurve 3","CheckBox Kurve 3"));
  checkboxh3->setEnabled(false);
  checkboxh3->setChecked(false);
  QObject::connect( checkboxh3, SIGNAL( toggled (bool)), this, SLOT(checkboxh3_checked(bool)));
  checkboxh3m = new QCheckBox(grh3);
  checkboxh3m->setText(tr("Frequenz-Marker sichtbar","CheckBox Frequenz-Marker sichtbar"));
  checkboxh3m->setEnabled(false);
  checkboxh3m->setChecked(false);
  QObject::connect( checkboxh3m, SIGNAL( toggled (bool)), this, SLOT(checkboxh3m_checked(bool)));

  grh4 = new QGroupBox(tr("Kurve 4","GroupBox Kurve 4"),widgetwkm);
  buttone4 = new QPushButton(grh4);
  buttone4->setText("...");
  QObject::connect( buttone4, SIGNAL( clicked()), this, SLOT(clickbuttone4()));
  buttone4->setStatusTip(tr("Messkurve Eigenschaften", "Statustip Messkurve Eigenschaften"));
  buttone4->setEnabled(false);
  buttonh4 = new QPushButton(grh4);
  buttonh4->setText(tr("Holen","Button Holen"));
  QObject::connect( buttonh4, SIGNAL( clicked()), this, SLOT(clickbuttonh4()));
  buttonh4->setStatusTip(tr("Messkurve in den Hintergrund laden", "Statustip Messkurve in den Hintergrund laden"));
  buttonh4->setEnabled(false);
  buttonl4 = new QPushButton(grh4);
  buttonl4->setText(tr("Laden"));
  QObject::connect( buttonl4, SIGNAL( clicked()), this, SLOT(clickbuttonl4()));
  buttonl4->setStatusTip(tr("Messkurve aus Datei in den Hintergrund laden", "Statustip Messkurve aus Datei in den Hintergrund laden"));
  buttonc4 = new QPushButton(grh4);
  buttonc4->setText("Color");
  QObject::connect( buttonc4, SIGNAL( clicked()), this, SLOT(setColorH4()));
  buttonc4->setStatusTip(tr("Color der Hintergrundkurve setzen", "Statustip Color der Hintergrundkurve setzen"));
  buttoni4 = new QPushButton(grh4);
  buttoni4->setText(tr("i"));
  QObject::connect( buttoni4, SIGNAL( clicked()), this, SLOT(infoh4()));
  buttoni4->setStatusTip(tr("Beschreibung der Kurve lesen", "Statustip Beschreibung der Kurve lesen"));
  checkboxh4 = new QCheckBox(grh4);
  checkboxh4->setText(tr("Kurve 4","CheckBox Kurve 4"));
  checkboxh4->setEnabled(false);
  checkboxh4->setChecked(false);
  QObject::connect( checkboxh4, SIGNAL( toggled (bool)), this, SLOT(checkboxh4_checked(bool)));
  checkboxh4m = new QCheckBox(grh4);
  checkboxh4m->setText(tr("Frequenz-Marker sichtbar","CheckBox Frequenz-Marker sichtbar"));
  checkboxh4m->setEnabled(false);
  checkboxh4m->setChecked(false);
  QObject::connect( checkboxh4m, SIGNAL( toggled (bool)), this, SLOT(checkboxh4m_checked(bool)));

  grh5 = new QGroupBox(tr("Kurve 5","GroupBox Kurve 5"),widgetwkm);
  buttone5 = new QPushButton(grh5);
  buttone5->setText("...");
  QObject::connect( buttone5, SIGNAL( clicked()), this, SLOT(clickbuttone5()));
  buttone5->setStatusTip(tr("Messkurve Eigenschaften", "Statustip Messkurve Eigenschaften"));
  buttone5->setEnabled(false);
  buttonh5 = new QPushButton(grh5);
  buttonh5->setText(tr("Holen","Button Holen"));
  QObject::connect( buttonh5, SIGNAL( clicked()), this, SLOT(clickbuttonh5()));
  buttonh5->setStatusTip(tr("Messkurve in den Hintergrund laden", "Statustip Messkurve in den Hintergrund laden"));
  buttonh5->setEnabled(false);
  buttonl5 = new QPushButton(grh5);
  buttonl5->setText(tr("Laden"));
  QObject::connect( buttonl5, SIGNAL( clicked()), this, SLOT(clickbuttonl5()));
  buttonl5->setStatusTip(tr("Messkurve aus Datei in den Hintergrund laden", "Statustip Messkurve aus Datei in den Hintergrund laden"));
  buttonc5 = new QPushButton(grh5);
  buttonc5->setText("Color");
  QObject::connect( buttonc5, SIGNAL( clicked()), this, SLOT(setColorH5()));
  buttonc5->setStatusTip(tr("Color der Hintergrundkurve setzen", "Statustip Color der Hintergrundkurve setzen"));
  buttoni5 = new QPushButton(grh5);
  buttoni5->setText(tr("i"));
  QObject::connect( buttoni5, SIGNAL( clicked()), this, SLOT(infoh5()));
  buttoni5->setStatusTip(tr("Beschreibung der Kurve lesen", "Statustip Beschreibung der Kurve lesen"));
  checkboxh5 = new QCheckBox(grh5);
  checkboxh5->setText(tr("Kurve 5","CheckBox Kurve 5"));
  checkboxh5->setEnabled(false);
  checkboxh5->setChecked(false);
  QObject::connect( checkboxh5, SIGNAL( toggled (bool)), this, SLOT(checkboxh5_checked(bool)));
  checkboxh5m = new QCheckBox(grh5);
  checkboxh5m->setText(tr("Frequenz-Marker sichtbar","CheckBox Frequenz-Marker sichtbar"));
  checkboxh5m->setEnabled(false);
  checkboxh5m->setChecked(false);
  QObject::connect( checkboxh5m, SIGNAL( toggled (bool)), this, SLOT(checkboxh5m_checked(bool)));

  //########################################################################################
  //mW-Meter
  //########################################################################################
  widgetmwatt = new QWidget();
  widgetmwatt->setStatusTip(tr("mW-Meter und VFO", "Statustip mW-Meter und VFO"));
  tabwidget->addTab(widgetmwatt, "VFO/mW");
  whline = new QFrame(widgetmwatt);
  whline->setFrameStyle(QFrame::Panel | QFrame::Raised);
  whline->setBackgroundRole(QPalette::Dark);
  whline->setAutoFillBackground(true);

  grmesskanal = new QGroupBox(tr("mW-Meter","GroupBox mW-Meter"),widgetmwatt);
  grmesskanal->setStatusTip(tr("mW-Meter mit verschiedenen Messkoepfen", "Statustip mW-Meter mit verschiedenen Messkoepfen"));
  grmesskanal->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  progressbarkanal = new QProgressBar(grmesskanal);
  progressbarkanal->setTextVisible(false);
  progressbarkanal->setMinimum(-850);
  progressbarkanal->setMaximum(50);
  spbarminkanal = new QSpinBox(grmesskanal);
  spbarminkanal->setSuffix(" dBm");
  spbarminkanal->setMinimum(-100);
  spbarminkanal->setMaximum(0);
  spbarminkanal->setValue(-85);
  spbarminkanal->setSingleStep(5);
  spbarmaxkanal = new QSpinBox(grmesskanal);
  spbarmaxkanal->setSuffix(" dBm");
  spbarmaxkanal->setMinimum(-80);
  spbarmaxkanal->setMaximum(10);
  spbarmaxkanal->setValue(5);
  spbarmaxkanal->setSingleStep(5);
  QObject::connect( spbarminkanal, SIGNAL( valueChanged(int)), this, SLOT(setbarkanal()));
  QObject::connect( spbarmaxkanal, SIGNAL( valueChanged(int)), this, SLOT(setbarkanal()));
  labeldbmkanal = new QLabel("",grmesskanal);
  labeldbmkanal->setAlignment(Qt::AlignCenter);
  labelvoltkanal = new QLabel("",grmesskanal);
  labelvoltkanal->setAlignment(Qt::AlignCenter);
  labelwattkanal = new QLabel("",grmesskanal);
  labelwattkanal->setAlignment(Qt::AlignCenter);
  labeladckanal = new QLabel("",grmesskanal);
  labeladckanal->setAlignment(Qt::AlignRight);
  sphangkanal = new QSpinBox(grmesskanal);
  sphangkanal->setSuffix(" mSek");
  sphangkanal->setMinimum(0);
  sphangkanal->setMaximum(4000);
  sphangkanal->setValue(300);
  sphangkanal->setSingleStep(100);
  QObject::connect( sphangkanal, SIGNAL( valueChanged(int)), this, SLOT(setsphang(int)));
  labelhangkanal = new QLabel(tr("Hang-Werte","label"),grmesskanal);
  labelhangkanal->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  checkkanal = new QCheckBox(grmesskanal);
  checkkanal->setText(tr("Aktiv","CheckBox Aktiv"));
  QObject::connect( checkkanal, SIGNAL( toggled (bool)), this, SLOT(checkmwkanal_checked(bool)));
  spattextkanal = new QSpinBox(grmesskanal);
  spattextkanal->setSuffix(" dB");
  spattextkanal->setMinimum(0);
  spattextkanal->setMaximum(60);
  spattextkanal->setValue(0);
  spattextkanal->setSingleStep(10);
  QObject::connect( spattextkanal, SIGNAL( valueChanged(int)), this, SLOT(setspattext(int)));
  labelattextkanal = new QLabel("Attenuator extern",grmesskanal);
  labelattextkanal->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  spdbmdigitkanal = new QSpinBox(grmesskanal);
  spdbmdigitkanal->setMinimum(0);
  spdbmdigitkanal->setMaximum(3);
  spdbmdigitkanal->setSingleStep(1);
  QObject::connect( spdbmdigitkanal, SIGNAL( valueChanged(int)), this, SLOT(setdbmdigit(int)));
  labeldbmdigitkanal = new QLabel("Precision",grmesskanal);

  grmessvfo = new QGroupBox("NWT-VFO",widgetmwatt);
  grmessvfo->setAlignment(Qt::AlignHCenter);
  sp1hz = new QSpinBox(grmessvfo);
  sp1hz->setRange(-1, 10);
  QObject::connect( sp1hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp10hz = new QSpinBox(grmessvfo);
  sp10hz->setRange(-1, 10);
  QObject::connect( sp10hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp100hz = new QSpinBox(grmessvfo);
  sp100hz->setRange(-1, 10);
  QObject::connect( sp100hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp1khz = new QSpinBox(grmessvfo);
  sp1khz->setRange(-1, 10);
  QObject::connect( sp1khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp10khz = new QSpinBox(grmessvfo);
  sp10khz->setRange(-1, 10);
  QObject::connect( sp10khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp100khz = new QSpinBox(grmessvfo);
  sp100khz->setRange(-1, 10);
  QObject::connect( sp100khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp1mhz = new QSpinBox(grmessvfo);
  sp1mhz->setRange(-1, 10);
  QObject::connect( sp1mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp10mhz = new QSpinBox(grmessvfo);
  sp10mhz->setRange(-1, 10);
  QObject::connect( sp10mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp100mhz = new QSpinBox(grmessvfo);
  sp100mhz->setRange(-1, 10);
  QObject::connect( sp100mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  sp1ghz = new QSpinBox(grmessvfo);
  sp1ghz->setRange(-1, 10);
  QObject::connect( sp1ghz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  labelsphz = new QLabel("Hz", grmessvfo);
  labelspkhz = new QLabel("kHz", grmessvfo);
  labelspmhz = new QLabel("MHz", grmessvfo);
  bvfo2sp = new QPushButton(QIcon(":/images/right.png"), tr("VFO to Speicher"), grmessvfo);
  bvfo2sp->setText(tr("VFO to Speicher"));
  QObject::connect( bvfo2sp, SIGNAL( clicked()), this, SLOT(clickbvfo2sp()));
  bsp2vfo = new QPushButton(QIcon(":/images/links.png"), tr("Speicher to VFO"), grmessvfo);
  bsp2vfo->setText(tr("Speicher to VFO"));
  QObject::connect( bsp2vfo, SIGNAL( clicked()), this, SLOT(clickbsp2vfo()));

  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  grlcda = new QGroupBox(tr("VFO-Speicher [MHz]"),widgetmwatt);
  grlcda->setAlignment(Qt::AlignHCenter);

  //LCD Anzeige 10 stellen mit punkt
  LCD1 = new QLCDNumber(10, grlcda);
  //Hintergrung gelb
  LCD1->setPalette(dp);
  LCD1->setAutoFillBackground(true);
  LCD1->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD1->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD1->setSmallDecimalPoint(true);
  rb1 = new QRadioButton(grlcda);
  rb1->setChecked(true);
  QObject::connect( rb1, SIGNAL(clicked()), this, SLOT(lcd1clicked() ));
  lcdspeicher=1;

  LCD2 = new QLCDNumber(10, grlcda);
  //Hintergrung dunkelgelb
  LCD2->setPalette(dpd);
  LCD2->setAutoFillBackground(true);
  LCD2->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD2->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD2->setSmallDecimalPoint(true);
  rb2 = new QRadioButton(grlcda);
  rb2->setChecked(false);
  QObject::connect( rb2, SIGNAL(clicked()), this, SLOT(lcd2clicked() ));

  LCD3 = new QLCDNumber(10, grlcda);
  //Hintergrung dunkelgelb
  LCD3->setPalette(dpd);
  LCD3->setAutoFillBackground(true);
  LCD3->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD3->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD3->setSmallDecimalPoint(true);
  rb3 = new QRadioButton(grlcda);
  rb3->setChecked(false);
  QObject::connect( rb3, SIGNAL(clicked()), this, SLOT(lcd3clicked() ));

  LCD4 = new QLCDNumber(10, grlcda);
  //Hintergrung dunkelgelb
  LCD4->setPalette(dpd);
  LCD4->setAutoFillBackground(true);
  LCD4->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD4->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD4->setSmallDecimalPoint(true);
  rb4 = new QRadioButton(grlcda);
  rb4->setChecked(false);
  QObject::connect( rb4, SIGNAL(clicked()), this, SLOT(lcd4clicked() ));

  LCD5 = new QLCDNumber(10, grlcda);
  //Hintergrung dunkelgelb
  LCD5->setPalette(dpd);
  LCD5->setAutoFillBackground(true);
  LCD5->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD5->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD5->setSmallDecimalPoint(true);
  rb5 = new QRadioButton(grlcda);
  rb5->setChecked(false);
  QObject::connect( rb5, SIGNAL(clicked()), this, SLOT(lcd5clicked() ));

  //########################################################################################
  //////////////////////////////////////////////////////////////////////////////////////////
  wgberechnung1 = new QWidget();
  wgberechnung1->setStatusTip(tr("Berechnungen Spulen..., Tabellen...", "Statustip Berechnungen Spulen..., Tabellen..."));
  tabwidget->addTab(wgberechnung1, tr("Berechnungen","TabWindow Berechnungen"));

  groupschwingkreis = new QGroupBox(tr("Schwingkreisberechnung/AL-Wert","GroupBox in Berechnungen"),wgberechnung1);
  editf1 = new QLineEdit(groupschwingkreis);
  QObject::connect( editf1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisf()));
  labelf1 = new QLabel(tr("MHz Frequenz","Label Berechnungen MHz Frequenz"), groupschwingkreis);
  editl1 = new QLineEdit(groupschwingkreis);
  QObject::connect( editl1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisl()));
  labell1 = new QLabel(tr("uH Induktivitaet","Label Berechnungen uH Induktivitaet"), groupschwingkreis);
  editc1 = new QLineEdit(groupschwingkreis);
  QObject::connect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisc()));
  labelc1 = new QLabel(tr("pF Kapazitaet","Label Berechnungen pF Kapazitaet"), groupschwingkreis);
  ergebnisxc = new QLabel("", groupschwingkreis);
  labelxc = new QLabel("Ohm XC", groupschwingkreis);
  ergebnisxl = new QLabel("", groupschwingkreis);
  labelxl = new QLabel("Ohm XL", groupschwingkreis);

  editn = new QLineEdit(groupschwingkreis);
  QObject::connect( editn, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisal()));
  labeln = new QLabel(tr("Windungen","in Berechnungen"), groupschwingkreis);
  edital = new QLineEdit(groupschwingkreis);
  edital->setReadOnly(true);
  labelal = new QLabel(tr("nH/N^2 AL-Wert","label"), groupschwingkreis);


  groupwindungen = new QGroupBox(tr("Windungen berechnen aus AL-Wert","in Berechnungen"),wgberechnung1);
  edital3 = new QLineEdit(groupwindungen);
  QObject::connect( edital3, SIGNAL(textChanged(QString)), this, SLOT(alwindungen()));
  labeledital3 = new QLabel(tr("nH/N^2 AL-Wert","in Berechnungen"), groupwindungen);
  editl3 = new QLineEdit(groupwindungen);
  QObject::connect( editl3, SIGNAL(textChanged(QString)), this, SLOT(alwindungen()));
  labeleditl3 = new QLabel(tr("uH Induktivitaet","in Berechnungen"), groupwindungen);
  ergebnisw = new QLabel("", groupwindungen);
  labelergebnisw = new QLabel(tr("N Windungen","in Berechnungen"), groupwindungen);

  grdbmvw = new QGroupBox(tr("dBm ---> Volt ---> Watt","in Berechnungen"),wgberechnung1);
  grdbmvw->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  spdbm = new QDoubleSpinBox(grdbmvw);
  spdbm->setRange(-140.0,60.0);
  spdbm->setValue(1.0);
  spdbm->setDecimals(1);
  spdbm->setSingleStep(1.0);
  spdbm->setSuffix(" dBm");
  QObject::connect( spdbm, SIGNAL( valueChanged(double)), this, SLOT(setvw(double)));
  cbdbm01 = new QCheckBox(tr("x0.1 (Schritt)","Checkbox"),grdbmvw);
  cbdbm01->setChecked(false);
  QObject::connect(cbdbm01, SIGNAL( toggled (bool)), this, SLOT(cbdbm01_checked(bool)));
  edvolt = new QLabel("Volt",grdbmvw);
  edvolt->setAlignment(Qt::AlignRight);
  edwatt = new QLabel("Watt",grdbmvw);
  edwatt->setAlignment(Qt::AlignRight);

  grrloss = new QGroupBox(tr("Return-Loss --> Ref.fak. --> SWV --> Impedanz", "Berechnungen"),wgberechnung1);
  grrloss->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  sprloss = new QDoubleSpinBox(grrloss);
  sprloss->setRange(-80,-0.1);
  sprloss->setValue(-10.0);
  sprloss->setDecimals(1);
  sprloss->setSingleStep(1.0);
  sprloss->setSuffix(" dB");
  QObject::connect( sprloss, SIGNAL( valueChanged(double)), this, SLOT(setrz(double)));
  cbrloss01 = new QCheckBox(tr("x0.1 (Schritt)","CheckBox"),grrloss);
  cbrloss01->setChecked(false);
  QObject::connect(cbrloss01, SIGNAL( toggled (bool)), this, SLOT(cbrloss01_checked(bool)));
  labelswv = new QLabel(tr("SWV:","Label"),grrloss);
  labelswv->setAlignment(Qt::AlignRight);
  labelrfaktor = new QLabel("R:",grrloss);
  labelrfaktor->setAlignment(Qt::AlignRight);
  labelzlow = new QLabel("Z1:",grrloss);
  labelzlow->setAlignment(Qt::AlignRight);
  labelzhigh = new QLabel("Z2:",grrloss);
  labelzhigh->setAlignment(Qt::AlignRight);

  // Berechnungen Widget
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // Impedanz Widget
  wgberechnung2 = new QWidget();
  wgberechnung2->setStatusTip(tr("Berechnungen Spulen...", "Statustip"));
  tabwidget->addTab(wgberechnung2, tr("Impedanzanpassung","TabWindow"));

  gimp = new QGroupBox(tr("Anpassung mit:","in Impedanz"),wgberechnung2);
  rbr = new QRadioButton(tr("R","RadioButton"),gimp);
  QObject::connect( rbr, SIGNAL(clicked()), this, SLOT(setimp() ));
  rbr->setChecked(true);
  rblc = new QRadioButton(tr("L/C","RadioButton"),gimp);
  QObject::connect( rblc, SIGNAL(clicked()), this, SLOT(setimp() ));


  gzr = new QGroupBox(tr("Anpassung mit R","in Impedanz"),wgberechnung2);
  editz1 = new QLineEdit(gzr);
  QObject::connect( editz1, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz1 = new QLabel(tr("(Ohm) Z1","in Impedanz"), gzr);
  editz2 = new QLineEdit(gzr);
  QObject::connect( editz2, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz2 = new QLabel(tr("(Ohm) Z2","in Impedanz"), gzr);
  editz3 = new QLineEdit(gzr);
  QObject::connect( editz3, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz3 = new QLabel(tr("(Ohm) Z3","in Impedanz"), gzr);
  editz4 = new QLineEdit(gzr);
  QObject::connect( editz4, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz4 = new QLabel(tr("(Ohm) Z4","in Impedanz"), gzr);
  lr1 = new QLabel(tr("xxx","in Impedanz"), gzr);
  lr1->setAlignment(Qt::AlignRight);
  lr2 = new QLabel(tr("xxx","in Impedanz"), gzr);
  lr2->setAlignment(Qt::AlignRight);
  lr3 = new QLabel(tr("xxx","in Impedanz"), gzr);
  lr3->setAlignment(Qt::AlignRight);
  lr4 = new QLabel(tr("xxx","in Impedanz"), gzr);
  lr4->setAlignment(Qt::AlignRight);
  ldaempfung = new QLabel(tr("xxx","in Impedanz"), gzr);
  ldaempfung->setAlignment(Qt::AlignRight);
  lbeschrr1 = new QLabel(tr("(Ohm) R1","in Impedanz"), gzr);
  lbeschrr2 = new QLabel(tr("(Ohm) R2","in Impedanz"), gzr);
  lbeschrr3 = new QLabel(tr("(Ohm) R3","in Impedanz"), gzr);
  lbeschrr4 = new QLabel(tr("(Ohm) R4","in Impedanz"), gzr);
  lbeschrdaempfung = new QLabel(tr("(dB) Einfuegedaempfung","in Impedanz"), gzr);
  bild1 = new QLabel(wgberechnung2);
  bild1->setPixmap(QPixmap::fromImage(QImage(":/images/zanp1.png")));

  gzlc = new QGroupBox(tr("Anpassung mit LC","in Impedanz"),wgberechnung2);
  gzlc->hide();
  editzlc1 = new QLineEdit(gzlc);
  QObject::connect( editzlc1, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc1 = new QLabel(tr("(Ohm) Z1","in Impedanz"), gzlc);
  editzlc2 = new QLineEdit(gzlc);
  QObject::connect( editzlc2, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc2 = new QLabel(tr("(Ohm) Z2","in Impedanz"), gzlc);
  editzlc3 = new QLineEdit(gzlc);
  QObject::connect( editzlc3, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc3 = new QLabel(tr("(MHz) Frequenz","in Impedanz"), gzlc);
  ll = new QLabel(tr("xxx","in Impedanz"), gzlc);
  ll->setAlignment(Qt::AlignRight);
  lc = new QLabel(tr("xxx","in Impedanz"), gzlc);
  lc->setAlignment(Qt::AlignRight);
  lbeschrl = new QLabel(tr("(uH) L","in Impedanz"), gzlc);
  lbeschrc = new QLabel(tr("(pF) C","in Impedanz"), gzlc);
  bild2 = new QLabel(wgberechnung2);
  bild2->setPixmap(QPixmap::fromImage(QImage(":/images/zanp2.png")));
  bild2->hide();
  // Impedanz Widget Ende
  ///////////////////////////////////////////////////////////////////////////////

  //########################################################################################
  //Einstellen max und min Groesse des Gesamtwindows
  setMaximumSize (2000, 2000);
  setMinimumSize (800, 530);

  createActions();
  createMenus();
  createStatusBar();
  createToolBars();

  messkurveClr();
  messkurve.bspur = false;

  repainttimer = new QTimer(this);
  QObject::connect(repainttimer, SIGNAL(timeout()), this, SLOT(actrepainttimer() ));
  openttytimer = new QTimer(this);
  QObject::connect(openttytimer, SIGNAL(timeout()), this, SLOT(fopentty() ));
  befantworttimer = new QTimer(this);
  //Signal vom befantworttimer (Auslesen der RS232)
  QObject::connect(befantworttimer, SIGNAL(timeout()), this, SLOT(befantworttimerstop() ));
  delaytimer = new QTimer(this);
  QObject::connect(delaytimer, SIGNAL(timeout()), this, SLOT(delaytimerstop() ));
  wobwiederholtimer = new QTimer(this);
  QObject::connect(wobwiederholtimer, SIGNAL(timeout()), this, SLOT(wobwiederholtimerstop() ));
  ddstakttimer = new QTimer(this);
  QObject::connect(ddstakttimer, SIGNAL(timeout()), this, SLOT(ddstakttimerstop() ));
  svfotimer = new QTimer(this);
  QObject::connect(svfotimer, SIGNAL(timeout()), this, SLOT(svfotimerstop() ));
  messvfotimer = new QTimer(this);
  QObject::connect(messvfotimer, SIGNAL(timeout()), this, SLOT(messvfotimerstop() ));
  mhangtimer = new QTimer(this);
  mhangtimer->setSingleShot(true);
  QObject::connect(mhangtimer, SIGNAL(timeout()), this, SLOT(mhangk1timerstop() ));
  wachtimer = new QTimer(this);
  QObject::connect(wachtimer, SIGNAL(timeout()), this, SLOT(wachtimerstop() ));
  rs232stoptimer = new QTimer(this);
  QObject::connect(rs232stoptimer, SIGNAL(timeout()), this, SLOT(fRS232Stop()));

#ifdef Q_OS_WIN
  int i;
  QString qs;
  for(i=1;i<=60;i++){
    qs.sprintf("COM%i",i);
    listtty.append(qs);
  }
#else
  listtty.append(schnittstelle1);// /dev/ttyS0 Test mit NWT01
  listtty.append(schnittstelle5);// USB Schnittstellen
  listtty.append(schnittstelle6);
  listtty.append(schnittstelle7);
  listtty.append(schnittstelle8);
#endif
  idtty = 0; //mit Index 0 HW-Suche beginnen
  repainttimer->start(10);//Windows in richtiger groesse darstellen

  //Funktion fuer Umschalten der Windows installieren
  QObject::connect( tabwidget, SIGNAL(currentChanged ( int )), this, SLOT(tabumschalten( int )));

  readSettings();
  grafiksetzen();

  if(bd)qDebug("Nwt4Window::Nwt4Window ENDE");
}

void Nwt4Window::setFonts(){
  if(bd)qDebug("Nwt4Window::setFonts()");

  fontapp = this->font();
  fontapp.setPixelSize(grunddaten.fontsize);
  fontapp.setBold(false);
  this->setFont(fontapp);
  menuDatei->setFont(fontapp);
  menuEinstellung->setFont(fontapp);
  menuKurven->setFont(fontapp);
  menuMesskopf->setFont(fontapp);
  menuHilfe->setFont(fontapp);

  QFont fontappbold = fontapp;
  fontappbold.setBold(true);

  labelkanal->setFont(fontappbold);

  QFont fontmw = fontapp;
  fontmw.setBold(false);
  fontmw.setPixelSize(grunddaten.mwfontsize);
  QFont fontmwbold = fontapp;
  fontmwbold.setBold(true);
  fontmwbold.setPixelSize(grunddaten.mwfontsize);

  QFont fontvfo = fontapp;
  fontvfo.setBold(false);
  fontvfo.setPixelSize(grunddaten.vfofontsize);
  QFont fontvfobold = fontapp;
  fontvfobold.setPixelSize( grunddaten.vfofontsize);
  fontvfobold.setBold(true);

  QFont fontber = fontapp;
  fontber.setBold(false);
  fontber.setPixelSize(grunddaten.berfontsize);
  QFont fontgrber = fontapp;
  fontgrber.setBold(true);
  fontgrber.setPixelSize(grunddaten.grberfontsize);

  grmesskanal->setFont(fontmwbold);
  spbarminkanal->setFont(fontapp);
  spbarmaxkanal->setFont(fontapp);
  labeldbmkanal->setFont(fontmw);
  labelvoltkanal->setFont(fontmw);
  labelwattkanal->setFont(fontmw);
  labeladckanal->setFont(fontmw);
  sphangkanal->setFont(fontapp);
  labelhangkanal->setFont(fontapp);
  checkkanal->setFont(fontapp);
  spattextkanal->setFont(fontapp);
  labelattextkanal->setFont(fontapp);
  spdbmdigitkanal->setFont(fontapp);
  labeldbmdigitkanal->setFont(fontapp);

  grmessvfo->setFont(fontvfobold);
  grlcda->setFont(fontvfobold);
  sp1hz->setFont(fontvfo);
  sp10hz->setFont(fontvfo);
  sp100hz->setFont(fontvfo);
  sp1khz->setFont(fontvfo);
  sp10khz->setFont(fontvfo);
  sp100khz->setFont(fontvfo);
  sp1mhz->setFont(fontvfo);
  sp10mhz->setFont(fontvfo);
  sp100mhz->setFont(fontvfo);
  sp1ghz->setFont(fontvfo);
  labelsphz->setFont(fontvfobold);
  labelspkhz->setFont(fontvfobold);
  labelspmhz->setFont(fontvfobold);
  bvfo2sp->setFont(fontapp);
  bsp2vfo->setFont(fontapp);

  groupschwingkreis->setFont(fontappbold);
  editf1->setFont(fontapp);
  labelf1->setFont(fontapp);
  editl1->setFont(fontapp);
  labell1->setFont(fontapp);
  editc1->setFont(fontapp);
  labelc1->setFont(fontapp);
  ergebnisxc->setFont(fontapp);
  labelxc->setFont(fontapp);
  ergebnisxl->setFont(fontapp);
  labelxl->setFont(fontapp);
  editn->setFont(fontapp);
  labeln->setFont(fontapp);
  edital->setFont(fontapp);
  labelal->setFont(fontapp);

  groupwindungen->setFont(fontappbold);
  edital3->setFont(fontapp);
  labeledital3->setFont(fontapp);
  editl3->setFont(fontapp);
  labeleditl3->setFont(fontapp);
  ergebnisw->setFont(fontapp);
  labelergebnisw->setFont(fontapp);

  grdbmvw->setFont(fontgrber);
  spdbm->setFont(fontber);
  cbdbm01->setFont(fontapp);
  edvolt->setFont(fontber);
  edwatt->setFont(fontber);

  grrloss->setFont(fontgrber);
  sprloss->setFont(fontber);
  cbrloss01->setFont(fontapp);
  labelswv->setFont(fontber);
  labelrfaktor->setFont(fontber);
  labelzlow->setFont(fontber);
  labelzhigh->setFont(fontber);
  if(bd)qDebug("Nwt4Window::setFonts() ENDE");
}

void Nwt4Window::ProgramInit(){
  if(bd)qDebug("Nwt4Window::ProgramInit()");
  WobbelGrundDatenToEdit();
  WobbelGrundDatenToDisplay();
  //HW ist noch nicht bekannt
  messkopf.init();//Messkopf init
  if(grunddaten.filemk == "Messkopf"){
    messkopf.setCalFrqmin(wobbelgrunddaten.calfrqmin);
    messkopf.setCalFrqmax(wobbelgrunddaten.calfrqmax);
  }

  messkurve.calablauf=0;//keine Kalibrierung Ablauf=0

  bkurvegeladen=false;//keine Kurve von Datei geladen

  bbefwobbeln=false;//Befehl Wobbeln AUS
  bnwtgefunden=false;//HW noch nicht gefunden

  display->setFrqMarken(frqmarken);//Frequenzmarken im Display setzen

  checkboxkanal->setChecked(messkurve.bkanal);//Kanal1 EIN/AUS
  bmessenmw=false;//mW-Meter aus
  hangtk1=300;//Wattmeter Hangzeit in mSek

  offsetk1=0.0;//dB Offset Kanal1
  bdbmk1=true;//mW-Meter Kanal1 EIN
  spdbm->setValue(0.0);//VFO Pegel auf 0dBm
  sprloss->setValue(-20.0);//in Berechnung Return Loss default auf -20dB
  grunddaten.bset0hz=true;
  QFont f = font();
  f.setPixelSize(grunddaten.fontsize);
  if(grunddaten.bfontsize)setFont(f);
  if(grunddaten.bfontsize)setFonts();
  bsetup=false;
  schwingkreis=kein;
  kabeldaten.ca = 0.0;
  kabeldaten.dk = 0.0;
  kabeldaten.laenge = 0.0;
  kabeldaten.lfrequenz = 0.0;
  kabeldaten.vf = 0.0;
  kabeldaten.z = 0.0;
  mKabeldaten->setEnabled(false);
  mKurvenAnalyse->setEnabled(false);
  if(bd)qDebug("Nwt4Window::ProgramInit() ENDE");
}

Nwt4Window::~Nwt4Window()
{
}

void Nwt4Window::createMenus()
{
  if(bd)qDebug("Nwt4Window::createMenus");
  menuDatei = menuBar()->addMenu(tr("&Datei", "Menu"));
  menuDatei->addAction(mSpeichernpng);
  menuDatei->addAction(mBeenden);

  menuEinstellung = menuBar()->addMenu(tr("&Einstellung", "Menu"));
  menuEinstellung->addAction(mSetup);
  menuEinstellung->addAction(mRS232Stop);

  menuKurven = menuBar()->addMenu(tr("&Kurven", "Menu"));
  menuKurven->addAction(mKurvenLaden);
  menuKurven->addAction(mKurvenSichern);
  menuKurven->addAction(mKurveCaption);
  menuKurven->addSeparator();
  menuKurven->addAction(mKurvenSpur);
  menuKurven->addAction(mKurvenAnalyse);
  menuKurven->addSeparator();
  menuKurven->addAction(mFrqMarken);
  menuKurven->addSeparator();
  menuKurven->addAction(mKabeldaten);

  menuMesskopf = menuBar()->addMenu(tr("&Messkopf", "Menu"));
  menuMesskopf->addAction(mNeuK1);
  menuMesskopf->addAction(mCalibK1);
  menuMesskopf->addSeparator();
  menuMesskopf->addAction(mSondeLadenK1);
  menuMesskopf->addAction(mSondeSpeichernK1);
  menuMesskopf->addAction(mSondeClr);
  menuMesskopf->addAction(mMkInfoK1);
  menuMesskopf->addAction(mNoFrqCalK1);
  menuMesskopf->addSeparator();
  menuMesskopf->addAction(mCaldBm);
  menuMesskopf->addAction(mCalmWm);
  menuMesskopf->addSeparator();
  menuMesskopf->addAction(mCalibFlat);

  menuHilfe = menuBar()->addMenu(tr("&Hilfe", "Menu"));
  menuHilfe->addAction(mTooltip);
  menuHilfe->addSeparator();
  menuHilfe->addAction(mInfo);
  menuHilfe->addAction(mFwVersion);
  if(bd)qDebug("Nwt4Window::createMenus ENDE");

}

void Nwt4Window::createToolBars()
{
  if(bd)qDebug("Nwt4Window::createToolBars");
  //Datei
  fileToolBar = addToolBar(tr("Datei"));
  fileToolBar->addAction(mBeenden);
  fileToolBar->addAction(mBeenden);
  fileToolBar->addAction(mSpeichernpng);
  //Einstellungen
  einstellungToolBar = addToolBar(tr("Einstellungen"));
  einstellungToolBar->addAction(mSetup);
  einstellungToolBar->addAction(mRS232Stop);
  einstellungToolBar->addAction(mInfo);
  //Kurven
  kurvenToolBar = addToolBar("ToolBarsKurven");
  kurvenToolBar->addAction(mKurvenLaden);
  kurvenToolBar->addAction(mKurvenSichern);
  kurvenToolBar->addAction(mKurveCaption);
  kurvenToolBar->addAction(mFrqMarken);
  //Sonden
  sondenLadenToolBar = addToolBar("ToolBarsSondenLaden");
  sondenLadenToolBar->addAction(mSondeLadenK1);
  sondenKalibToolBar = addToolBar("ToolBarsSondenKalibrieren");
  sondenKalibToolBar->addAction(mCalibK1);
  if(bd)qDebug("Nwt4Window::createToolBars ENDE");
}

void Nwt4Window::createActions()
{
  if(bd)qDebug("Nwt4Window::createActions");
  mSpeichernpng = new QAction(QIcon(":/images/savebild.png"), tr("Speichern als Bild","Menu"), this);
  mSpeichernpng->setStatusTip(tr("Speichern des Wobbelfensters als Bild-Datei", "Statustip"));
  connect(mSpeichernpng, SIGNAL(triggered()), this, SLOT(bildspeichern()));

  mBeenden = new QAction(QIcon(":/images/beenden.png"), tr("&Beenden","Menu"), this);
  mBeenden->setShortcut(Qt::CTRL + Qt::Key_Q);
  mBeenden->setStatusTip(tr("Beenden des Programmes und Speichern der Konfiguration", "Statustip"));
  connect(mBeenden, SIGNAL(triggered()), this, SLOT(beenden()));

  mKurvenLaden = new QAction(QIcon(":/images/open.png"), tr("Wobbelkurve &laden","Menu"), this);
  mKurvenLaden->setStatusTip(tr("Laden einer abgespeicherten Wobbeldatei", "Statustip"));
  connect(mKurvenLaden, SIGNAL(triggered()), this, SLOT(KurvenLaden()));

  mKurvenSichern = new QAction(QIcon(":/images/save.png"),tr("Wobbelkurve &sichern","Menu"), this);
  mKurvenSichern->setStatusTip(tr("Abspeichern des Wobbelergebnisses in eine Datei", "Statustip"));
  connect(mKurvenSichern, SIGNAL(triggered()), this, SLOT(SaveKurve()));

  mKurvenSpur = new QAction(tr("Wobbelspur EIN/AUS","Menu"), this);
  mKurvenSpur->setStatusTip(tr("Wobbelkurven Spur max. 20 Kurven", "Statustip"));
  mKurvenSpur->setCheckable(true);
  connect(mKurvenSpur, SIGNAL(triggered(bool)), this, SLOT(setKurvenSpur(bool)));

  mKurvenAnalyse = new QAction(tr("Wobbelspur Analyse","Menu"), this);
  mKurvenAnalyse->setStatusTip(tr("Wobbelkurven Spuren Auswerten", "Statustip"));
  connect(mKurvenAnalyse, SIGNAL(triggered()), this, SLOT(fKurvenAnalyse()));

  mNeuK1 = new QAction(tr("Neue Messkopfdaten generieren","Menu"), this);
  mNeuK1->setStatusTip(tr("Neuer Messkopf (neue Messkopfdaten erzeugen, kalibrieren und abspeichern)", "Statustip"));
  connect(mNeuK1, SIGNAL(triggered()), this, SLOT(sondeNeu()));

  mCalibFlat = new QAction(tr("HW Flatnes Kalibrierung","Menu"), this);
  mCalibFlat->setStatusTip(tr("HW Flatnes Kalibrierung direkt im NWT", "Statustip"));
  connect(mCalibFlat, SIGNAL(triggered()), this, SLOT(setFlatCal()));

  mCalibK1 = new QAction(QIcon(":/images/calk1.png"),tr("Messkopfdaten Nachkalibrieren","Menu"), this);
  mCalibK1->setStatusTip(tr("Nachkalibrieren (Genauigkeit der Messkopf nachkalibireren)", "Statustip"));
  connect(mCalibK1, SIGNAL(triggered()), this, SLOT(nachkalib()));

  mSondeLadenK1 = new QAction(QIcon(":/images/openk1.png"), tr("Messkopfdaten laden","Menu"), this);
  mSondeLadenK1->setStatusTip(tr("Messkopf-Datei laden", "Statustip"));
  connect(mSondeLadenK1, SIGNAL(triggered()), this, SLOT(ladenMesskopf1()));

  mSondeSpeichernK1 = new QAction(tr("Messkopfdaten speichern","Menu"), this);
  mSondeSpeichernK1->setStatusTip(tr("Messkopfdaten in eine Datei speichern", "Statustip"));
  connect(mSondeSpeichernK1, SIGNAL(triggered()), this, SLOT(speichernMesskopf()));

  mSondeClr = new QAction(tr("Messkopfdaten loeschen","Menu"), this);
  mSondeClr->setStatusTip(tr("Messkopf-Datei loeschen", "Statustip"));
  connect(mSondeClr, SIGNAL(triggered()), this, SLOT(ClrMesskopf()));

  mInfo = new QAction(QIcon(":/images/info.png"), tr("&Info","Menu"), this);
  mInfo->setStatusTip(tr("Informationen zur Programmversion", "Statustip"));
  connect(mInfo, SIGNAL(triggered()), this, SLOT(Info()));

  mFwVersion = new QAction(tr("&Firmware Version","Menu"), this);
  mFwVersion->setStatusTip(tr("Informationen zur Firwareversion", "Statustip"));
  connect(mFwVersion, SIGNAL(triggered()), this, SLOT(fwversion()));

  mTooltip = new QAction(tr("Tip?","Menu"), this);
  mTooltip->setStatusTip(tr("Hilfe an der Maus ein/aus-blenden", "Statustip"));
  mTooltip->setCheckable(true);
  connect(mTooltip, SIGNAL(triggered(bool)), this, SLOT(tip(bool)));

  mFrqMarken = new QAction(QIcon(":/images/fmarken.png"), tr("Frequenzmarken","Menu"), this);
  mFrqMarken->setStatusTip(tr("Frequenzmarken im Wobbelfenster einblenden", "Statustip"));
  connect(mFrqMarken, SIGNAL(triggered()), this, SLOT(setFrequenzmarken()));

  mKurveCaption = new QAction(QIcon(":/images/label.png"), tr("Kurve Beschreibung","Menu"), this);
  mKurveCaption->setStatusTip(tr("Ein Beschreibung fuer die Kurve eingeben", "Statustip"));
  connect(mKurveCaption, SIGNAL(triggered()), this, SLOT(kurveCaption()));

  mSetup = new QAction(QIcon(":/images/option.png"), tr("&Setup","Menu"), this);
  mSetup->setStatusTip(tr("SETUP Einstellungen", "Statustip"));
  mSetup->setEnabled(false);
  connect(mSetup, SIGNAL(triggered()), this, SLOT(setSetup()));

  mKabeldaten = new QAction(tr("Info Kabeldaten","Menu"), this);
  mKabeldaten->setStatusTip(tr("Ansicht und Speichern der Kabeldaten in ein Textfile", "Statustip"));
  connect(mKabeldaten, SIGNAL(triggered()), this, SLOT(fKabeldaten()));

  mNoFrqCalK1 = new QAction(tr("Frequenzgang Korrektur AUS","Menu"), this);
  mNoFrqCalK1->setStatusTip(tr("Frequenzgangkorrektur abschalten", "Statustip"));
  mNoFrqCalK1->setCheckable(true);
  connect(mNoFrqCalK1, SIGNAL(triggered(bool)), this, SLOT(setNoFrqCalK1(bool)));

  mMkInfoK1 = new QAction(tr("&Information zum Messkopf","Menu"), this);
  mMkInfoK1->setStatusTip(tr("Informationen zum Messkopf", "Statustip"));
  connect(mMkInfoK1, SIGNAL(triggered()), this, SLOT(MesskopfInfoK1()));

  mCaldBm = new QAction(tr("dB-Korrektur Spektumanzeige","Menu"), this);
  mCaldBm->setStatusTip(tr("dB-Korrektur Spektumanzeige kalibrieren", "Statustip"));
  connect(mCaldBm, SIGNAL(triggered()), this, SLOT(fCaldBm()));

  mCalmWm = new QAction(tr("Kalibrieren mW-Meter","Menu"), this);
  mCalmWm->setStatusTip(tr("mW-Meter kalibrieren", "Statustip"));
  connect(mCalmWm, SIGNAL(triggered()), this, SLOT(fCalmWm()));
  mCalmWm->setEnabled(false);//Programmstart im Wobbelfenster

  mRS232Stop = new QAction(QIcon(":/images/usb_close.png"), tr("RS232 Suche Abbrechen!","Menu"), this);
  mRS232Stop->setStatusTip(tr("RS232 Suche Abbrechen!", "Statustip"));
  connect(mRS232Stop, SIGNAL(triggered()), this, SLOT(fRS232Stop()));


  if(bd)qDebug("Nwt4Window::createActions ENDE");
}

void Nwt4Window::beenden()
{
  if(bd)qDebug("Nwt4Window::beenden()");
  close();
  if(bd)qDebug("Nwt4Window::beenden() ENDE");
}

void Nwt4Window::fopentty(){
  int gelesen;

  if(bd)qDebug("Nwt4Window::fopentty");
  openttytimer->stop();
  if(bvantwort){
    bvantwort = false;
    gelesen = picmodem->readttybuffer(rxbuffer, 20);
    if(gelesen == 1){
      qDebug()<<"Antwort auf v:"<<gelesen<<", inhalt:"<< int(rxbuffer[0]);
      grunddaten.version = int(rxbuffer[0]);
      bvantwort = false;
      qDebug("FW-Version: %03i", grunddaten.version);
      if(grunddaten.version == 119){
        cfgladen();
        ProgramInit();
        bnwtgefunden = true;
        fRS232Stop();
        openttytimer->stop(); //Suchen der Schnittstelle bendet
        statusBar()->showMessage(tr("HW gefunden an: ") + srs232);
        setAText(srs232);
        buttoneinzeln->setEnabled(true);
        buttonwobbeln->setEnabled(true);
        mSetup->setEnabled(true);
        loadMk(grunddaten.filemk);//Messkopf laden
        //qDebug()<<"loadMk(grunddaten.filemk) "<<grunddaten.filemk;//Messkopf laden
        clrrxbuffer();
        delay(500);//etwas warten
        //Anforderung @k; (DDS Takt abfragen)
        //qs="@k;";
        //if(bnwtgefunden)picmodem->writeLine(qPrintable(qs));
        befantworttimer->start(200);
      }else{
        idtty++;
        if(idtty > (listtty.count()-1))idtty = 0;
        openttytimer->start(500); //Suchen der Schnittstelle
        picmodem->closetty();
        rs232isopen = false;
      }
    }else{
      idtty++;
      if(idtty > (listtty.count()-1))idtty = 0;
      openttytimer->start(200); //Suchen der Schnittstelle
      picmodem->closetty();
      rs232isopen = false;
    }
  }else{
    if(!rs232isopen){
      srs232 = listtty.at(idtty);
      statusBar()->showMessage(tr("Suche HW :","fopentty") + srs232);
      //qDebug(qPrintable(srs232));
      //qDebug("%i",idtty);
      rs232isopen = picmodem->opentty(srs232);
      if(rs232isopen){
        clrrxbuffer();
        // Anforderung v; (Firmware abfrage)
        if(rs232isopen){
          picmodem->writeChar(0x8F);
          picmodem->writeChar('v');
        }
        //qDebug("open");
        bvantwort = true;
        openttytimer->start(500); //Suchen der HW
      }else{
        idtty++;
        if(idtty > (listtty.count()-1))idtty = 0;
        openttytimer->start(100); //Suchen der Schnittstelle
        bvantwort = false;
        picmodem->closetty();
        rs232isopen=false;
      }
    }
  }
  if(bd)qDebug("Nwt4Window::fopentty ENDE");
}

void Nwt4Window::createStatusBar()
{
  if(bd)qDebug("Nwt4Window::createStatusBar()");
  statusBar()->showMessage(tr("Ready"));
  if(bd)qDebug("Nwt4Window::createStatusBar() ENDE");
}

void Nwt4Window::writeSettings()
{
  if(bd)qDebug("Nwt4Window::writeSettings");
  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  if(bnwtgefunden)settings.setValue("rs232", srs232);
  display->writeSettings();
  if(bd)qDebug("Nwt4Window::writeSettings ENDE");
}

void Nwt4Window::readSettings()
{
  if(bd)qDebug("Nwt4Window::readSettings");
  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(610, 510)).toSize();
  move(pos);
  resize(size);
#ifdef Q_OS_WIN
  srs232 = settings.value("rs232", QString("COM1")).toString();
#else
  srs232 = settings.value("rs232", QString("/dev/ttyS0")).toString();
#endif

  picmodem->closetty();
  if(!rs232isopen)rs232isopen = picmodem->opentty(srs232);
  if(rs232isopen){
    clrrxbuffer();
    if(rs232isopen){
      picmodem->writeChar(0x8F);
      picmodem->writeChar('v');
    }
    bvantwort = true;
  }else{
    //warmstart();
  }
  openttytimer->start(500); //des Geraetes
  rs232stoptimer->start(15000);//nach 15 Sek RS232 Suche beenden
  if(bd)qDebug("Nwt4Window::readSettings ENDE");
}

void Nwt4Window::setAText(QString stty){
  if(bd)qDebug("Nwt4Window::setAText");

  QString qs, qs1;
#ifdef Q_OS_WIN
  qs = tr("Netzwerktester-NWT4000-Windows ");
#else
  qs = tr("Netzwerktester-NWT4000-Linux ");
#endif
  QString fwversion = tr("HW nicht gefunden");
  if(grunddaten.version == 119){
    fwversion.sprintf("FW:%i.%02i",grunddaten.version/100, grunddaten.version-100);
    switch(grunddaten.variante){
    case vnwt4_1:qs1=":NWT4000-1";fwversion+=qs1;break;
    case vnwt4_2:qs1=":NWT4000-2";fwversion+=qs1;break;
    case vnwt_ltdz:qs1=":LTDZ 35-4400M";fwversion+=qs1;break;
    case vnwt6:qs1=":NWT6000";fwversion+=qs1;break;
    case vnwt_nn:qs1=":NWT-noname";fwversion+=qs1;break;
    default: qs1=":HW-unbekannt";fwversion+=qs1;break;
    }
  }
  QString astty;
  if(grunddaten.variante==vnwt_nn){
    astty = qs + "[" + fwversion + "]";
  }else{
    astty = qs + "[" + stty + "-" + fwversion + "]";
  }
  setWindowTitle(astty);
  if(bd)qDebug("Nwt4Window::setAText ENDE");
}

void Nwt4Window::actrepainttimer(){
  if(bd)qDebug("Nwt4Window::actrepainttimer()");
  repainttimer->stop();
  grafiksetzen();
  resize(this->rect().size());
  if(bd)qDebug("Nwt4Window::actrepainttimer() ENDE");
}

void Nwt4Window::grafiksetzen(){
  int w = nwt4widget->width();
  int h = nwt4widget->height();

  int gl = w-200;
  int a = 25;
  int bw = 70;

  if(bd)qDebug("Nwt4Window::grafiksetzen");
  display->setGeometry(0,0,gl,h-a-5);
  //toolbutton->setGeometry(gl-4*60-9, h-a, 60, a);
  buttonwobbeln->setGeometry(gl-3*bw-9, h-a, bw, a);
  buttoneinzeln->setGeometry(gl-2*bw-6, h-a, bw, a);
  buttonstop->setGeometry(gl-bw-3, h-a, bw, a);

  rlauf->setGeometry(10, h-a, gl-4*60, a-3);

  int wxl=16; //label x
  int wyl=26;  //label y
  int wwl=50; //label weite
  int whl=17; //label hoehe

  int wxe=60; //edit x
  int wye=21;  //edit y
  int wwe=120;//edit weite
  int whe=27; //edit hoehe

  int wa=30;  //Y abstand

  tabwidgetwobbeln->setGeometry(gl, 0, 200, h);

  grwobbel->setGeometry(3,3,190,240);

  lineEdit_frq_anfang->setGeometry(wxe, wye, wwe, whe);
  label_frq_anfang->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  lineEdit_frq_ende->setGeometry(wxe, wye, wwe, whe);
  label_frq_ende->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  lineEdit_frq_mpunkte->setGeometry(wxe, wye, wwe, whe);
  label_mpunkte->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  lineEdit_schrittweite->setGeometry(wxe, wye, wwe, whe);
  label_frq_schrittweite->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  lineEdit_frq_mitte->setGeometry(wxe, wye, wwe, whe);
  label_frq_mitte->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  lineEdit_frq_span->setGeometry(wxe, wye, wwe, whe);
  label_frq_span->setGeometry(wxl, wyl, wwl, whl);
  wye+=wa;wyl+=wa;
  boxztime->setGeometry(wxe+80, wye, wwe-80, whe);
  label_ztime->setGeometry(wxl, wyl, wwl+80, whl);
  wye+=wa;wyl+=wa;

  wxl=11; //label x
  wyl=26; //label y
  wwl=60; //label weite
  whl=17; //label hoehe

  wxe=81; //edit x
  wye=21; //edit y
  wwe=65; //edit weite
  int wwe1=65; //edit weite
  whe=27; //edit hoehe

  grygrenzen->setGeometry(3,250,190,85);

  labelboxydbmax->setGeometry(wxl, wyl, wwl, whl);
  boxydbmax->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;wyl+=wa;
  labelboxydbmin->setGeometry(wxl, wyl, wwl, whl);
  boxydbmin->setGeometry(wxe, wye, wwe, whe);

  wwl=35; //label weite
  wxe=61; //edit x
  wwe=50; //edit weite

  grkanal->setGeometry(3,340,190,110);

  wye=21; //edit y

  checkboxkanal->setGeometry(wxl, 20, 80, 20);
  buttonloadmessk->setGeometry(wxl+80, 20, 70, 20);
  buttoninfomessk->setGeometry(wxl+80+70, 20, 20, 20);
  labelkanal->setGeometry(wxl, 36, 190, 40);
  checkboxspeki->setGeometry(wxl, 80, 160, 20);

  wxe=16; //edit x
  wye=26;  //edit y
  wwe=150;//edit weite
  whe=27; //edit hoehe

  wa=30;  //Y abstand

  grbandbreite->setGeometry(3,3,190,310);
  checkboxdbmax->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkboxdbmin->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkbox3db->setGeometry(wxe, wye, wwe, whe);
  checkboxwatt->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkbox3dbinv->setGeometry(wxe, wye, wwe, whe);
  checkboxvolt->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkboxguete->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkbox6db->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkbox60db->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkboxshape->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;
  checkboxswvkanal->setGeometry(wxe, wye, wwe, whe);
  wye+=wa;

  wxl=6; //label x
  wyl=26;  //label y
  wwl=66; //label weite
  whl=17; //label hoehe

  wxe=75; //edit x
  wye=21;  //edit y
  wwe=60;//edit weite
  whe=27; //edit hoehe

  wa=30;  //Y abstand

  grzusatzlinien->setGeometry(3,320,190,115);
  labelboxdblinie->setGeometry(wxl, wyl, wwl, whl);
  boxdblinie->setGeometry(wxe-10, wye, wwe, whe);
  checkboxdblinie->setGeometry(wxe+55, wye, wwe1, whe);
  wye+=wa;wyl+=wa;
  labelboxswvlinie1->setGeometry(wxl, wyl, wwl-10, whl);
  boxswvlinie1->setGeometry(wxe-10, wye, wwe, whe);
  checkboxswvlinie1->setGeometry(wxe+55, wye, wwe1, whe);
  wye+=wa;wyl+=wa;
  labelboxswvlinie2->setGeometry(wxl, wyl, wwl-10, whl);
  boxswvlinie2->setGeometry(wxe-10, wye, wwe, whe);
  checkboxswvlinie2->setGeometry(wxe+55, wye, wwe1, whe);


  wxe=65; //edit x
  wye=21;  //edit y
  wwe=50;//edit weite
  whe=27; //edit hoehe
  //int wxe1=122; //edit x

  wxe=65; //edit x
  wye=21;  //edit y
  wwe=100;//edit weite
  whe=27; //edit hoehe
  //int wxe1=122; //edit x

  wxl=16; //label x
  wyl=26;  //label y
  wwl=50; //label weite
  whl=17; //label hoehe

  grswvant->setGeometry(3,3,190,90);
  labelalaenge->setGeometry(wxl, wyl, wwl+20, whl);
  boxalaenge->setGeometry(wxe+50, wye, wwe-30, whe);
  wye+=wa;wyl+=wa;
  labela_100->setGeometry(wxl, wyl, wwl, whl);
  boxa_100->setGeometry(wxe, wye, wwe+20, whe);

  wxe=65; //edit x
  wye=21;  //edit y
  wwe=50;//edit weite
  whe=27; //edit hoehe

  wxl=16; //label x
  wyl=26;  //label y
  wwl=50; //label weite
  whl=17; //label hoehe

  wa=30;  //Y abstand

  grdshift->setGeometry(3,100,190,60);
  labeldshift->setGeometry(wxl, wyl, wwl, whl);
  boxdshift->setGeometry(wxe, wye, wwe, whe);

  grh1->setGeometry(3,3,190,85);
  checkboxh1->setGeometry(3,13,190,20);
  checkboxh1m->setGeometry(3,32,190,20);
  buttone1->setGeometry(5,55,20,25);
  buttonh1->setGeometry(25,55,50,25);
  buttonl1->setGeometry(75,55,50,25);
  buttonc1->setGeometry(125,55,45,25);
  buttoni1->setGeometry(170,55,15,25);
  grh2->setGeometry(3,93,190,85);
  checkboxh2->setGeometry(3,13,190,20);
  checkboxh2m->setGeometry(3,32,190,20);
  buttone2->setGeometry(5,55,20,25);
  buttonh2->setGeometry(25,55,50,25);
  buttonl2->setGeometry(75,55,50,25);
  buttonc2->setGeometry(125,55,45,25);
  buttoni2->setGeometry(170,55,15,25);
  grh3->setGeometry(3,183,190,85);
  checkboxh3->setGeometry(3,13,190,20);
  checkboxh3m->setGeometry(3,32,190,20);
  buttone3->setGeometry(5,55,20,25);
  buttonh3->setGeometry(25,55,50,25);
  buttonl3->setGeometry(75,55,50,25);
  buttonc3->setGeometry(125,55,45,25);
  buttoni3->setGeometry(170,55,15,25);
  grh4->setGeometry(3,273,190,85);
  checkboxh4->setGeometry(3,13,190,20);
  checkboxh4m->setGeometry(3,32,190,20);
  buttone4->setGeometry(5,55,20,25);
  buttonh4->setGeometry(25,55,50,25);
  buttonl4->setGeometry(75,55,50,25);
  buttonc4->setGeometry(125,55,45,25);
  buttoni4->setGeometry(170,55,15,25);
  grh5->setGeometry(3,363,190,85);
  checkboxh5->setGeometry(3,13,190,20);
  checkboxh5m->setGeometry(3,32,190,20);
  buttone5->setGeometry(5,55,20,25);
  buttonh5->setGeometry(25,55,50,25);
  buttonl5->setGeometry(75,55,50,25);
  buttonc5->setGeometry(125,55,45,25);
  buttoni5->setGeometry(170,55,15,25);

  wxl=40; //label x
  wyl=20;  //label y
  wwl=90; //label weite
  whl=30; //label hoehe

  wxe=wxl-20; //edit x
  wye=wyl+30;  //edit y
  wwe=40;//edit weite


  //mW-Meter Kanal1
  whe=30; //edit hoehe
  int y=20;
  int x=10;
  int wg=710;
  //Positionen oberhalb Baranzeige
  int w1=60,w2=60,w3=40,w4=150,w5=170,w6=170,w7=0,w8=0;
  int p1=20,p2=p1+w1,p3=p2+w2,p4=p3+w3,p5=p4+w4,p6=p5+w5,p7=0,p8=0;

  grmesskanal->setGeometry(x,y,wg+50,125);

  y=30;
  checkkanal->setGeometry(p1,y,w1,whe);
  labeldbmdigitkanal->setGeometry(p2,y,w2,whe);
  spdbmdigitkanal->setGeometry(p3,y,w3,whe);
  labeldbmkanal->setGeometry(p4,y,w4,whe);
  labelvoltkanal->setGeometry(p5,y,w5,whe);
  labelwattkanal->setGeometry(p6,y,w6,whe);

  y+=35;
  progressbarkanal->setGeometry(20,y,wg-40,whe-10);
  labeladckanal->setGeometry(wg-20,y,50,whe);

  //Positionen unterhalb Baranzeige
  w1=80,w2=70,w3=140,w4=120,w5=80,w6=80,w7=90,w8=80;
  p1=20,p3=p1+w1,p4=p3+w3,p5=p4+w4,p6=p5+w5,p7=p6+w6,p8=p7+w7;

  y+=22;
  spbarminkanal->setGeometry(p1,y,w1,whe);
  labelattextkanal->setGeometry(p4,y,w4,whe);
  spattextkanal->setGeometry(p5,y,w5,whe);
  labelhangkanal->setGeometry(p6,y,w6,whe);
  sphangkanal->setGeometry(p7,y,w7,whe);
  spbarmaxkanal->setGeometry(p8,y,w8,whe);

  //mW-Meter Kanal2
  y=170;

  //Positionen oberhalb Baranzeige
  w1=60,w2=60,w3=40,w4=150,w5=170,w6=170,w7=0;
  p1=20,p2=p1+w1,p3=p2+w2,p4=p3+w3,p5=p4+w4,p6=p5+w5,p7=0;

  y=30;

  y+=35;

  //Positionen unterhalb Baranzeige
  w1=80,w2=70,w3=160,w4=50,w5=60,w6=80,w7=90,w8=80;
  p1=20,p2=p1+w1,p3=p2+w2,p4=p3+w3,p5=p4+w4,p6=p5+w5,p7=p6+w6,p8=p7+w7;

  y+=22;

  w = widgetmwatt->width();
  whline->setGeometry(0,150,w,20);
  //VFO mit dBm Ausgabe
  y=180;
  grmessvfo->setGeometry(x,y,450,150);
  labelspmhz->setGeometry(wxl,wyl,wwl,whl);
  wxe-=10;
  sp1ghz->setGeometry(wxe,wye,wwe,whe);
  sp100mhz->setGeometry(wxe+40,wye,wwe,whe);
  sp10mhz->setGeometry(wxe+40*2,wye,wwe,whe);
  sp1mhz->setGeometry(wxe+40*3,wye,wwe,whe);
  wxl+=160; wxe=wxl-20;
  labelspkhz->setGeometry(wxl,wyl,wwl,whl);
  sp100khz->setGeometry(wxe,wye,wwe,whe);
  sp10khz->setGeometry(wxe+40,wye,wwe,whe);
  sp1khz->setGeometry(wxe+40*2,wye,wwe,whe);
  wxl+=130; wxe=wxl-20;
  labelsphz->setGeometry(wxl,wyl,wwl,whl);
  sp100hz->setGeometry(wxe,wye,wwe,whe);
  sp10hz->setGeometry(wxe+40,wye,wwe,whe);
  sp1hz->setGeometry(wxe+40*2,wye,wwe,whe);
  bvfo2sp->setGeometry(wxe-50,wye+5+30,170,25);
  bsp2vfo->setGeometry(wxe-50,wye+5+30*2,170,25);
  wxl=30; wxe=wxl-20; wwe=100; x+=460;
  //VF0 dBm Pegelsteller
  //VFO externes Daempfungsglied
  grlcda->setGeometry(x,y,250,290);
  LCD1->setGeometry(30,30,200,45);
  rb1->setGeometry(10,30,20,40);
  LCD2->setGeometry(30,80,200,45);
  rb2->setGeometry(10,80,20,40);
  LCD3->setGeometry(30,130,200,45);
  rb3->setGeometry(10,130,20,40);
  LCD4->setGeometry(30,180,200,45);
  rb4->setGeometry(10,180,20,40);
  LCD5->setGeometry(30,230,200,45);
  rb5->setGeometry(10,230,20,40);



  //berechnungsfenster
  groupschwingkreis->setGeometry(20,20,230,170);
  editf1->setGeometry(20, 20, 100, 20);
  labelf1->setGeometry(130, 20, 90, 20);
  editl1->setGeometry(20, 40, 100, 20);
  labell1->setGeometry(130, 40, 90, 20);
  editc1->setGeometry(20, 60, 100, 20);
  labelc1->setGeometry(130, 60, 90, 20);
  ergebnisxc->setGeometry(20, 80, 100, 20);
  labelxc->setGeometry(130, 80, 90, 20);
  ergebnisxl->setGeometry(20, 100, 100, 20);
  labelxl->setGeometry(130, 100, 90, 20);
  editn->setGeometry(20, 120, 100, 20);
  labeln->setGeometry(130, 120, 90, 20);
  edital->setGeometry(20, 140, 100, 20);
  labelal->setGeometry(130, 140, 90, 20);

  groupwindungen->setGeometry(20,200,230,90);
  edital3->setGeometry(20, 20, 100, 20);
  labeledital3->setGeometry(130, 20, 90, 20);
  editl3->setGeometry(20, 40, 100, 20);
  labeleditl3->setGeometry(130, 40, 90, 20);
  ergebnisw->setGeometry(25, 60, 95, 20);
  labelergebnisw->setGeometry(130, 60, 90, 20);

  grdbmvw->setGeometry(260,20,370,90);
  spdbm->setGeometry(10,25,120,30);
  cbdbm01->setGeometry(10,55,100,30);
  edvolt->setGeometry(140,40,100,30);
  edwatt->setGeometry(250,40,100,30);

  grrloss->setGeometry(260,130,370,100);
  sprloss->setGeometry(10,35,100,30);
  cbrloss01->setGeometry(10,65,100,30);
  labelswv->setGeometry(110,35,110,30);
  labelrfaktor->setGeometry(120,65,100,30);
  labelzlow->setGeometry(230,35,130,30);
  labelzhigh->setGeometry(230,65,130,30);

  //berechnungsfenster ENDE

  //Impedanzberechnung
  gimp->setGeometry(20,20,130,50);
  rbr->setGeometry(20,20,50,20);
  rblc->setGeometry(70,20,50,20);

  bild1->setGeometry(280,110,444,156);
  gzr->setGeometry(20,100,250,220);
  editz1->setGeometry(20, 20, 50, 20);
  labeleditz1->setGeometry(90, 20, 90, 20);
  editz2->setGeometry(20, 40, 50, 20);
  labeleditz2->setGeometry(90, 40, 90, 20);
  editz3->setGeometry(20, 60, 50, 20);
  labeleditz3->setGeometry(90, 60, 90, 20);
  editz4->setGeometry(20, 80, 50, 20);
  labeleditz4->setGeometry(90, 80, 90, 20);
  lr1->setGeometry(20, 100, 50, 20);
  lbeschrr1->setGeometry(90, 100, 90, 20);
  lr2->setGeometry(20, 120, 50, 20);
  lbeschrr2->setGeometry(90, 120, 90, 20);
  lr3->setGeometry(20, 140, 50, 20);
  lbeschrr3->setGeometry(90, 140, 90, 20);
  lr4->setGeometry(20, 160, 50, 20);
  lbeschrr4->setGeometry(90, 160, 90, 20);
  ldaempfung->setGeometry(20, 180, 50, 20);
  lbeschrdaempfung->setGeometry(90, 180, 160, 20);

  bild2->setGeometry(280,110,444,209);
  gzlc->setGeometry(20,100,250,130);
  editzlc1->setGeometry(20, 20, 100, 20);
  labeleditzlc1->setGeometry(140, 20, 90, 20);
  editzlc2->setGeometry(20, 40, 100, 20);
  labeleditzlc2->setGeometry(140, 40, 90, 20);
  editzlc3->setGeometry(20, 60, 100, 20);
  labeleditzlc3->setGeometry(140, 60, 90, 20);
  ll->setGeometry(20, 80, 100, 20);
  lbeschrl->setGeometry(140, 80, 90, 20);
  lc->setGeometry(20, 100, 100, 20);
  lbeschrc->setGeometry(140, 100, 90, 20);
  //Impedanzberechnung ENDE

  if(bd)qDebug("Nwt4Window::grafiksetzen ENDE");
}

void Nwt4Window::cfgladen(){
  configfile cfgk;
  int r,g,b,a;
  QPalette palette;
  QString qs1;

  if(bd)qDebug("Nwt4Window::cfgladen");
  cfgk.open(grunddaten.homedir.filePath(".nwt4.cfg"));
  r = cfgk.readInteger("hintergrundr", 255);
  g = cfgk.readInteger("hintergrundg", 255);
  b = cfgk.readInteger("hintergrundb", 152);
  a = cfgk.readInteger("hintergrunda", 255);
  grunddaten.colorhintergrund = QColor(r,g,b,a);
  ksettings.setcolorhintergrund(grunddaten.colorhintergrund);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("fmarkenr", 0);
  g = cfgk.readInteger("fmarkeng", 255);
  b = cfgk.readInteger("fmarkenb", 255);
  a = cfgk.readInteger("fmarkena", 255);
  grunddaten.colorfmarken = QColor(r,g,b,a);
  ksettings.setcolorfmarken(grunddaten.colorfmarken);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("wobelspurr", 188);
  g = cfgk.readInteger("wobelspurg", 94);
  b = cfgk.readInteger("wobelspurb", 0);
  a = cfgk.readInteger("wobelspura", 255);
  grunddaten.colorwobbelspur = QColor(r,g,b,a);
  ksettings.setcolorspur(grunddaten.colorwobbelspur);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("schriftr", 118);
  g = cfgk.readInteger("schriftg", 117);
  b = cfgk.readInteger("schriftb", 136);
  a = cfgk.readInteger("schrifta", 255);
  grunddaten.colorschrift = QColor(r,g,b,a);
  ksettings.setcolorschrift(grunddaten.colorschrift);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("markerschriftr", 0);
  g = cfgk.readInteger("markerschriftg", 0);
  b = cfgk.readInteger("markerschriftb", 0);
  a = cfgk.readInteger("markerschrifta", 255);
  grunddaten.colormarkerschrift = QColor(r,g,b,a);
  ksettings.setcolormarkerschrift(grunddaten.colormarkerschrift);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("c1linier", 255);
  g = cfgk.readInteger("c1linieg", 0);
  b = cfgk.readInteger("c1linieb", 0);
  a = cfgk.readInteger("c1liniea", 255);
  grunddaten.colorlinie = QColor(r,g,b,a);
  ksettings.setcolorlinek(grunddaten.colorlinie);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("c1liniemr", 255);
  g = cfgk.readInteger("c1liniemg", 0);
  b = cfgk.readInteger("c1liniemb", 0);
  a = cfgk.readInteger("c1liniema", 255);
  grunddaten.colorliniemarker = QColor(r,g,b,a);
  ksettings.setcolorlinekmarker(grunddaten.colorliniemarker);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("h1linier", 21);
  g = cfgk.readInteger("h1linieg", 115);
  b = cfgk.readInteger("h1linieb", 255);
  a = cfgk.readInteger("h1liniea", 255);
  colorhline1 = QColor(r,g,b,a);
  ksettings.setcolorhline1(colorhline1);
  palette = buttonc1->palette();
  palette.setColor(QPalette::ButtonText, colorhline1);
  buttonc1->setPalette(palette);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("h2linier", 104);
  g = cfgk.readInteger("h2linieg", 3);
  b = cfgk.readInteger("h2linieb", 255);
  a = cfgk.readInteger("h2liniea", 255);
  colorhline2 = QColor(r,g,b,a);
  ksettings.setcolorhline2(colorhline2);
  palette = buttonc2->palette();
  palette.setColor(QPalette::ButtonText, colorhline2);
  buttonc2->setPalette(palette);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("h3linier", 228);
  g = cfgk.readInteger("h3linieg", 17);
  b = cfgk.readInteger("h3linieb", 255);
  a = cfgk.readInteger("h3liniea", 255);
  colorhline3 = QColor(r,g,b,a);
  ksettings.setcolorhline3(colorhline3);
  palette = buttonc3->palette();
  palette.setColor(QPalette::ButtonText, colorhline3);
  buttonc3->setPalette(palette);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("h4linier", 15);
  g = cfgk.readInteger("h4linieg", 255);
  b = cfgk.readInteger("h4linieb", 3);
  a = cfgk.readInteger("h4liniea", 255);
  colorhline4 = QColor(r,g,b,a);
  ksettings.setcolorhline4(colorhline4);
  palette = buttonc4->palette();
  palette.setColor(QPalette::ButtonText, colorhline4);
  buttonc4->setPalette(palette);
  //----------------------------------------------------------------------
  r = cfgk.readInteger("h5linier", 170);
  g = cfgk.readInteger("h5linieg", 0);
  b = cfgk.readInteger("h5linieb", 0);
  a = cfgk.readInteger("h5liniea", 255);
  colorhline5 = QColor(r,g,b,a);
  ksettings.setcolorhline5(colorhline5);
  palette = buttonc5->palette();
  palette.setColor(QPalette::ButtonText, colorhline5);
  buttonc5->setPalette(palette);
  //----------------------------------------------------------------------
  grunddaten.variante = evariante(cfgk.readInteger("nwtvariante", int(vnwt4_1)));
  grunddaten.bflatcal= cfgk.readBool("flatcal", false);
  if(grunddaten.variante==vnwt_ltdz or grunddaten.variante==vnwt_nn)
    grunddaten.bflatcal=true;//Variante LTDZ Flatcal und NN immer sperren
  mCalibFlat->setDisabled(grunddaten.bflatcal);

  grunddaten.filemk= cfgk.readString("mkfile", "Messkopf" );
  grunddaten.bnozwtime= cfgk.readBool("nozwtime", false);
  int zt=cfgk.readInteger("zwischenzeit", 0);
  boxztime->setValue(zt);
  label_ztime->setEnabled(!grunddaten.bnozwtime);
  boxztime->setEnabled(!grunddaten.bnozwtime);
  if(grunddaten.bnozwtime)boxztime->setValue(0);
  //----------------------------------------------------------------------
  wobbelgrunddaten.frequenzanfang= cfgk.readDouble("frequenz_anfang", 138.0);//138 MHz
  wobbelgrunddaten.frequenzschritt= cfgk.readDouble("frequenz_schritt", 8.524);
  wobbelgrunddaten.mpunkte= cfgk.readInteger("mpunkte", 501);
  wobbelgrunddaten.calfrqmin= cfgk.readDouble("calfrqmin",138.0);
  wobbelgrunddaten.calfrqmax= cfgk.readDouble("calfrqmax",4400.0);
  wobbelgrunddaten.bdBm = cfgk.readBool("spektrumanalyse", false);
  wobbelgrunddaten.b0dBline = cfgk.readBool("0dBLine", true);

  checkboxspeki->setChecked(wobbelgrunddaten.bdBm);
  setSpeki(wobbelgrunddaten.bdBm);
  wobbelgrunddaten.bwatt = cfgk.readBool("speki_watt", false);
  checkboxwatt->setChecked(wobbelgrunddaten.bwatt);
  wobbelgrunddaten.bvolt = cfgk.readBool("speki_volt", false);
  checkboxvolt->setChecked(wobbelgrunddaten.bvolt);
  //----------------------------------------------------------------------
  //wobbelgrunddaten.mpunktemin= cfgk.readInteger("mpunktemin", 10);
  wobbelgrunddaten.dbmin= cfgk.readDouble("dbmin", -90.0);
  boxydbmin->setValue(wobbelgrunddaten.dbmin);
  wobbelgrunddaten.dbmax= cfgk.readDouble("dbmax", 10.0);
  boxydbmax->setValue(wobbelgrunddaten.dbmax);
  bool k=cfgk.readBool("Kanal1", true);
  messkurve.bkanal=k;
  btip=cfgk.readBool("ToolTip", true);
  mTooltip->setChecked(btip);
  tip(btip);
  grunddaten.formatfilename=cfgk.readString("formatfilename","yyMMdd_hhmmss");
  grunddaten.bset0hz = cfgk.readBool("wobbelset0hz",true);
  grunddaten.bmwmeter = cfgk.readBool("bmwmeter",true);
  checkkanal->setChecked(grunddaten.bmwmeter);
  grunddaten.precisionmw = cfgk.readInteger("precisionmw",1);
  spdbmdigitkanal->setValue(grunddaten.precisionmw);
  grunddaten.nwttakt = cfgk.readDouble("nwttakt", 1000000000.0);
  grunddaten.nwttaktcalfrq = cfgk.readDouble("nwttaktcalfrq", 1000000000.0);
  grunddaten.fontsize = cfgk.readInteger("fontsize",12);
  grunddaten.markerfontsize = cfgk.readInteger("markerfontsize", 12);
  grunddaten.mwfontsize = cfgk.readInteger("mwfontsize", 14);
  grunddaten.vfofontsize = cfgk.readInteger("vfofontsize", 14);
  grunddaten.berfontsize = cfgk.readInteger("berfontsize", 13);
  grunddaten.grberfontsize = cfgk.readInteger("grberfontsize", 12);
  grunddaten.bfontsize = cfgk.readBool("bfontsize",true);
  ksettings.setfontsize(grunddaten.fontsize);
  ksettings.setmarkerfontsize(grunddaten.markerfontsize);
  ksettings.setEnabledfontsize(grunddaten.bfontsize);
  //-----------------------------------------------------------------------
  double f = cfgk.readDouble("messvfo",wobbelgrunddaten.calfrqmin * 1000000.0);
  setMessVfoFrq(f);
  f = cfgk.readDouble("LCD1", 0.0);
  LCD1->display(f);
  f = cfgk.readDouble("LCD2", 0.0);
  LCD2->display(f);
  f = cfgk.readDouble("LCD3", 0.0);
  LCD3->display(f);
  f = cfgk.readDouble("LCD4", 0.0);
  LCD4->display(f);
  f = cfgk.readDouble("LCD5", 0.0);
  LCD5->display(f);
  //-----------------------------------------------------------------------
  //Zusätzliche USB-Schnittstelle
  QString qsdevusb = cfgk.readString("devusb","nodev");
  if(!(qsdevusb=="nodev")){
    listtty.append(qsdevusb);
    //qDebug()<<"devusb="<<qsdevusb;
  }
  for(int j=0;j<16;j++){
    qs1.sprintf("%02i",j);
    frqmarken.aktiv[j]=cfgk.readBool("frqmactiv"+qs1, false);
    frqmarken.frq1[j]=cfgk.readDouble("frqmf1"+qs1, 0.0);
    frqmarken.frq2[j]=cfgk.readDouble("frqmf2"+qs1, 0.0);
    frqmarken.caption[j]=cfgk.readString("caption"+qs1, "def");
  }
  //Configdatei schliessen
  cfgk.close();
  //-----------------------------------------------------------------------
  //Kontrolle der geladenen Daten, keine 0 Werte sind erlaubt
  if(wobbelgrunddaten.frequenzanfang==0) wobbelgrunddaten.frequenzanfang=wobbelgrunddaten.calfrqmin;
  if(wobbelgrunddaten.frequenzschritt==0) wobbelgrunddaten.frequenzschritt=1.0;
  if(wobbelgrunddaten.mpunkte==0) wobbelgrunddaten.mpunkte= 500;
  if(wobbelgrunddaten.dbmin==0) wobbelgrunddaten.dbmin= -90.0;
  if(wobbelgrunddaten.dbmax==0) wobbelgrunddaten.dbmax= 10.0;
  boxydbmin->setValue(wobbelgrunddaten.dbmin);
  boxydbmax->setValue(wobbelgrunddaten.dbmax);
  //-----------------------------------------------------------------------
  wobnormalisieren();
  display->readSettings();
  if(bd)qDebug("Nwt4Window::cfgladen ENDE");
}

void Nwt4Window::cfgspeichern(){
  configfile cfgk;
  int r,g,b,a;
  QString qs1;

  if(bd)qDebug("Nwt4Window::cfgspeichern");
  cfgk.open(grunddaten.homedir.filePath(".nwt4.cfg"));
  //----------------------------------------------------------------------
  grunddaten.colorhintergrund.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("hintergrundr", r);
  cfgk.writeInteger("hintergrundg", g);
  cfgk.writeInteger("hintergrundb", b);
  cfgk.writeInteger("hintergrunda", a);
  //----------------------------------------------------------------------
  grunddaten.colorwobbelspur.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("wobelspurr", r);
  cfgk.writeInteger("wobelspurg", g);
  cfgk.writeInteger("wobelspurb", b);
  cfgk.writeInteger("wobelspura", a);
  //----------------------------------------------------------------------
  grunddaten.colorfmarken.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("fmarkenr", r);
  cfgk.writeInteger("fmarkeng", g);
  cfgk.writeInteger("fmarkenb", b);
  cfgk.writeInteger("fmarkena", a);
  //----------------------------------------------------------------------
  grunddaten.colorschrift.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("schriftr", r);
  cfgk.writeInteger("schriftg", g);
  cfgk.writeInteger("schriftb", b);
  cfgk.writeInteger("schrifta", a);
  //----------------------------------------------------------------------
  grunddaten.colormarkerschrift.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("markerschriftr", r);
  cfgk.writeInteger("markerschriftg", g);
  cfgk.writeInteger("markerschriftb", b);
  cfgk.writeInteger("markerschrifta", a);
  //----------------------------------------------------------------------
  grunddaten.colorlinie.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("c1linier", r);
  cfgk.writeInteger("c1linieg", g);
  cfgk.writeInteger("c1linieb", b);
  cfgk.writeInteger("c1liniea", a);
  //----------------------------------------------------------------------
  grunddaten.colorliniemarker.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("c1liniemr", r);
  cfgk.writeInteger("c1liniemg", g);
  cfgk.writeInteger("c1liniemb", b);
  cfgk.writeInteger("c1liniema", a);
  //----------------------------------------------------------------------
  colorhline1.getRgb(&r, &g, &b, &a);
  //qDebug("r:%i, g:%i, b:%i, a:%i", r, g, b, a);
  cfgk.writeInteger("h1linier", r);
  cfgk.writeInteger("h1linieg", g);
  cfgk.writeInteger("h1linieb", b);
  cfgk.writeInteger("h1liniea", a);
  //----------------------------------------------------------------------
  colorhline2.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("h2linier", r);
  cfgk.writeInteger("h2linieg", g);
  cfgk.writeInteger("h2linieb", b);
  cfgk.writeInteger("h2liniea", a);
  //----------------------------------------------------------------------
  colorhline3.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("h3linier", r);
  cfgk.writeInteger("h3linieg", g);
  cfgk.writeInteger("h3linieb", b);
  cfgk.writeInteger("h3liniea", a);
  //----------------------------------------------------------------------
  colorhline4.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("h4linier", r);
  cfgk.writeInteger("h4linieg", g);
  cfgk.writeInteger("h4linieb", b);
  cfgk.writeInteger("h4liniea", a);
  //----------------------------------------------------------------------
  colorhline5.getRgb(&r, &g, &b, &a);
  cfgk.writeInteger("h5linier", r);
  cfgk.writeInteger("h5linieg", g);
  cfgk.writeInteger("h5linieb", b);
  cfgk.writeInteger("h5liniea", a);
  //----------------------------------------------------------------------
  //cfgk.writeInteger("mpunktemin", wobbelgrunddaten.mpunktemin);
  cfgk.writeDouble("dbmin", wobbelgrunddaten.dbmin);
  cfgk.writeDouble("dbmax", wobbelgrunddaten.dbmax);
  cfgk.writeDouble("frequenz_anfang", wobbelgrunddaten.frequenzanfang);
  cfgk.writeDouble("frequenz_schritt", wobbelgrunddaten.frequenzschritt);
  cfgk.writeInteger("mpunkte", wobbelgrunddaten.mpunkte);
  cfgk.writeDouble("calfrqmin", wobbelgrunddaten.calfrqmin);
  cfgk.writeDouble("calfrqmax", wobbelgrunddaten.calfrqmax);
  cfgk.writeBool("spektrumanalyse", wobbelgrunddaten.bdBm);
  cfgk.writeBool("speki_watt", wobbelgrunddaten.bwatt);
  cfgk.writeBool("speki_volt", wobbelgrunddaten.bvolt);
  cfgk.writeBool("0dBLine", wobbelgrunddaten.b0dBline);

  cfgk.writeBool("Kanal1",messkurve.bkanal);
  cfgk.writeBool("ToolTip", btip);
  //----------------------------------------------------------------------
  cfgk.writeInteger("nwtvariante",int(grunddaten.variante));
  cfgk.writeString("mkfile", grunddaten.filemk);
  cfgk.writeBool("flatcal", grunddaten.bflatcal);
  cfgk.writeBool("nozwtime", grunddaten.bnozwtime);
  int zt=boxztime->value();
  cfgk.writeInteger("zwischenzeit", zt);
  //----------------------------------------------------------------------
  //qDebug("vfofrequenz:%9.0f", grunddaten.vfofrequenz);
  cfgk.writeString("formatfilename",grunddaten.formatfilename);
  cfgk.writeBool("wobbelset0hz",grunddaten.bset0hz);
  cfgk.writeBool("bmwmeter",grunddaten.bmwmeter);
  cfgk.writeInteger("precisionmw",grunddaten.precisionmw);
  cfgk.writeDouble("nwttakt", grunddaten.nwttakt);
  cfgk.writeDouble("nwttaktcalfrq", grunddaten.nwttaktcalfrq);
  //----------------------------------------------------------------------
  cfgk.writeInteger("fontsize",grunddaten.fontsize);
  cfgk.writeInteger("markerfontsize", grunddaten.markerfontsize);
  cfgk.writeInteger("mwfontsize", grunddaten.mwfontsize);
  cfgk.writeInteger("vfofontsize", grunddaten.vfofontsize);
  cfgk.writeInteger("berfontsize", grunddaten.berfontsize);
  cfgk.writeInteger("grberfontsize", grunddaten.grberfontsize);
  cfgk.writeBool("bfontsize",grunddaten.bfontsize);
  //----------------------------------------------------------------------
  double f=getMessVfoFrq();
  cfgk.writeDouble("messvfo",f);
  f=LCD1->value();
  cfgk.writeDouble("LCD1",f);
  f=LCD2->value();
  cfgk.writeDouble("LCD2",f);
  f=LCD3->value();
  cfgk.writeDouble("LCD3",f);
  f=LCD4->value();
  cfgk.writeDouble("LCD4",f);
  f=LCD5->value();
  cfgk.writeDouble("LCD5",f);
  //----------------------------------------------------------------------
  for(int j=0;j<16;j++){
    qs1.sprintf("%02i",j);
    cfgk.writeBool("frqmactiv"+qs1, frqmarken.aktiv[j]);
    cfgk.writeDouble("frqmf1"+qs1, frqmarken.frq1[j]);
    cfgk.writeDouble("frqmf2"+qs1, frqmarken.frq2[j]);
    cfgk.writeString("caption"+qs1, frqmarken.caption[j]);
  }
  //----------------------------------------------------------------------
  cfgk.close();
  //----------------------------------------------------------------------
  if(bd)qDebug("Nwt4Window::cfgspeichern ENDE");
}

void Nwt4Window::setDisplayYmax(int i){
  if(bd)qDebug("Nwt4Window::setDisplayYmax");
  int dmax=i;
  int dmin=boxydbmin->value();
  if(dmax==dmin){
    dmax=dmin+10;
    boxydbmax->setValue(dmax);
  }
  wobbelgrunddaten.dbmax=double(dmax);
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::setDisplayYmax ENDE");
}

void Nwt4Window::setDisplayYmin(int i){
  if(bd)qDebug("Nwt4Window::setDisplayYmin");
  int dmin=i;
  int dmax=boxydbmax->value();
  if(dmax==dmin){
    dmin=dmax-10;
    boxydbmin->setValue(dmin);
  }
  wobbelgrunddaten.dbmin=double(dmin);
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::setDisplayYmin ENDE");
}

void Nwt4Window::checkboxkanal_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxkanal_checked(bool b)");
  messkurve.bkanal=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxkanal_checked(bool b) ENDE");
}

void Nwt4Window::checkboxspeki_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxspeki_checked(bool b)");
  wobbelgrunddaten.bdBm=b;
  setSpeki(wobbelgrunddaten.bdBm);
  WobbelGrundDatenToDisplay();
  setInfoText();
  messkurveClr();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxspeki_checked(bool b) ENDE");
}

void Nwt4Window::checkboxwatt_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxwatt_checked(bool b)");
  wobbelgrunddaten.bwatt=b;
  WobbelGrundDatenToDisplay();
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxwatt_checked(bool b) ENDE");
}

void Nwt4Window::checkboxvolt_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxvolt_checked(bool b)");
  wobbelgrunddaten.bvolt=b;
  WobbelGrundDatenToDisplay();
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxvolt_checked(bool b) ENDE");
}

void Nwt4Window::checkbox3db_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxk3db_checked(bool b)");
  messkurve.b3db=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxk3db_checked(bool b) ENDE");
}

void Nwt4Window::checkbox3dbinv_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxk3dbinv_checked(bool b)");
  messkurve.b3dbinv=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxk3dbinv_checked(bool b) ENDE");
}

void Nwt4Window::checkboxguete_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxguete_checked(bool b)");
  messkurve.bguete=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxguete_checked(bool b) ENDE");
}

void Nwt4Window::checkbox6db_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkbox6db_checked(bool b)");
  messkurve.b6db=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkbox6db_checked(bool b) ENDE");
}

void Nwt4Window::checkbox60db_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkbox60db_checked(bool b)");
  messkurve.b60db=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkbox60db_checked(bool b) ENDE");
}

void Nwt4Window::checkboxshape_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxshape_checked(bool b)");
  messkurve.bshape=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxshape_checked(bool b) ENDE");
}

void Nwt4Window::checkboxswvkanal_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxswvkanal_checked(bool b)");
  messkurve.bswv=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxswvkanal_checked(bool b) ENDE");
}

void Nwt4Window::checkboxdbmax_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxdbmax_checked(bool b)");
  messkurve.bdbmax=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxdbmax_checked(bool b) ENDE");
}

void Nwt4Window::checkboxdbmin_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxdbmin_checked(bool b)");
  messkurve.bdbmin=b;
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::checkboxdbmin_checked(bool b) ENDE");
}

void Nwt4Window::setswvEnabled(){
  if(bd)qDebug("Nwt4Window::setswvEnabled()");
  double d2;

  d2 = boxdshift->value();
  if(d2==0.0){
    checkboxswvlinie1->setEnabled(true);
    checkboxswvlinie2->setEnabled(true);
    checkboxswvkanal->setEnabled(true);
  }else{
    checkboxswvlinie1->setChecked(false);
    checkboxswvlinie2->setChecked(false);
    checkboxswvlinie1->setEnabled(false);
    checkboxswvlinie2->setEnabled(false);
  }
  checkboxswvkanal->setChecked(messkopf.MkTyp()==mks11 or messkopf.MkTyp()==mks11var);
  if(bd)qDebug("Nwt4Window::setswvEnabled() ENDE");
}

void Nwt4Window::setboxdshift(double d){
  if(bd)qDebug("Nwt4Window::setboxdshift(double d)");
  setswvEnabled();
  double dsh=d;
  double ds = double(int(dsh/10.0) * 10.0);
  wobbelgrunddaten.dshift=ds;
  //qDebug()<<"dshift= "<<ds;
  double ks = dsh - ds;
  wobbelgrunddaten.kshift=ks;
  //qDebug()<<"kshift= "<<ks;
  WobbelGrundDatenToDisplay();
  setInfoText();
  display->setMessKurve(messkurve);
  if(bd)qDebug("Nwt4Window::setboxdshift(double d) ENDE");
}

void Nwt4Window::setboxdblinie(double d){
  if(bd)qDebug("Nwt4Window::setboxdblinie(double d)");
  wobbelgrunddaten.dbline=d;
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::setboxdblinie(double d) ENDE");
}

void Nwt4Window::checkboxdblinie_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxdblinie_checked(bool b)");
  if(b){
    boxdblinie->setSingleStep(0.1);
  }else{
    boxdblinie->setSingleStep(1.0);
    wobbelgrunddaten.dbline = round(wobbelgrunddaten.dbline);
    boxdblinie->setValue(wobbelgrunddaten.dbline);
    WobbelGrundDatenToDisplay();
  }
  if(bd)qDebug("Nwt4Window::checkboxdblinie_checked(bool b) ENDE");
}

void Nwt4Window::setboxswvlinie1(double d){
  if(bd)qDebug("Nwt4Window::setboxswvlinie1(double d)");
  bool fertig=false;
  bool abwaertz=d<swvline1alt;
  if(boxswvlinie1->isEnabled()){
    if(abwaertz){
      if(!fertig and d<1.011){boxswvlinie1->setSingleStep(0.001);fertig=true;}
      if(!fertig and d<1.31){boxswvlinie1->setSingleStep(0.01);fertig=true;}
      if(!fertig){boxswvlinie1->setSingleStep(0.1);fertig=true;}
    }else{
      if(!fertig and d>1.29){boxswvlinie1->setSingleStep(0.1);fertig=true;}
      if(!fertig and d>1.009){boxswvlinie1->setSingleStep(0.01);fertig=true;}
      if(!fertig){boxswvlinie1->setSingleStep(0.001);fertig=true;}
    }
    ksettings.swvlinie1=d;
    swvline1alt=d;
  }else{
    ksettings.swvlinie1=0.0;
  }
  checkboxswvlinie1->setChecked(true);
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::setboxswvlinie1(double d) ENDE");
}

void Nwt4Window::checkboxswvlinie1_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxswvlinie1_checked(bool b)");
  if(b){
    ksettings.swvlinie1=boxswvlinie1->value();
  }else{
    ksettings.swvlinie1=0.0;
  }
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::checkboxswvlinie1_checked(bool b) ENDE");
}

void Nwt4Window::setboxswvlinie2(double d){
  if(bd)qDebug("Nwt4Window::setboxswvlinie2(double d)");
  bool fertig=false;
  bool abwaertz=d<swvline2alt;
  if(boxswvlinie2->isEnabled()){
    if(abwaertz){
      if(!fertig and d<1.011){boxswvlinie2->setSingleStep(0.001);fertig=true;}
      if(!fertig and d<1.31){boxswvlinie2->setSingleStep(0.01);fertig=true;}
      if(!fertig){boxswvlinie2->setSingleStep(0.1);fertig=true;}
    }else{
      if(!fertig and d>1.29){boxswvlinie2->setSingleStep(0.1);fertig=true;}
      if(!fertig and d>1.009){boxswvlinie2->setSingleStep(0.01);fertig=true;}
      if(!fertig){boxswvlinie2->setSingleStep(0.001);fertig=true;}
    }
    ksettings.swvlinie2=d;
    swvline2alt=d;
  }else{
    ksettings.swvlinie2=0.0;
  }
  checkboxswvlinie2->setChecked(true);
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::setboxswvlinie2(double d) ENDE");
}

void Nwt4Window::checkboxswvlinie2_checked(bool b){
  if(bd)qDebug("Nwt4Window::checkboxswvlinie2_checked(bool b)");
  if(b){
    ksettings.swvlinie2=boxswvlinie2->value();
  }else{
    ksettings.swvlinie2=0.0;
  }
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::checkboxswvlinie2_checked(bool b)");
}

void Nwt4Window::resizeEvent( QResizeEvent * ){
  //nach Groessenaenderung grafische Elemente anpassen
  resize(this->rect().size());
  grafiksetzen();

  //qDebug("breite %i hoehe %i", gsize.width(), gsize.height());
}

void Nwt4Window::repaintEvent( QPaintEvent * ){
  //nach Groessenaenderung grafische Elemente anpassen
  //qDebug("breite %i hoehe %i", width(), height());
  grafiksetzen();
  QWidget::repaint();
}

void Nwt4Window::closeEvent(QCloseEvent *event)
{
  korrKurveGeladen();//Frequenzeinstellungen korrigieren wenn Kurvendatei geladen ist
  cfgspeichern();//Einstellungen in Registry speichern
  writeSettings();//alle Einstellungen speichern
  //vfofgspeichern();//DDS-VFO Frequenzgang speichern

  event->accept();//Beenden erlauben
}

void Nwt4Window::clickbuttonwobbeln(){
  if(bd)qDebug("Nwt4Window::clickbuttonwobbeln()");
  wobnormalisieren();
  for(int i=0;i<wobbelgrunddaten.mpunkte;i++)
    messkurve.frequenz[i]=wobbelgrunddaten.frequenzanfang+wobbelgrunddaten.frequenzschritt*i; //frequenz in die Messkurve speichern
  spurClr();
  sendbefwobbeln();
  buttonwobbeln->setEnabled(false);
  buttoneinzeln->setEnabled(false);
  grwobbel->setEnabled(false);
  grzusatzlinien->setEnabled(false);
  menuMesskopf->setEnabled(false);
  menuKurven->setEnabled(false);
  display->setEnabled(false);
  widgetwkm->setEnabled(false);
  wobbeinmal=false;
  if(bd)qDebug("Nwt4Window::clickbuttonwobbeln() ENDE");
}

void Nwt4Window::clickbuttoneinzeln(){
  if(bd)qDebug("Nwt4Window::clickbuttoneinzeln()");
  wobnormalisieren();
  for(int i=0;i<wobbelgrunddaten.mpunkte;i++)
    messkurve.frequenz[i]=wobbelgrunddaten.frequenzanfang+wobbelgrunddaten.frequenzschritt*i; //frequenz in die Messkurve speichern
  spurClr();
  sendbefwobbeln();
  wobbeinmal=true;
  buttonwobbeln->setEnabled(false);
  buttoneinzeln->setEnabled(false);
  grwobbel->setEnabled(false);
  grzusatzlinien->setEnabled(false);
  menuMesskopf->setEnabled(false);
  menuKurven->setEnabled(false);
  display->setEnabled(false);
  widgetwkm->setEnabled(false);
  twobbel.restart();//Timer zum messen eines Wobbeldurchlaufes
  if(bd)qDebug("Nwt4Window::clickbuttoneinzeln() ENDE");
}

void Nwt4Window::sendbefwobbeln(){
  if(bd)qDebug("Nwt4Window::sendbefwobbeln()");
  qlonglong frqa;
  long frqs;
  int s, zt;
  QString qs;

  idxmesskurve=0;
  bbefwobbeln=true;
  if(messkurve.calablauf==0)
    WobbelGrundDatenToDisplay();
  frqa = round(wobbelgrunddaten.frequenzanfang * 100000.0); //Angabe in 10Hz
  frqs = round(wobbelgrunddaten.frequenzschritt * 100000.0); //Angabe in 10Hz
  s = wobbelgrunddaten.mpunkte; //int Anzahl der Schritte
  //Befehle zur Ansteuerung des NWT4000
  //wobbelgrunddaten.ztime = boxztime->value() * 111;
  zt = wobbelgrunddaten.ztime;
  if(grunddaten.variante == vnwt_ltdz){
    if(zt == 0)zt=1;
    qs.sprintf("a%09lli%08li%04i%03i", frqa, frqs, s, zt);//Wobbeln mit Zwischenzeit
    catbefehl='a';
  }else{
    if(zt >0){
      qs.sprintf("a%09lli%08li%04i%03i", frqa, frqs, s, zt);//Wobbeln mit Zwischenzeit
      catbefehl='a';
    }else{
      qs.sprintf("x%09lli%08li%04i", frqa, frqs, s);//Wobbeln ohne Zwischenzeit
      catbefehl='x';
    }
  }
  qDebug()<<qs;
  clrrxbuffer();//RS232 RX Buffer loeschen

  if(bnwtgefunden)picmodem->writeChar(0x8F);
  if(bnwtgefunden)picmodem->writeLine(qPrintable(qs));//Wobbelbefehl zum NWT senden
  //befantworttimer->start(20);//20mSek warten, anschliessend RS232 RX Buffer auswerten
  befantworttimer->start(10);//10mSek warten, anschliessend RS232 RX Buffer auswerten
  rlauf->setMaximum(wobbelgrunddaten.mpunkte);//Wobbelverlaufsanzeige max setzen
  buttonstop->setEnabled(true);//Button STOP aktivieren
  messkurve.mpunkte=wobbelgrunddaten.mpunkte;//Messpunkte in Messkurve kopieren
  messkurve.mktyp=messkopf.MkTyp();//MK-Typ in Messkurve speichern
  if(messkurve.calablauf==0){//kein Kalibrieren
    synclevel=0;
    setInfoText();//Info Text im Display, je nach Einstellung
    display->setMessKurve(messkurve);//Messkurve im Display darstellen
  }
  kurvebeschr="";
  if(bd)qDebug("Nwt4Window::sendbefwobbeln() ENDE");
}

void Nwt4Window::clickbuttonstop(){
  if(bd)qDebug("Nwt4Window::clickbuttonstop()");
  wobbeinmal=true;
  buttonstop->setEnabled(false);
  if(bd)qDebug("Nwt4Window::clickbuttonstop() ENDE");
}

void Nwt4Window::clrrxbuffer(){
  if(bd)qDebug("Nwt4Window::clrrxbuffer()");
  char abuffer[maxmesspunkte*4];

  if (bnwtgefunden){
    picmodem->readttybuffer(abuffer, maxmesspunkte*4);//PC Buffer maximal auslesen
  }
  for(int i=0;i<maxmesspunkte*4;i++)rxbuffer[i]=0; //rs232buffer loeschen
  idxbufferread=0;
  idxbufferwrite=0;
  qsbefehl="";
  befehlanfang=false;
  befehlende=false;
  if(bd)qDebug("Nwt4Window::clrrxbuffer() ENDE");
}

void Nwt4Window::kalibrierende(){
  if(bd)qDebug("Nwt4Window::kalibrierende()");
  loadMk(grunddaten.filemk);//Messkopf wieder laden
  tabumschalten(tabwidget->currentIndex());//TAB Einstellungen wieder vornehmen
  wobbelgrunddaten=wgrunddatenmerk; //alte Wobbelgrunddaten zurueck
  testFrqMinMax();//Ueberpruefung der Frequenzen
  setSpeki(wobbelgrunddaten.bdBm);//alte Einstellung Speki EIN/AUS
  messkurve.calablauf=0;//ENDE Kalibrierung
  messkurveClr();//Messkurve loeschen
  WobbelGrundDatenToEdit();//Frq-Eingabe aktualisieren
  WobbelGrundDatenToDisplay();//Display aktualisieren
  display->setMessKurve(messkurve);//Messkurve setzen
  if(bd)qDebug("Nwt4Window::kalibrierende() ENDE");
}

void Nwt4Window::befantworttimerstop(){
  if(bd)qDebug("Nwt4Window::befantworttimerstop()");
  int  i, j;
  qint64 a;
  unsigned char abuffer[maxmesspunkte*4];
  bool rs232stop=false;
  QString qs;
  double frq;
  double mkx=0.0, mky=0.0, mkxmw=0.0, mkymw=0.0;
  unsigned char c1, c2;
  double swvcaldb;

  befantworttimer->stop();
  if (bnwtgefunden){
    a = picmodem->readttybuffer(abuffer, maxmesspunkte*4);
    if(a > 0){
      for(i=0, j=idxbufferwrite; i<a; i++, j++){
        rxbuffer[j] = abuffer[i]; //Die gelesenen Daten in den richtigen Buffer kopieren
        //qDebug("abuffer[i]:%i",abuffer[i]);
      }
      idxbufferwrite = idxbufferwrite + a; // und den Index auf die richtige Stelle setzen
      //qDebug("a:%i",a);
    }
    //qDebug("idxbufferwrite:%i, idxbufferread:%i, a:%i",idxbufferwrite,idxbufferread,a);
    //Wobbelbetrieb
    if(bbefwobbeln){
      if(a>0){//Es sind Daten gekommen
        synclevel=0;
        abbruchlevel=0;
      }else{//Es sind keine Daten gekommen
        synclevel++;
        if(synclevel > 20){//20 x keine Daten
          abbruchlevel++;
          if(abbruchlevel > 3){//3 x 20 keine Daten
            QMessageBox::warning( this, tr("USB Schnittstelle","InformationsBox"),
                                  tr("Es kommen keine Daten vom NWT!","InformationsBox"));
            rs232stop=true;
            clrrxbuffer();//alle Bufferdaten loeschen
            set0hz();
            buttonstop->setEnabled(false);
            buttonwobbeln->setEnabled(true);
            buttoneinzeln->setEnabled(true);
            grwobbel->setEnabled(true);
            grzusatzlinien->setEnabled(true);
            menuMesskopf->setEnabled(true);
            menuKurven->setEnabled(true);
            display->setEnabled(true);
            widgetwkm->setEnabled(true);
            rlauf->setValue(0);
          }else{// Abbruchlevel <= 3
            if(messkurve.calablauf > 0){// Kalibrieren
              QMessageBox::warning( this, tr("USB Schnittstelle","InformationsBox"),
                                    tr("Es kam zu Datenverlusten. Bitte neu kalibrieren!","InformationsBox"));
              kalibrierende();
              rs232stop=true;
              clrrxbuffer();//alle Bufferdaten loeschen
              set0hz();
              buttonstop->setEnabled(false);
              buttonwobbeln->setEnabled(true);
              buttoneinzeln->setEnabled(true);
              grwobbel->setEnabled(true);
              grzusatzlinien->setEnabled(true);
              menuMesskopf->setEnabled(true);
              menuKurven->setEnabled(true);
              display->setEnabled(true);
              widgetwkm->setEnabled(true);
              rlauf->setValue(0);
            }else{// kein Kalibrieren
              synclevel=0;
              sendbefwobbeln();// Wobbeln neu anschieben
            }
          }
        }
      }
      while(idxbufferwrite-idxbufferread >=4){
        c1=rxbuffer[idxbufferread];
        idxbufferread++;
        c2=rxbuffer[idxbufferread];
        //qDebug("c1:%i, c2:%i",c1,c2);
        adckanal=c1+c2*256;
        //qDebug("adckanal:%i",adckanal);
        idxbufferread++;
        c1=rxbuffer[idxbufferread];
        idxbufferread++;// Messwerte Kanal 2 ueberspringen
        c2=rxbuffer[idxbufferread];
        idxbufferread++;// Messwerte Kanal 2 ueberspringen
        //kanal2=c1+c2*256;
        frq=wobbelgrunddaten.frequenzanfang+wobbelgrunddaten.frequenzschritt*idxmesskurve;//Frequenz errechnen
        messkurve.frequenz[idxmesskurve]= frq; //frequenz in die Messkurve speichern
        //qDebug("ADCKanal:%i,Frequenz:%f",adckanal,frq);
        swvcaldb=0.0;
        if(messkurve.calablauf==0 or messkurve.calablauf==3){
          //S11, Return Loss Messung
          if(messkurve.calablauf==3 and (messkopf.MkTyp()==mks11 or messkopf.MkTyp()==mks11var)){
            switch (messkopf.getS11cal()) {
            case vref25:
            case vref100:
              swvcaldb=-9.542;//Pegel Messwert1 mit 100Ohm oder 25Ohm Abschluss swv2.0
              break;
            case vref75:
              swvcaldb=-13.979;//Pegel Messwert1 mit 75Ohm swv Abschluss swv1.5
              break;
            case vrefopen:
              swvcaldb=0.0;//Pegel Messwert1 mit Messbruecke offen
              break;
            default:
              break;
            }
          }
          if(wobbelgrunddaten.bdBm){// S21, Spektrumanalyser
            messkurve.dbk1[idxmesskurve]=messkopf.adc2Pegel(adckanal, 0.0) - swvcaldb;//ADC wandeln in dB ohne Frequenzgang, swvcaldb ist normal 0dB
            //messkurve.dbk1[idxmesskurve]=messkopf.adc2Pegel(adckanal, frq) - swvcaldb;//ADC wandeln in dB ohne Frequenzgang, swvcaldb ist normal 0dB
            messkurve.dbk1[idxmesskurve]+=messkopf.CalKorrdBmPegel();//umrechenen von dB in dBm
          }else{//S21, dB Kurve
            messkurve.dbk1[idxmesskurve]=messkopf.adc2Pegel(adckanal, frq) - swvcaldb;//ADC wandeln in dB, swvcaldb ist normal 0dB
          }
        }else{
          //calablauf == 1 oder calablauf == 2; Kalibrieren
          //Kalibrieren mit Befehl a oder x Wobbeln
          if(catbefehl=='a' or catbefehl=='x'){
            messkurve.dbk1[idxmesskurve]=adckanal;
          }
        }
        idxmesskurve++;//naechster Index
        rlauf->setValue(idxmesskurve);
      }
      messkurve.calindex=idxmesskurve-1;//für Anzeige beim Kalibrieren
      display->setMessKurve(messkurve);//Im Display anzeigen
      //Ende der Wobbelkurve erreicht
      if(idxmesskurve==messkurve.mpunkte){
        setInfoText();
        messkurve.bkurvegueltig=true;
        buttonh1->setEnabled(true);
        buttonh2->setEnabled(true);
        buttonh3->setEnabled(true);
        buttonh4->setEnabled(true);
        buttonh5->setEnabled(true);
        messkurveSetSpur();
        display->setMessKurve(messkurve);
        if(!wobbeinmal){
          sendbefwobbeln();
        }else{
          switch (messkurve.calablauf) {
          //Kalibrien Schritt 1 Auswertung
          case 1:
            if(catbefehl=='a' or catbefehl=='x'){
              ymkwert1=0.0;
              for(i=0;i<wobbelgrunddaten.mpunkte;i++){
                ymkwert1+=messkurve.dbk1[i];
                //qDebug()<<"messkurve.dbk1[i]:"<<messkurve.dbk1[i];
              }
              ymkwert1=ymkwert1/(wobbelgrunddaten.mpunkte);
              caldaten.adc1=ymkwert1;
              qDebug("ymkwert1:%f",ymkwert1);

              pegel2=0.0;
              ymkwert1mw=ymkwert1;// gleich setzen
              wobbelgrunddaten.frequenzanfang = frqcalpunkt1 - 0.5;//HF Messkopf Kalibrierung Frequenzpunkt1 500kHz unterhalb
              wobbelgrunddaten.frequenzschritt=0.01;//10kHz
              wobbelgrunddaten.mpunkte=100;//nur Messpunkte fuer calX und calY
              wobbelgrunddaten.ztime=100;//Zwischenzeit 10mSek
              wobbeinmal=true;//nur einmaliger Durchlauf
              switch(messkopf.MkTyp()){
              case mks21:
              case mks21var:
                QMessageBox::information( this, tr("Kalibrieren S21","in Dialogbox"),
                                          tr("<center><B>Bitte <u>Daempfungsglied %1dB</u> entfernen und<br>"
                                             "RFout mit RFin direkt verbinden!!</B><br>"
                                             "-------------------------------------------<br>"
                                             "Kalibrierung fortsetzen!</center>","in Dialogbox").arg(pegel1*-1.0));
                break;
              case mks11:
              case mks11var:
                QMessageBox::information( this, tr("Kalibrieren S11","in Dialogbox"),
                                          tr("<center><B>Bitte <u>Daempfungsglied %1dB</u> entfernen und<br>"
                                             "RFin mit SWV-Messbruecke direkt verbinden!!</B><br>"
                                             "-------------------------------------------<br>"
                                             "Kalibrierung fortsetzen!</center>","in Dialogbox").arg(pegel1*-1.0));
                break;
              default:
                break;
              }
              messkurve.calablauf=2;
              delay(500);
              clrrxbuffer();
              sendbefwobbeln();
            }
            break;
          case 2:
            // Kalibrierung Schritt 2 Auswertung
            if(catbefehl=='a' or catbefehl=='x'){
              ymkwert2=0.0;
              //aufaddieren aller Messwerte
              for(i=0;i<wobbelgrunddaten.mpunkte;i++){
                ymkwert2+=messkurve.dbk1[i];
                //qDebug()<<"messkurve.dbk1[i]:"<<messkurve.dbk1[i];
              }
              //und Mittelwert bilden
              ymkwert2=ymkwert2/(wobbelgrunddaten.mpunkte);
              caldaten.adc2=ymkwert2;
              qDebug("ymkwert2:%f",ymkwert2);
              ymkwert2mw=ymkwert2;//gleich setzen
              caldaten.adcmw2=ymkwert2mw;
              qDebug("ymkwert1mw=%f",ymkwert1mw);
              qDebug("ymkwert2mw=%f",ymkwert2mw);
              qDebug("ymkwert2:%f",ymkwert2);
              qDebug("ymkwert1:%f",ymkwert1);
              if(messkopf.MkTyp()==mks11 or messkopf.MkTyp()==mks11var){//nur mit swv Messkopf
                //log swv Messkopf
                swvcaldb=0.0;
                switch (messkopf.getS11cal()) {
                case vref25:
                case vref100:
                  swvcaldb=-9.542;//Pegel Messwert1 mit 100Ohm oder 25Ohm Abschluss swv2.0
                  break;
                case vref75:
                  swvcaldb=-13.979;//Pegel Messwert1 mit 75Ohm swv Abschluss swv1.5
                  break;
                case vrefopen:
                  swvcaldb=0.0;//Pegel Messwert1 mit Messbruecke offen
                  break;
                default:
                  break;
                }
                mkx=(pegel1*-1.0)/(ymkwert2-ymkwert1);
                mky=(ymkwert2 * mkx * -1.0)+ swvcaldb;//Pegel Messwert1 mit 100Ohm swv Abschluss swv2.0
                if(Mkneu){
                  mkxmw=(pegel1*-1.0)/(ymkwert2mw-ymkwert1mw);
                  mkymw=(ymkwert2mw * mkx * -1.0)+ swvcaldb;//Pegel Messwert1 mit 100Ohm swv Abschluss swv2.0
                }
              }else{
                //log HF und NF
                //qDebug()<<"pegel1:"<<pegel1;
                //qDebug()<<"pegel2:"<<pegel2;
                //qDebug()<<"ymkwert2:"<<ymkwert2;
                //qDebug()<<"ymkwert1:"<<ymkwert1;
                mkx=(pegel1 - pegel2)/(ymkwert1 - ymkwert2);
                mky=(ymkwert1 * mkx * -1.0) + pegel1;
                if(Mkneu){
                  mkxmw=(pegel1 - pegel2)/(ymkwert1mw - ymkwert2mw);
                  mkymw=(ymkwert1mw * mkx * -1.0) + pegel1;
                }
                //qDebug()<<"mkx:"<<mkx;
                //qDebug()<<"mky:"<<mky;
              }
              messkopf.setMkXY(mkx,mky);//neu errechnete Werte im Messkopf speichern
              if(Mkneu){
                messkopf.setMkXYmWMeter(mkxmw,mkymw);//neu errechnete Werte mW-Meter im Messkopf speichern
              }
              //qDebug("Messkopf: mkx:%f, mky:%f",mkx,mky);
              //qDebug("Messkopf: mkxmw:%f, mkymw:%f",mkxmw,mkymw);
              wobbelgrunddaten.frequenzanfang = messkopf.CalFrqmin();//unterste Frequenz
              wobbelgrunddaten.frequenzschritt = messkopf.CalFrqSchritt();// errechnete Frequenzschritte
              wobbelgrunddaten.mpunkte=maxmesspunkte;//
              wobbelgrunddaten.ztime=100;//Zwischenzeit
              messkurve.calablauf=3;
              wobbeinmal=true;//nur einmaliger Durchlauf
              delay(500);
              clrrxbuffer();
              sendbefwobbeln();
            }
            break;
          case 3:
            //Kalibrierung Schritt 3 Auswertung
            //qDebug("Ablauf3");
            messkopf.setCalPegel(messkurve);
            display->setMessKurve(messkurve);
            speichernMesskopf();
            kalibrierende();
            //SWR-Mk im Display SWR anzeigen aktivieren
            if((messkopf.MkTyp()==mks11) or (messkopf.MkTyp()==mks11var)){
              checkboxswvkanal_checked(true);
              checkboxswvkanal->setChecked(true);
              if(Mkneu)mNoFrqCalK1->setChecked(true);//S11 Freq.gang.korr AUS
            }else{
              checkboxswvkanal_checked(false);
              checkboxswvkanal->setChecked(false);
              if(Mkneu)mNoFrqCalK1->setChecked(false);//S21 Freq.gang.korr EIN
            }
            Mkneu=false;
            break;
          case 0:
            break;
          default:
            break;
          }
          if(messkurve.calablauf==0){
            rs232stop=true;
            clrrxbuffer();//alle Bufferdaten loeschen
            set0hz();
            buttonstop->setEnabled(false);
            buttonwobbeln->setEnabled(true);
            buttoneinzeln->setEnabled(true);
            grwobbel->setEnabled(true);
            grzusatzlinien->setEnabled(true);
            menuMesskopf->setEnabled(true);
            menuKurven->setEnabled(true);
            display->setEnabled(true);
            widgetwkm->setEnabled(true);
          }
        }
      }
    //kein Wobbeln
    }else{
      //Befehl m, auslesen der ADC-Werte fuer das mW-Meter
      if((catbefehl=='m') and (idxbufferwrite-idxbufferread >=4)){
        catbefehl=' ';
        //qDebug("diff:%i",idxbufferwrite-idxbufferread);
        //ADC-Wert Kanal1 auslesen
        c1=rxbuffer[idxbufferread];
        idxbufferread++;
        c2=rxbuffer[idxbufferread];
        adckanal=c1+c2*256;
        //qDebug("c1:%i, c2:%i",c1,c2);
        //qDebug()<<adckanal;
        idxbufferread++;
        idxbufferread++;//dummy lesen Kanal 2
        idxbufferread++;
        //ReturnLoss benötigt Frequenz vom DDS oder VCO
        double frq = getMessVfoFrq() / 1000000.0;
        //qDebug("frq=%f",frq);
        //Messergebnis aus ADC berechnen
        if((messkopf.MkTyp() == mks11) or (messkopf.MkTyp() == mks11var)){
          mdbmK1=messkopf.adc2mWMeter(adckanal, frq);//bei SWV Frequenzgang mit einrechnen
          //mdbmK1=messkopf.adc2mWMeter(adckanal, 0.0);//bei SWV keinen Frequenzgang
        }else{
          mdbmK1=messkopf.adc2mWMeter(adckanal, 0.0);//bei S21 keinen Frequenzgang
          //qDebug("mdbmK1:%f",mdbmK1);
          //mdbmK1+=messkopf.CalKorrdBmPegel();
        }
        //qDebug("mdbmK1:%f",mdbmK1);
        //mdbmK1=mdbmK1+offsetk1;//dB Offset am mW-Meter mit einrechnen
        //qDebug("mdbmK1:%f",mdbmK1);
        setMessLabel1();//alles im VFO/mW-Meter anzeigen
        clrrxbuffer();//alle Bufferdaten loeschen
        if(bmessenmw){//mW-Meter Modus, Messung neu starten
          wachtimer->stop();//Datenfluss Ueberwachung STOP
          wachtimer->start(500);//Datenfluss Ueberwachung neu starten 500mSek
          if(bnwtgefunden){
            picmodem->writeChar(0x8F);//zum NWT senden
            picmodem->writeChar('m');//zum NWT senden
            catbefehl='m';//Befehl fuer Auswertung merken
          }
          rs232stop=false;//Dauerhaftes RX der RS232
        }else{
          rs232stop=true;//kein Dauerhaftes RX der RS232
        }
      }
      if(catbefehl==0x60){//Befehl Flatnes HW Kalibrierung
        QMessageBox *msgBox;
        msgBox = new QMessageBox(this);
        QPushButton *ButtonWeiter = msgBox->addButton(tr("Weiter"), QMessageBox::ActionRole);

        //qDebug("a=%i, idxbufferread=%i, idxbufferwrite:%i",a, idxbufferread, idxbufferwrite);
        if(idxbufferwrite != idxbufferread){//RS232 RX String
          while (idxbufferread != idxbufferwrite){//auslesen
            c1=rxbuffer[idxbufferread];//char lesen
            messkurve.calstring.append(c1);//String zusammen setzen
            idxbufferread++;//index++
          }
          cal60counter=0;//RS232 RX lesen counter=0
          //qDebug(qPrintable(messkurve.calstring));
          display->setMessKurve(messkurve);//Im Display den Kalibrierstring anzeigen
        }else{//RS232 RX alles gelesen
          cal60counter++;//Leseversuch++ und keine Zeichen im Buffer
          if(cal60counter==50){//50 Leseversuche ohne Zeichen im Buffer
            cal60counter=0;//Leseversuche Counter=0
            switch(cal60index){//Auswertung des Kalibrierzustandes Flatnes Kalibrierung
            case 0://Erster Schritt
              msgBox->setText(tr("<b>Flatnes Kalibrierung</b>"));
              msgBox->setInformativeText(tr("Input und Output mit einem Koaxkabel verbinden!"));
              msgBox->setDefaultButton(ButtonWeiter);
              msgBox->exec();
              cal60index++;
              if(bnwtgefunden){
                picmodem->writeChar(0x8F);
                picmodem->writeChar(0x60);
              }
              messkurve.calstring = tr("Datenruecklauf -");
              display->setMessKurve(messkurve);//Im Display anzeigen
              break;
            case 1://Zweiter Schritt
              msgBox->setText(tr("<b>Flatnes Kalibrierung</b>"));
              msgBox->setInformativeText(tr("Zwischen Input und Output ein Daempfungsglied 40dB einschleifen!"));
              msgBox->setDefaultButton(ButtonWeiter);
              msgBox->exec();
              cal60index++;
              if(bnwtgefunden){
                picmodem->writeChar(0x8F);
                picmodem->writeChar(0x60);
              }
              messkurve.calstring = tr("Datenruecklauf -");
              display->setMessKurve(messkurve);//Im Display anzeigen
              break;
            case 2://dritter Schritt (Abschluss)
              msgBox->setText(tr("<b>Flatnes Kalibrierung</b>"));
              ButtonWeiter->setText(tr("Ende"));
              msgBox->setInformativeText(tr("Kalibrierung OK, Funktion beendet!"));
              msgBox->setDefaultButton(ButtonWeiter);
              msgBox->exec();
              clrrxbuffer();//RS232 RX Buffer loeschen
              cal60index=0;
              rs232stop=true;
              messkurve.calstring = "";//Kalibrierinfo loeschen
              display->setMessKurve(messkurve);//Im Display anzeigen
              catbefehl=' ';
              grunddaten.bflatcal=true;
              kalibrierende();//Kalibrieren beenden
              break;
            }
          }
        }
        delete ButtonWeiter;
        delete msgBox;
      }
    }
  }
  //qDebug("catbefehl:%i",catbefehl);
  if(!rs232stop)befantworttimer->start();//mit vorheriger Zeit
  if(bd)qDebug("Nwt4Window::befantworttimerstop() ENDE");
}

void Nwt4Window::WobbelGrundDatenToDisplay(){
  if(bd)qDebug("Nwt4Window::WobbelGrundDatenToDisplay()");
  double fende;

  //STOP Frequenz berechnen fuer das Display
  ksettings.setmaxY(wobbelgrunddaten.dbmax); //double
  ksettings.setminY(wobbelgrunddaten.dbmin); //double
  ksettings.setschrittweiteX(wobbelgrunddaten.frequenzschritt); //double
  //qDebug()<<"1)wobbelgrunddaten.frequenzschritt:"<<wobbelgrunddaten.frequenzschritt;
  ksettings.setmpunkteX(wobbelgrunddaten.mpunkte); //integer
  //qDebug()<<"1)wobbelgrunddaten.mpunkte:"<<wobbelgrunddaten.mpunkte;
  ksettings.setnumYTicks(round((wobbelgrunddaten.dbmax-wobbelgrunddaten.dbmin) / 10.0));//integer
  ksettings.setminX(wobbelgrunddaten.frequenzanfang);//double
  //qDebug()<<"1)wobbelgrunddaten.frequenzanfang:"<<wobbelgrunddaten.frequenzanfang;
  fende=wobbelgrunddaten.frequenzanfang+(wobbelgrunddaten.frequenzschritt*(wobbelgrunddaten.mpunkte-1));
  ksettings.setmaxX(fende);//double
  //ksettings.setnumXTicks(10);//int Anzahl der Frequenzmpunkte
  ksettings.setdbline(wobbelgrunddaten.dbline);//waagerechte dBLinie einblenden

  ksettings.setDisplayshift(wobbelgrunddaten.dshift);
  ksettings.setKurvenshift(wobbelgrunddaten.kshift);
  ksettings.setSettingPath(settingspath);
  ksettings.setbdBm(wobbelgrunddaten.bdBm);
  ksettings.setbwatt(wobbelgrunddaten.bwatt);
  ksettings.setbvolt(wobbelgrunddaten.bvolt);
  ksettings.set0dBLine(wobbelgrunddaten.b0dBline);
  ksettings.hwvariante=grunddaten.variante;

  messkurve.mpunkte=wobbelgrunddaten.mpunkte; //Schritte der Messkurve setzen
  display->setPlotSettings(ksettings);// Daten zum Display
  if(bd)qDebug("Nwt4Window::WobbelGrundDatenToDisplay() ENDE");
}

void Nwt4Window::testFrqMinMax(){
  if(bd)qDebug("Nwt4Window::testFrqMinMax()");
  double fanf, fende;
  QString qs;

  fanf=messkopf.CalFrqmin();
  qs = frq2str(fanf,'W');
  //qDebug()<<qs;
  wfrqstr.sstart=qs;//Strings merken für Veränderungen der Werte
  fende=messkopf.CalFrqmax();
  qs = frq2str(fende,'W');
  wfrqstr.sstop=qs;//Strings merken für Veränderungen der Werte
  wobnormalisieren();
  if(bd)qDebug("Nwt4Window::testFrqMinMax() ENDE");
}


void Nwt4Window::normfanfangende(){
  if(bd)qDebug("Nwt4Window::normfanfangende()");
  bool ok;
  QString qs;
  double fanf, fende, fschritt, fspan, fmitte;
  int mpunkte=301, i;
  qlonglong lfende, lfanf, lfschritt, callmin, callmax;

  //Startfrequenz auslesen
  qs=lineEdit_frq_anfang->text();
  fanf=linenormalisieren(qs);
  //qDebug()<<"Startfrequenz1:"<<fanf;
  //wandeln in interger
  lfanf= round(fanf*1000000.0);
  //qDebug()<<"Startfrequenz2:"<<lfanf;
  //Stopfrequenz auslesen
  qs=lineEdit_frq_ende->text();
  fende=linenormalisieren(qs);
  //qDebug()<<"Stopfrequenz:"<<fende;
  //wandeln in integer
  lfende= round(fende*1000000.0);
  //qDebug()<<"Stopfrequenz2:"<<lfende;
  //Anzahl der Schritte auslesen
  qs=lineEdit_frq_mpunkte->text();
  i=qs.toInt(&ok);
  if(ok)mpunkte=i+1;//Messpunkte = Frequenzabschnitte + 1
  // mpunkte nur 0-2001
  if(mpunkte>maxmesspunkte)mpunkte=maxmesspunkte;
  //Korrektur Frequenz Anfang und Ende
  callmin=messkopf.CalFrqmin()*1000000.0;
  callmax=messkopf.CalFrqmax()*1000000.0;
  if(lfende > callmax or lfende < callmin)lfende=callmax;
  if(lfanf < callmin or lfanf > callmax)lfanf=callmin;
  if(lfende < lfanf){
    lfanf=callmin;
    lfende=callmax;
  }
  //nur bei DDS Betrieb
  //aus integer die Schrittweite neu berechnen
  lfschritt=(lfende-lfanf)/(mpunkte-1);
  //qDebug()<<"Schrittweite1:"<<lfschritt;
  //STOP-Frequenz neu aus integer berechnen
  lfende = lfanf + ((mpunkte-1) * lfschritt);
  //Schrittweite aus integer neu berechnen nur wenn kein ADF4351
  lfschritt=(lfende-lfanf)/(mpunkte-1);
  //qDebug()<<"Schrittweite3:"<<lfschritt;
  fschritt = double(lfschritt)/1000000.0;
  //wieder in double wandeln MHz darstellung
  fende = double(lfende)/1000000.0;
  //wieder in double wandeln MHz darstellung
  fanf = double(lfanf)/1000000.0;
  //Span aus den neuen Ergebnissen berechnen
  fspan=fende-fanf;
  //Mitte aus den neuen Ergebnissen berechnen
  fmitte=fanf+fspan/2.0;
  //Alles in Stringdarstellung zurück wandeln
  qs = frq2str(fanf,'W');
  wfrqstr.sstart=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_anfang->setText(qs);
  qs = frq2str(fende,'W');
  wfrqstr.sstop=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_ende->setText(qs);
  fschritt=fspan/(mpunkte-1);
  qs = frq2str(fschritt,'W');
  wfrqstr.sstep=qs;//Strings merken für Veränderungen der Werte
  lineEdit_schrittweite->setText(qs);
  qs.sprintf("%i",mpunkte-1);
  wfrqstr.ssteps=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_mpunkte->setText(qs);
  qs = frq2str(fspan,'W');
  wfrqstr.sspan=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_span->setText(qs);
  qs = frq2str(fmitte,'W');
  wfrqstr.scenter=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_mitte->setText(qs);
  //WobbelGrundaten neu setzen
  wobbelgrunddaten.frequenzanfang = fanf;
  wobbelgrunddaten.frequenzschritt = fschritt;
  wobbelgrunddaten.mpunkte = mpunkte;
  //alles zum Display uebertragen
  korrKurveGeladen();//Frequenzeinstellungen korrigieren wenn Kurvendatei geladen ist
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::normfanfangende() ENDE");
}

void Nwt4Window::normmittespan(){
  if(bd)qDebug("Nwt4Window::normmittespan()");
  bool ok;
  QString qs;
  double fanf, fende, fschritt, fspan, fmitte;
  qlonglong lfende, lfanf, lfschritt, lfspan, lfmitte;
  int mpunkte=301, i;
  bool bwerte_bad=false;

  //Strings auslesen
  qs=lineEdit_frq_mpunkte->text();
  i = qs.toInt(&ok,10);
  if(ok)mpunkte=i+1;
  //if(mpunkte<(wobbelgrunddaten.mpunktemin+1))mpunkte=wobbelgrunddaten.mpunktemin+1;
  if(mpunkte>maxmesspunkte)mpunkte=maxmesspunkte;

  //Startfrequenz auslesen
  qs=lineEdit_frq_anfang->text();
  fanf=linenormalisieren(qs);
  //wandeln in interger
  lfanf= round(fanf*1000000.0);

  //Stopfrequenz auslesen
  qs=lineEdit_frq_ende->text();
  fende=linenormalisieren(qs);
  //qDebug()<<"Stopfrequenz:"<<fende;
  //wandeln in integer
  lfende= round(fende*1000000.0);

  //Mitte auslesen
  qs=lineEdit_frq_mitte->text();
  fmitte=linenormalisieren(qs);
  //wandeln in interger
  lfmitte= round(fmitte*1000000.0);

  //Span auslesen
  qs=lineEdit_frq_span->text();
  fspan=linenormalisieren(qs);
  //if(frqfaktor>1)fspan = round(fspan / double(frqfaktor));
  //wandeln in interger
  lfspan= round(fspan*1000000.0);

  //qDebug()<<"1.fmitte"<<fmitte;
  //qDebug()<<"1.fspan"<<fspan;

  //lfmax = vfos.Vfomax() * 1000000.0 / frqfaktor;
  //qDebug()<<"lfmax"<<lfmax;

  //lfmin = vfos.Vfomin() * 1000000.0 / frqfaktor;
  //qDebug()<<"lfmin"<<lfmin;

  // Mitte und Span ins richtige Verhaeltnis setzen
  if(lfmitte < (lfspan/2)){
    lfspan = lfmitte * 2.0;
  }
  //qDebug()<<"2.fspan"<<fspan;
  //Startfrequenz und Stopfrequenz berechnen
  lfanf=lfmitte - lfspan/2;
  lfende=lfmitte + lfspan/2;

  //Kontrolle der Frequenzbereiche
  if(lfende>(messkopf.CalFrqmax()*1000000.0))lfende=(messkopf.CalFrqmax()*1000000.0);
  if(lfanf<(messkopf.CalFrqmin()*1000000.0))lfanf=(messkopf.CalFrqmin()*1000000.0);

  lfschritt=lfspan/(mpunkte-1);

  //wieder in double wandeln MHz darstellung
  fanf = double(lfanf)/1000000.0;
  //if(frqfaktor>1)fanf = round(fanf * double(frqfaktor));
  fende = double(lfende)/1000000.0;
  //if(frqfaktor>1)fende = round(fende * double(frqfaktor));
  fmitte = double(lfmitte)/1000000.0;
  //if(frqfaktor>1)fmitte = round(fmitte * double(frqfaktor));
  fspan = double(lfspan)/1000000.0;
  //if(frqfaktor>1)fspan = round(fspan * double(frqfaktor));
  fschritt = double(lfschritt)/1000000.0;
  //if(frqfaktor>1)fschritt = round(fschritt * double(frqfaktor));

  //Alles in Stringdarstellung zurück wandeln
  qs = frq2str(fanf,'W');
  wfrqstr.sstart=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_anfang->setText(qs);
  qs = frq2str(fende,'W');
  wfrqstr.sstop=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_ende->setText(qs);
  fschritt=fspan/(mpunkte-1);
  qs = frq2str(fschritt,'W');
  wfrqstr.sstep=qs;//Strings merken für Veränderungen der Werte
  lineEdit_schrittweite->setText(qs);
  qs.sprintf("%i",mpunkte-1);
  wfrqstr.ssteps=qs;
  lineEdit_frq_mpunkte->setText(qs);
  qs = frq2str(fspan,'W');
  wfrqstr.sspan=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_span->setText(qs);
  qs = frq2str(fmitte,'W');
  wfrqstr.scenter=qs;//Strings merken für Veränderungen der Werte
  lineEdit_frq_mitte->setText(qs);

  //START und STOP hat sich geaendert, noch einmal die andere Normalisierung
  if(bwerte_bad)normfanfangende();

  wobbelgrunddaten.frequenzanfang = fanf;
  wobbelgrunddaten.frequenzschritt = fschritt;
  wobbelgrunddaten.mpunkte = mpunkte;

  korrKurveGeladen();//Frequenzeinstellungen korrigieren wenn Kurvendatei geladen ist
  WobbelGrundDatenToDisplay();
  if(bd)qDebug("Nwt4Window::normmittespan() ENDE");
}

double Nwt4Window::linenormalisieren(const QString &line)
{
  if(bd)qDebug("Nwt4Window::linenormalisieren(const QString &line)");
  bool ok;
  double faktor=1.0;
  double ergebnis = 0.0;
  double d=0.0, r=0.0;
  bool bremove=false;

  QString aline(line);

  //qDebug("linenormalisieren");
  //qDebug()<<aline;
  aline = aline.toLower();	//alles auf Kleinschreibung
  if(aline.contains("ghz")){
    aline.remove("ghz");
    d=1000.0;
    bremove=true;
  }
  if(aline.contains("mhz")){
    aline.remove("mhz");
    d=1.0;
    bremove=true;
  }
  if(aline.contains("khz")){
    aline.remove("khz");
    d=0.001;
    bremove=true;
  }
  if(aline.contains("hz")){
    aline.remove("hz");
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
    aline.remove(QChar(' ')); // Leerzeichen entfernen
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
  //qDebug()<<ergebnis;
  //qDebug("linenormalisieren ENDE");
  if(bd)qDebug("Nwt4Window::linenormalisieren(const QString &line) ENDE");
  return ergebnis;
}

void Nwt4Window::WobbelGrundDatenToEdit(){
  if(bd)qDebug("Nwt4Window::WobbelGrundDatenToEdit()");
  QString qs;
  double fende, fmitte, fspan;

  fende = wobbelgrunddaten.frequenzanfang + wobbelgrunddaten.frequenzschritt*(wobbelgrunddaten.mpunkte-1);
  fspan = fende - wobbelgrunddaten.frequenzanfang;
  fmitte = wobbelgrunddaten.frequenzanfang + fspan / 2;


  qs = frq2str(wobbelgrunddaten.frequenzanfang,'W');
  lineEdit_frq_anfang->setText(qs);
  qs = frq2str(fende,'W');
  lineEdit_frq_ende->setText(qs);
  qs.sprintf("%i",(wobbelgrunddaten.mpunkte-1));
  lineEdit_frq_mpunkte->setText(qs);
  qs = frq2str(wobbelgrunddaten.frequenzschritt,'W');
  lineEdit_schrittweite->setText(qs);
  qs = frq2str(fmitte,'W');
  lineEdit_frq_mitte->setText(qs);
  qs = frq2str(fspan,'W');
  lineEdit_frq_span->setText(qs);
  if(bd)qDebug("Nwt4Window::WobbelGrundDatenToEdit() ENDE");
}

QString Nwt4Window::frq2str(double d, char c){
  if(bd)qDebug("Nwt4Window::frq2str(double d, char c)");
  QString me=" GHz";
  QString qs;
  double w=d/1000.0;
  int l;

  if(bd)qDebug()<<d;
  if(c=='W')qs.sprintf("%3.9f",w);
  if(c=='M')qs.sprintf("%3.6f",w);
  if(w<1.0){
    me=" MHz";
    w=w*1000.0;
    if(c=='W')qs.sprintf("%3.6f",w);
    if(c=='M')qs.sprintf("%3.4f",w);
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
  l=qs.length();
  while(qs.at(l-1)=='0'){
    qs=qs.left(l-1);
    l=qs.length();
  }
  if(qs.at(l-1)=='.')qs=qs.left(l-1);
  qs=qs+me;
  if(bd)qDebug("Nwt4Window::frq2str(double d, char c) ENDE");
  return(qs);
}

double Nwt4Window::str2frq(const QString &s){
  if(bd)qDebug("Nwt4Window::str2frq(const QString &s)");
  QString qs=s;
  bool ok;
  double d,r;

  if(bd)qDebug()<<qs;
  qs.remove(" ");
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
  if(ok){
    d=d*r;
  }else{
    d=0.0;
  }
  if(bd)qDebug("Nwt4Window::str2frq(const QString &s) ENDE");
  return d;
}

void Nwt4Window::wobnormalisieren(){
  if(bd)qDebug("Nwt4Window::wobnormalisieren()");
  wobedit2str();
  if(wfrqstr.banfende){
    normfanfangende();
    wobedit2str();
    if(bd)qDebug("Nwt4Window::wobnormalisieren() ENDE1");
    return;
  }
  if(wfrqstr.bspanmitte){
    normmittespan();
    wobedit2str();
    if(bd)qDebug("Nwt4Window::wobnormalisieren() ENDE2");
    return;
  }
}


void Nwt4Window::wobedit2str(){
  if(bd)qDebug("Nwt4Window::wobedit2str()");
  QString qs;

  wfrqstr.banfende=false;
  wfrqstr.bspanmitte=false;
  qs = lineEdit_frq_anfang->text();

  if(qs!=wfrqstr.sstart)wfrqstr.banfende=true;
  qs = lineEdit_frq_ende->text();
  if(qs!=wfrqstr.sstop)wfrqstr.banfende=true;
  qs = lineEdit_frq_mpunkte->text();
  if(qs!=wfrqstr.ssteps)wfrqstr.banfende=true;

  qs = lineEdit_frq_mitte->text();
  if(qs!=wfrqstr.scenter)wfrqstr.bspanmitte=true;
  qs = lineEdit_frq_span->text();
  if(qs!=wfrqstr.sspan)wfrqstr.bspanmitte=true;
  if(bd)qDebug("Nwt4Window::wobedit2str() ENDE");
}

void Nwt4Window::setInfoText(){
  if(bd)qDebug("Nwt4Window::setInfoText()");
  double maxfrq=0.0;
  int idxmaxfrq=0;
  int idxminfrq=0;
  double minfrq=0.0;
  double maxdb=-100.0;

  double maxswv=0.0;
  double maxswvant=0.0;

  double mindb=100.0;
  double minswv=0.0;
  double minswvant=0.0;

  double fdb3_1=0.0;
  double fdb3_2=0.0;
  double fdb6_1=0.0;
  double fdb6_2=0.0;
  double fdb60_1=0.0;
  double fdb60_2=0.0;
  double mwatt;
  double volt;
  bool b3db=false, b6db=false, b60db=false;
  QString qs,qs1,qs2,qs3,qs4,qs5,qsm,qsdbm;
  bool bkanal=false;
  QString qskanal, a, b, c;
  int i;
  bool stop;
  double d;

  display->clrMarkerStrList();//Stringliste loeschen

  //######################################################################
  //Kanal 1
  //######################################################################
  for(int i=0; i<messkurve.mpunkte; i++){
    //Maximalwerte Kanal1 ermitten
    if(messkurve.mktyp==mks11 or messkurve.mktyp==mks11var){
      d=messkurve.dbk1[i];
    }else{
      d=messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift();
    }
    if(maxdb<d){
      maxdb=d;
      maxswv=db2swv(d);
      if(messkurve.adaempfung > 0.0){
        maxswvant=db2swv(d+messkurve.adaempfung*2.0);
      }else{
        maxswvant=0.0;
      }
      maxfrq=messkurve.frequenz[i];
      idxmaxfrq=i;
    }
    //Minimalwerte Kanal1 ermitten
    if(messkurve.mktyp==mks11 or messkurve.mktyp==mks11var){
      d=messkurve.dbk1[i];
    }else{
      d=messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift();
    }
    if(mindb>d){
      mindb=d;
      minswv=db2swv(d);
      if(messkurve.adaempfung > 0.0){
        minswvant=db2swv(d+messkurve.adaempfung*2.0);
      }else{
        minswvant=0.0;
      }
      minfrq=messkurve.frequenz[i];
      idxminfrq=i;
    }
  }
  //##################################
  // 3dB Bandbreite Kanal 1 ermitteln
  //##################################
  messkurve.b3db=false;
  //i=idxmaxfrq;
  if(messkurve.b3dbinv){
    stop=false;
    //qDebug()<<"idxminfrq:"<<idxminfrq;
    //qDebug()<<"mindb:"<<mindb;
    for(i=idxminfrq;i>=0;i--){
      if((mindb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))<=-3.0){
        fdb3_1=messkurve.frequenz[i];
        stop=true;
      }
      if(stop)break;
    }
  }else{
    stop=false;
    for(i=idxmaxfrq;i>=0;i--){
      if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=3.0){
        fdb3_1=messkurve.frequenz[i];
        stop=true;
      }
      if(stop)break;
    }
  }
  //qDebug("db31_1: %f",fdb3_1);
  //i=idxmaxfrq;
  if(messkurve.b3dbinv){
    stop=false;
    for(i=idxminfrq;i<messkurve.mpunkte;i++){
      if((mindb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))<=-3.0){
        fdb3_2=messkurve.frequenz[i];
        stop=true;
      }
      if(stop)break;
    }
  }else{
    stop=false;
    for(i=idxmaxfrq;i<messkurve.mpunkte;i++){
      if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=3.0){
        fdb3_2=messkurve.frequenz[i];
        stop=true;
      }
      if(stop)break;
    }
  }
  if(fdb3_1!=0.0 and fdb3_2!=0.0){
    b3db=true;
    messkurve.frq3db1=fdb3_1;
    messkurve.frq3db2=fdb3_2;
  }
  //qDebug("db31_2: %f",fdb3_2);
  //##########################
  // 6dB Bandbreite ermitteln
  //##########################
  messkurve.b6db=false;
  i=idxmaxfrq;
  stop=false;
  for(i=idxmaxfrq;i>=0;i--){
    if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=6.0){
      fdb6_1=messkurve.frequenz[i];
      stop=true;
    }
    if(stop)break;
  }
  //qDebug("db6_1: %f",db6_1);
  i=idxmaxfrq;
  stop=false;
  for(i=idxmaxfrq;i<messkurve.mpunkte;i++){
    if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=6.0){
      fdb6_2=messkurve.frequenz[i];
      stop=true;
    }
    if(stop)break;
  }
  if(fdb6_1!=0.0 and fdb6_2!=0.0){
    b6db=true;
    messkurve.frq6db1=fdb6_1;
    messkurve.frq6db2=fdb6_2;
  }
  //qDebug("db6_2: %f",db6_2);
  //##########################
  // 60dB Bandbreite ermitteln
  //##########################
  messkurve.b60db=false;
  i=idxmaxfrq;
  stop=false;
  for(i=idxmaxfrq;i>=0;i--){
    if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=60.0){
      fdb60_1=messkurve.frequenz[i];
      stop=true;
    }
    if(stop)break;
  }
  //qDebug("db60_1: %f",db60_1);
  i=idxmaxfrq;
  stop=false;
  for(i=idxmaxfrq;i<messkurve.mpunkte;i++){
    if((maxdb-(messkurve.dbk1[i]+ksettings.Displayshift()+ksettings.Kurvenshift()))>=60.0){
      fdb60_2=messkurve.frequenz[i];
      stop=true;
    }
    if(stop)break;
  }
  if(fdb60_1!=0.0 and fdb60_2!=0.0){
    b60db=true;
    messkurve.frq60db1=fdb60_1;
    messkurve.frq60db2=fdb60_2;
  }
  //qDebug("db60_2: %f",db60_2);
  //##########################
  // Strings erzeugen
  //##########################
  if(checkboxkanal->isChecked()){
    qs1 = frq2str(maxfrq,'M');
    qs2 = frq2str(minfrq,'M');


    if(messkurve.bdbmax and messkurve.bdbmin){
      if(messkurve.bswv){//SWV aktiv
        if(maxswv<1.1){//< 1.1
          if(maxswvant>0.0){//mit Kabeldaempfung
            a.sprintf("%3.2f", maxdb);
            b.sprintf("%2.3f", maxswv);
            c.sprintf("%2.3f", maxswvant);
            qsm=tr("Max: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs1).arg(a).arg(b).arg(c);
          }else{//ohne Kabeldaempfung
            a.sprintf("%3.2f", maxdb);
            b.sprintf("%2.3f", maxswv);
            qsm=tr("Max: %1 [%2 dB, SWV:%3]").arg(qs1).arg(a).arg(b);
          }
        }else{//>= 1.1
          if(maxswvant>0.0){//mit Kabeldaempfung
            a.sprintf("%3.2f", maxdb);
            b.sprintf("%2.2f", maxswv);
            c.sprintf("%2.2f", maxswvant);
            qsm=tr("Max: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs1).arg(a).arg(b).arg(c);
          }else{//ohne Kabeldaempfung
            a.sprintf("%3.2f", maxdb);
            b.sprintf("%2.2f", maxswv);
            qsm=tr("Max: %1 [%2 dB, SWV:%3]").arg(qs1).arg(a).arg(b);
          }
        }
        if(minswv<1.1){//< 1.1
          if(minswvant>0.0){//mit Kabeldaempfung
            a.sprintf("%3.2f", mindb);
            b.sprintf("%2.3f", minswv);
            c.sprintf("%2.3f", minswvant);
            qsm+=tr(", min: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs2).arg(a).arg(b).arg(c);
          }else{//ohne Kabeldaempfung
            a.sprintf("%3.2f", mindb);
            b.sprintf("%2.3f", minswv);
            qsm+=tr(", min: %1 [%2 dB, SWV:%3]").arg(qs2).arg(a).arg(b);
          }
        }else{//>= 1.1
          if(minswvant>0.0){//mit Kabeldaempfung
            a.sprintf("%3.2f", mindb);
            b.sprintf("%2.2f", minswv);
            c.sprintf("%2.2f", minswvant);
            qsm+=tr(", min: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs2).arg(a).arg(b).arg(c);
          }else{//ohne Kabeldaempfung
            a.sprintf("%3.2f", mindb);
            b.sprintf("%2.2f", minswv);
            qsm+=tr(", min: %1 [%2 dB, SWV:%3]").arg(qs2).arg(a).arg(b);
          }
        }
      }else{//ohne SWV
//...................................................................
        a.sprintf("%3.2f", maxdb);
        if(wobbelgrunddaten.bdBm){
          qsm=tr("Max: %1 [%2 dBm").arg(qs1).arg(a);
          mwatt = pow(10.0 , maxdb/10.0);
          volt = sqrt(mwatt * 0.05);
          if(wobbelgrunddaten.bwatt){
            b=mwatt2str(mwatt);
            qsm=qsm + tr(", %1").arg(b);
          }
          if(wobbelgrunddaten.bvolt){
            b=volt2str(volt);
            qsm=qsm + tr(", %1").arg(b);
          }
        }else{
          qsm=tr("Max: %1 [%2 dB").arg(qs1).arg(a);
        }
        qsm=qsm+"]";
        a.sprintf("%3.2f", mindb);
        if(wobbelgrunddaten.bdBm){
          qsm=qsm+tr(", min: %1 [%2 dBm").arg(qs2).arg(a);
          mwatt = pow(10.0 , mindb/10.0);
          volt = sqrt(mwatt * 0.05);
          if(wobbelgrunddaten.bwatt){
            b=mwatt2str(mwatt);
            qsm=qsm+tr(", %1").arg(b);
          }
          if(wobbelgrunddaten.bvolt){
            b=volt2str(volt);
            qsm=qsm + tr(", %1").arg(b);
          }
        }else{
          qsm=qsm+tr(", min: %1 [%2 dB").arg(qs2).arg(a);
        }
        qsm=qsm+"]";
      }
      bkanal=true;
      display->addMarkerStrList(qsm);
//...................................................................
    }else{
      if(messkurve.bdbmax){
        if(messkurve.bswv){
          if(maxswv<1.1){
            if(maxswvant>0.0){
              a.sprintf("%3.2f", maxdb);
              b.sprintf("%2.3f", maxswv);
              c.sprintf("%2.3f", maxswvant);
              qsm=tr("Max: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs1).arg(a).arg(b).arg(c);
            }else{
              a.sprintf("%3.2f", maxdb);
              b.sprintf("%2.3f", maxswv);
              qsm=tr("Max: %1 [%2 dB, SWV:%3]").arg(qs1).arg(a).arg(b);
            }
          }else{
            if(maxswvant>0.0){
              a.sprintf("%3.2f", maxdb);
              b.sprintf("%2.2f", maxswv);
              c.sprintf("%2.2f", maxswvant);
              qsm=tr("Max: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs1).arg(a).arg(b).arg(c);
            }else{
              a.sprintf("%3.2f", maxdb);
              b.sprintf("%2.2f", maxswv);
              qsm=tr("Max: %1 [%2 dB, SWV:%3]").arg(qs1).arg(a).arg(b);
            }
          }
        }else{
          a.sprintf("%3.2f", maxdb);
          if(wobbelgrunddaten.bdBm){
            qsm=tr("Max: %1 [%2 dBm").arg(qs1).arg(a);
            mwatt = pow(10.0 , maxdb/10.0);
            volt = sqrt(mwatt * 0.05);
            if(wobbelgrunddaten.bwatt){
              b=mwatt2str(mwatt);
              qsm=qsm+tr(", %1").arg(b);
            }
            if(wobbelgrunddaten.bvolt){
              b=volt2str(volt);
              qsm=qsm + tr(", %1").arg(b);
            }
          }else{
            qsm=tr("Max: %1 [%2 dB").arg(qs1).arg(a);
          }
          qsm=qsm+"]";
        }
        bkanal=true;
        display->addMarkerStrList(qsm);
      }
      if(messkurve.bdbmin){
        if(messkurve.bswv){
          if(minswv<1.1){
            if(minswvant>0.0){
              a.sprintf("%3.2f", mindb);
              b.sprintf("%2.3f", minswv);
              c.sprintf("%2.3f", minswvant);
              qsm=tr("Min: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs2).arg(a).arg(b).arg(c);
            }else{
              a.sprintf("%3.2f", mindb);
              b.sprintf("%2.3f", minswv);
              qsm=tr("Min: %1 [%2 dB, SWV:%3]").arg(qs2).arg(a).arg(b);
            }
          }else{
            if(minswvant>0.0){
              a.sprintf("%3.2f", mindb);
              b.sprintf("%2.2f", minswv);
              c.sprintf("%2.2f", minswvant);
              qsm=tr("Min: %1 [%2 dB, SWV:%3, SWVant:%4]").arg(qs2).arg(a).arg(b).arg(c);
            }else{
              a.sprintf("%3.2f", mindb);
              b.sprintf("%2.2f", minswv);
              qsm=tr("Min: %1 [%2 dB, SWV:%3]").arg(qs2).arg(a).arg(b);
            }
          }
        }else{
          a.sprintf("%3.2f", mindb);
          if(wobbelgrunddaten.bdBm){
            qsm=tr("Min: %1 [%2 dBm").arg(qs2).arg(a);
            mwatt = pow(10.0 , mindb/10.0);
            volt = sqrt(mwatt * 0.05);
            if(wobbelgrunddaten.bwatt){
              b=mwatt2str(mwatt);
              qsm=qsm+tr(", %1").arg(b);
            }
            if(wobbelgrunddaten.bvolt){
              b=volt2str(volt);
              qsm=qsm + tr(", %1").arg(b);
            }
          }else{
            qsm=tr("Min: %1 [%2 dB").arg(qs2).arg(a);
          }
          qsm=qsm+"]";
        }
        bkanal=true;
        display->addMarkerStrList(qsm);
      }
    }
    qs2=frq2str(fdb3_1,'M');
    qs3=frq2str(fdb3_2,'M');
    qs4=frq2str(fdb3_2-fdb3_1,'M');
    a.sprintf("%1.2f",((fdb3_1 + fdb3_2)/2.0)/(fdb3_2-fdb3_1));
    qs5=tr("Guete Q:%1").arg(a);
    qsdbm=frq2str((fdb3_1+fdb3_2)/2.0,'M');
    qsm="B3dB: "+qs4+"; fm: "+qsdbm+"; f1: "+qs2+"; f2: "+qs3;
    if(b3db and checkbox3db->isChecked()){
      if(!bkanal){
        bkanal=true;qskanal=tr("");
      }else{
        qskanal=" + ";
      }
      qsm=qskanal+qsm;
      messkurve.b3db=true;
      display->addMarkerStrList(qsm);
    }else{
      messkurve.b3db=false;
    }
    if(b3db and checkboxguete->isChecked()){
      messkurve.bguete=true;
      if(!bkanal){
        bkanal=true;qskanal=tr("");
      }else{
        qskanal=" + ";
      }
      qs5=qskanal+qs5;
      display->addMarkerStrList(qs5);
    }else{
      messkurve.bguete=false;
    }
    qs2=frq2str(fdb6_1,'M');
    qs3=frq2str(fdb6_2,'M');
    qs4=frq2str(fdb6_2-fdb6_1,'M');
    qsdbm=frq2str((fdb6_1+fdb6_2)/2.0,'M');
    qsm="B6dB: "+qs4+"; fm: "+qsdbm+"; f1: "+qs2+"; f2: "+qs3;
    if(b6db and checkbox6db->isChecked()){
      messkurve.b6db=true;
      if(!bkanal){
        bkanal=true;qskanal=tr("");
      }else{
        qskanal=" + ";
      }
      qsm=qskanal+qsm;
      display->addMarkerStrList(qsm);
    }else{
      messkurve.b6db=false;
    }
    qs2=frq2str(fdb60_1,'M');
    qs3=frq2str(fdb60_2,'M');
    qs4=frq2str(fdb60_2-fdb60_1,'M');
    qsdbm=frq2str((fdb60_1+fdb60_2)/2.0,'M');
    qsm="B60dB: "+qs4+"; fm: "+qsdbm+"; f1: "+qs2+"; f2: "+qs3;
    if(b60db and checkbox60db->isChecked()){
      messkurve.b60db=true;
      if(!bkanal){
        bkanal=true;qskanal=tr("");
      }else{
        qskanal=" + ";
      }
      qsm=qskanal+qsm;
      display->addMarkerStrList(qsm);
    }else{
      messkurve.b60db=false;
    }
    a.sprintf("%1.2f",(fdb60_2-fdb60_1)/(fdb6_2-fdb6_1));
    qsm=tr("Shape-Faktor: %1").arg(a);
    if(b60db and checkboxshape->isChecked()){
      messkurve.bshape=true;
      if(!bkanal){
        bkanal=true;qskanal=tr("");
      }else{
        qskanal=" + ";
      }
      qsm=qskanal+qsm;
      display->addMarkerStrList(qsm);
    }else{
      messkurve.bshape=false;
    }
  }
  if(bd)qDebug("Nwt4Window::setInfoText() ENDE");
}

void Nwt4Window::frqanfangdisplay2edit(double d){
  if(bd)qDebug("Nwt4Window::frqanfangdisplay2edit(double d)");
  QString qs = frq2str(d,'W');
  lineEdit_frq_anfang->setText(qs);
  if(bd)qDebug("Nwt4Window::frqanfangdisplay2edit(double d) ENDE");
}

void Nwt4Window::frqendedisplay2edit(double d){
  if(bd)qDebug("Nwt4Window::frqendedisplay2edit(double d)");
  QString qs = frq2str(d,'W');
  lineEdit_frq_ende->setText(qs);
  if(bd)qDebug("Nwt4Window::frqendedisplay2edit(double d) ENDE");
}

void Nwt4Window::frq2berechnung(double d){
  if(bd)qDebug("Nwt4Window::frq2berechnung(double d)");
  QString qs = frq2str(d,'W');
  qs.remove("MHz");
  //qDebug()<<qs;
  editf1->setText(qs);
  //editf2->setText(qs);
  editzlc3->setText(qs);
  if(bd)qDebug("Nwt4Window::frq2berechnung(double d) ENDE");
}

void Nwt4Window::frqmittedisplay2edit(double d){
  if(bd)qDebug("Nwt4Window::frqmittedisplay2edit(double d)");
  QString qs = frq2str(d,'W');
  lineEdit_frq_mitte->setText(qs);
  normmittespan();
  if(bd)qDebug("Nwt4Window::frqmittedisplay2edit(double d) ENDE");
}

void Nwt4Window::bildspeichern(){
  if(bd)qDebug("Nwt4Window::bildspeichern()");
  QString qs;
  if(messkurve.caption.isEmpty()){
    qs=datetime2filename();
  }else{
    qs=messkurve.caption;
  }
  qs=fnamenormal(qs);
  emit dbildspeichern(qs);
  if(bd)qDebug("Nwt4Window::bildspeichern() ENDE");
}

void Nwt4Window::sondeNeu(){
  if(bd)qDebug("Nwt4Window::sondeNeu()");
  bool ok;
  double frqcal;

  calibdlg *dlg = new calibdlg(this);
  QString beschrvcomin(tr("<center><u><b>Auswahl der minimalen Kalibrierfrequenz</b></u><br>"
                            "Bitte die minimale Frequenz eingeben<br>"
                            "------------------------------------</center>"
                            "Frequenz in (MHz)","Inputdialog in der Grafik"));

  QString beschrvcomax(tr("<center><u><b>Auswahl der maximalen Kalibrierfrequenz</b></u><br>"
                            "Bitte die maximale Frequenz eingeben<br>"
                            "------------------------------------</center>"
                            "Frequenz in (MHz)","Inputdialog in der Grafik"));

  messkopf.init();
  wobbelgrunddaten.bdBm=false;
  wobbelgrunddaten.kshift=0.0;
  wobbelgrunddaten.dshift=0.0;
  dlg->setdaten(messkopf);
  dlg->setHWVariante(grunddaten.variante);
  dlg->tip(btip);
  QString qs=tr("Kalibrieren des Messkopfes","sondeneu");
  dlg->setTitel(qs);
  dlg->init();
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    messkopf=dlg->getdaten();
    //qDebug()<<"MkTyp:"<<messkopf.MkTyp();
    //qDebug()<<"ACD:"<<messkopf.ADCKanalZuordnung();
    //qDebug()<<"FrqFaktor:"<<messkopf.FrqFaktor();
    if((messkopf.MkTyp()==mks21) or messkopf.MkTyp()==mks11){
      messkopf.setCalFrqmin(wobbelgrunddaten.calfrqmin);
      messkopf.setCalFrqmax(wobbelgrunddaten.calfrqmax);
    }else{//mks21var und mks11var
      frqcal = wobbelgrunddaten.calfrqmin;
      messkopf.setCalFrqmin(frqcal);
      frqcal = QInputDialog::getDouble(this, tr("Kalibrier-Frequenzbereich","Inputdialog in der Grafik"),
                                       beschrvcomin, frqcal, wobbelgrunddaten.calfrqmin, wobbelgrunddaten.calfrqmax, 5, &ok);
      if(!ok){
        delete dlg;
        kalibrierende();
        return;
      }else{
        messkopf.setCalFrqmin(frqcal);
      }
      frqcal = wobbelgrunddaten.calfrqmax;
      messkopf.setCalFrqmax(frqcal);
      frqcal = QInputDialog::getDouble(this, tr("Kalibrier-Frequenzbereich","Inputdialog in der Grafik"),
                                       beschrvcomax, frqcal, wobbelgrunddaten.calfrqmin, wobbelgrunddaten.calfrqmax, 5, &ok);
      if(!ok){
        delete dlg;
        kalibrierende();
        return;
      }else{
        messkopf.setCalFrqmax(frqcal);
      }
    }
    //Unterscheiden zwischen NWT4000-1 und alle anderen NWTs
    switch(messkopf.MkTyp()){
    case mks11var:
    case mks21var:
      frqcal=(messkopf.CalFrqmax() - messkopf.CalFrqmin()) / 2.0 + messkopf.CalFrqmin();
      break;
    case mks11:
    case mks21:
    default:
      if(messkopf.CalFrqmin() > 100.0){
        frqcal=150.0;
      }else{
        frqcal=100.0;
      }
      break;
    }
    messkopf.setCalFrqPunkt(frqcal);
    messkopf.setCalFrqSchritt((messkopf.CalFrqmax() - messkopf.CalFrqmin()) / (maxmesspunkte - 1));
    grunddaten.filemk = messkopf.defDateiName(); //neuen Filename vergeben da neuer Messkopf
    Mkneu=true;
    kalibrierenMk();
  }
  delete dlg;
  if(bd)qDebug("Nwt4Window::sondeNeu() ENDE");
}

void Nwt4Window::setFlatCal(){
  if(bd)qDebug("Nwt4Window::setFlatCal()");

  wgrunddatenmerk=wobbelgrunddaten; //alte Grunddaten merken
  idxmesskurve=0;
  bbefwobbeln=false;
  catbefehl=0x60;
  clrrxbuffer();//RS232 RX Buffer loeschen
  if(bnwtgefunden){
    picmodem->writeChar(0x8F);
    picmodem->writeChar(0x60);//Flatnes Cal zum NWT senden
    befantworttimer->start(20);//20mSek warten, anschliessend RS232 RX Buffer auswerten
  }
  rlauf->setMaximum(wobbelgrunddaten.mpunkte);//Wobbelverlaufsanzeige max setzen
  messkurve.calablauf=4;//Flatnes Kalibrieren
  messkurve.calstring="";
  display->setMessKurve(messkurve);//Messkurve im Display darstellen
  cal60counter=0;
  cal60index=0;
  if(bd)qDebug("Nwt4Window::setFlatCal() ENDE");
}

void Nwt4Window::nachkalib(){
  if(bd)qDebug("Nwt4Window::nachkalib()");
  Mkneu=false;
  kalibrierenMk();
  if(bd)qDebug("Nwt4Window::nachkalib() ENDE");
}

void Nwt4Window::kalibrierenMk(){
  if(bd)qDebug("Nwt4Window::kalibrierenMk()");
  bool ok;
  QString qs;
  double frq;

  QMessageBox *msgBox;
  msgBox = new QMessageBox(this);
  QPushButton *ButtonOpen = msgBox->addButton(tr("Offen"), QMessageBox::ActionRole);
  QPushButton *Button100ohm = msgBox->addButton(tr("100 Ohm"), QMessageBox::ActionRole);
  QPushButton *Button25ohm = msgBox->addButton(tr("25 Ohm"), QMessageBox::ActionRole);
  QPushButton *Button75ohm = msgBox->addButton(tr("75 Ohm"), QMessageBox::ActionRole);
  QPushButton *ButtonAbbruch = msgBox->addButton(tr("Abbruch"), QMessageBox::ActionRole);

  QString beschrvcoS21(tr("<center><u><b>Kalibrieren S21-Messkopf</b></u><br>"
                       "Bitte zwischen RFout und RFin<br>"
                       "ein Daempfungsglied einschleifen</center>"
                       "-------------------------------------------------------<br>"
                       "Daempfungsglied in (dB)","Inputdialog in der Grafik"));

  QString beschrvcoS11(tr("<center><u><b>Kalibrieren S11-Messkopf</b></u><br>"
                       "Bitte zwischen SWV-Messkopf und RFin<br>"
                       "ein Daempfungsglied einschleifen</center>"
                       "-------------------------------------------------------<br>"
                       "Daempfungsglied in (dB)","Inputdialog in der Grafik"));

  QString beschrvcomitte(tr("<center><u><b>Auswahl der Kalibrierfrequenz</b></u><br>"
                            "Der Frequenzbereich des Messkopfes betraegt:</center>"
                            "VFO(min)= %1 MHz<br>"
                            "VFO(max)= %2 MHz"
                            "<center>------------------------------------</center>"
                            "Kalibrier-Frequenz in (MHz)","Inputdialog in der Grafik").arg(messkopf.CalFrqmin()).arg(messkopf.CalFrqmax()));

  QString tip_b100 = tr(
        "<b>Br&uuml;cke mit 100 Ohm Abschliessen</b><br>"
        "---------------------------------------<br>"
        "Dieser Abschlusswiderstand erzeugt eine <br>"
        "dB-Line von -9.542dB (SWV=2.0). Der Widerstand <br>"
        "muss selbst hergestellt werden."
        ,"tooltip text");

  QString tip_b25 = tr(
        "<b>Br&uuml;cke mit 25 Ohm Abschliessen</b><br>"
        "---------------------------------------<br>"
        "Dieser Abschlusswiderstand erzeugt eine <br>"
        "dB-Line von -9.542dB (SWV=2.0). Der Widerstand <br>"
        "muss selbst hergestellt werden. Oder 2x 50 Ohm <br>"
        "mit einem T-Glied parallel schalten."
        ,"tooltip text");

  QString tip_b75 = tr(
        "<b>Br&uuml;cke mit 75 Ohm Abschliessen</b><br>"
        "---------------------------------------<br>"
        "Dieser Abschlusswiderstand erzeugt eine <br>"
        "dB-Line von -13.979dB (SWV=1.5). Der Widerstand <br>"
        "muss selbst hergestellt werden aus 2x 150 Ohm."
        ,"tooltip text");

  QString tip_bopen = tr(
        "<b>Br&uuml;cke Offen lassen</b><br>"
        "---------------------------------------<br>"
        "Den Messeingang Offen lassen. Es entsteht <br>"
        "eine dB-Line von 0dB (SWV= unendlich)."
        ,"tooltip text");

  Button100ohm->setToolTip(tip_b100);
  Button25ohm->setToolTip(tip_b25);
  Button75ohm->setToolTip(tip_b75);
  ButtonOpen->setToolTip(tip_bopen);
  korrKurveGeladen();//Frequenzeinstellungen korrigieren wenn Kurvendatei geladen ist

  //Temporaere Daten zum kalibrieren
  caldaten.bmkneu=true;//Neuer Messkopf, neue Kalibrierung
  caldaten.adc1=0.0;//ADC-Wert Pegel 1 (0dB) beim Wobbeln
  caldaten.adc2=0.0;//ADC-Wert Pegel 2 (mit ATT) beim Wobbeln
  caldaten.adcmw1=0.0;//ADC-Wert Pegel 1 (0dB) mW-Meter
  caldaten.adcmw2=0.0;//ADC-Wert Pegel 2 (mit ATT) mW-Meter
  caldaten.att=0.0;//Wert des verwendeten ATT

  wgrunddatenmerk=wobbelgrunddaten; //alte Grunddaten merken
  wobbelgrunddaten.bdBm=false;//Spektrumanalyser aus
  mNoFrqCalK1->setChecked(false);//Frequenzkorrektur im Menue EIN
  messkopf.setNoFrqCal(false);//Frequenzkorrektur EIN
  //calibrier Frequenzgang loeschen
  messkopf.clrcalpegel();//DDS Frequenzgang loeschen
  //nach Mk-Typ den erste Pegel vorgeben
  //qDebug("CalATT: %f",messkopf.CalAtt());
  if(messkopf.CalAtt()!=0.0){
    pegel1 = messkopf.CalAtt();
  }else{
    if((messkopf.MkTyp()==mks11) or (messkopf.MkTyp()==mks11var)){
      if(grunddaten.variante == vnwt_ltdz){
        pegel1=-20.0;
      }else{
        pegel1=-30.0;
      }
    }else{
      if(grunddaten.variante == vnwt_ltdz){
        pegel1=-30.0;
      }else{
        pegel1=-40.0;
      }
    }
  }
  if((messkopf.MkTyp()==mks11) or (messkopf.MkTyp()==mks11var)){
    qs=beschrvcoS11;
  }else{
    qs=beschrvcoS21;
  }
  pegel1 = QInputDialog::getDouble(this, tr("Kalibrieren","Inputdialog in der Grafik"),
                                   qs, pegel1, -40, -10, 1, &ok);
  if(!ok){
    delete ButtonAbbruch;
    delete Button75ohm;
    delete Button100ohm;
    delete Button25ohm;
    delete ButtonOpen;
    delete msgBox;
    kalibrierende();
    return;
  }
  switch (messkopf.MkTyp()) {
  case mks21:
  case mks21var:
    break;
  case mks11:
  case mks11var:
    msgBox->setText(tr("<Center><u><b>Kalibrieren SWV-Messkopf</b></u></center>"));
    msgBox->setInformativeText(tr("<Center><B>Bitte Ausgang des SWV-Messkopfes mit Widerstand abschliessen, oder Offen lassen!!</B><br>"
                                  "Es folgt 1 Kalibrierdurchlauf.</Center>","kalibrieren"));
    switch(messkopf.getS11cal()){
    case vref25:msgBox->setDefaultButton(Button25ohm); break;
    case vref75:msgBox->setDefaultButton(Button75ohm); break;
    case vref100:msgBox->setDefaultButton(Button100ohm); break;
    case vrefopen:msgBox->setDefaultButton(ButtonOpen); break;
    case vrefudef:msgBox->setDefaultButton(Button75ohm); break;
    default:msgBox->setDefaultButton(Button75ohm); break;
    }
    msgBox->exec();
    if (msgBox->clickedButton() == ButtonAbbruch) {
      delete ButtonAbbruch;
      delete Button75ohm;
      delete Button100ohm;
      delete Button25ohm;
      delete ButtonOpen;
      delete msgBox;
      kalibrierende();
      return;
    }
    messkopf.setS11cal(vrefudef);
    if(msgBox->clickedButton()==Button25ohm) messkopf.setS11cal(vref25);
    if(msgBox->clickedButton()==Button75ohm) messkopf.setS11cal(vref75);
    if(msgBox->clickedButton()==Button100ohm) messkopf.setS11cal(vref100);
    if(msgBox->clickedButton()==ButtonOpen) messkopf.setS11cal(vrefopen);
    break;
  default:
    break;
  }
  delay(200);//200 mSek warten
  switch (messkopf.MkTyp()) {
  case mks21:
  case mks11:
    if(messkopf.CalFrqmin() > 100.0){
      frq=150.0;
    }else{
      frq=100.0;
    }
    break;
  case mks11var:
  case mks21var:
  default:
    frq=(messkopf.CalFrqmax() - messkopf.CalFrqmin())/2.0 + messkopf.CalFrqmin();
    break;
  }
  if(messkopf.CalFrqPunkt()==0.0)messkopf.setCalFrqPunkt(frq);//HF Messkopf Kalibrierung auf eine gültige Frequenz setzen
  frqcalpunkt1 = messkopf.CalFrqPunkt();
  frqcalpunkt1 = QInputDialog::getDouble(this, tr("Kalibrier-Frequenzpunkt","Inputdialog in der Grafik"),
                                   beschrvcomitte, frqcalpunkt1, messkopf.CalFrqmin(), messkopf.CalFrqmax(), 5, &ok);
  if(!ok){
    delete ButtonAbbruch;
    delete Button75ohm;
    delete Button100ohm;
    delete Button25ohm;
    delete ButtonOpen;
    delete msgBox;
    kalibrierende();
    return;
  }
  messkopf.setCalFrqPunkt(frqcalpunkt1);//Kalibrierfrequenz im Messkopf speichern
  //wobbelgrunddaten.frequenzanfang = frqcalpunkt1 - 0.05; // 50 kHz unterhalb anfangen
  wobbelgrunddaten.frequenzanfang = frqcalpunkt1 - 0.5; // 500 kHz unterhalb anfangen
  wobbelgrunddaten.mpunkte=100;//nur 100 Messpunkte fuer calX und calY
  wobbelgrunddaten.ztime=100;//etwa 10mSek
  //wobbelgrunddaten.ztime=0;//etwa 0mSek
  //wobbelgrunddaten.frequenzschritt=0.001;//1kHz
  wobbelgrunddaten.frequenzschritt=0.01;//10kHz
  wobbeinmal=true;//nur einmaliger Durchlauf
  messkurve.calablauf=1;//zuerst Schritt1
  buttoneinzeln->setEnabled(false);
  buttonwobbeln->setEnabled(false);
  buttonstop->setEnabled(false);
  caldaten.att=pegel1;
  sendbefwobbeln();
  //qDebug()<<"1)wobbelgrunddaten.mpunkte:"<<wobbelgrunddaten.mpunkte;
  //qDebug()<<"1)wobbelgrunddaten.frequenzanfang"<<wobbelgrunddaten.frequenzanfang;
  //qDebug()<<"1)wobbelgrunddaten.frequenzschritt"<<wobbelgrunddaten.frequenzschritt;
  delete ButtonAbbruch;
  delete Button75ohm;
  delete Button100ohm;
  delete Button25ohm;
  delete ButtonOpen;
  delete msgBox;
  if(bd)qDebug("Nwt4Window::kalibrierenMk() ENDE");
}

void Nwt4Window::saveMk(QString s){
  if(bd)qDebug("Nwt4Window::saveMk(QString s)");
  QFile f;

  if (!s.isNull())
  {
    //Datei ueberpruefen ob Sufix vorhanden
    if(!s.contains(".n4m")) s += ".n4m";
    grunddaten.filemk=s;
    QDir dir(s);
    QString s1 = dir.dirName();
    s1.remove(".n4m");
    QString s3=s1;
    s3.replace('_',' ');
    labelkanal->setText(s3);
    //checkboxkanal->setText(s1);
    grmesskanal->setTitle("mW-Meter - [ "+s1+" ]");
    spbarminkanal->setValue(-85);
    configfile ms;
    ms.open(s);
    ms.writeInteger("typ", int(messkopf.MkTyp()));
    ms.writeDouble("calx", messkopf.CalX());
    //qDebug("calx: %0.9e",messkopf.CalX());
    ms.writeDouble("caly", messkopf.CalY());
    //qDebug("caly: %0.9e",messkopf.CalY());
    ms.writeDouble("calxmw", messkopf.CalXmWMeter());
    //qDebug("calx: %0.9e",messkopf.CalXmWMeter());
    ms.writeDouble("calymw", messkopf.CalYmWMeter());
    //qDebug("caly: %0.9e",messkopf.CalYmWMeter());
    ms.writeDouble("calfrqmin",messkopf.CalFrqmin());
    ms.writeDouble("calfrqmax",messkopf.CalFrqmax());
    ms.writeDouble("calfrqpoint",messkopf.CalFrqPunkt());
    ms.writeDouble("calkorrdBm",messkopf.CalKorrdBmPegel());
    if(caldaten.bmkneu){
      ms.writeDouble("caldaten_adc1",caldaten.adc1);
      ms.writeDouble("caldaten_adc2",caldaten.adc2);
      ms.writeDouble("caldaten_adcmw1",caldaten.adcmw1);
      ms.writeDouble("caldaten_adcmw2",caldaten.adcmw2);
      ms.writeDouble("caldaten_att",caldaten.att);
      ms.writeInteger("S11cal_variante",int(messkopf.getS11cal()));
      caldaten.bmkneu=false;
    }
    s3 = s;
    s3.replace(QString(".n4m"), ".n4c");
    ms.writeString("caldateiname",s3);
    //qDebug("Messkopf1 speichern");
    ms.close();
    //speichern der Kalibierfrequenzdatei
    f.setFileName(grunddaten.homedir.filePath(s3));
    if(f.open(QIODevice::WriteOnly)){
      QTextStream ts(&f);
      ts.setRealNumberPrecision(9);
      for(int i=0; i<maxmesspunkte; i++){
        ts << messkopf.calpegelwert(i) << endl;
      }
      f.close();
    }
  }
  if(bd)qDebug("Nwt4Window::saveMk(QString s) ENDE");
}

void Nwt4Window::speichernMesskopf(){
  if(bd)qDebug("Nwt4Window::speichernMesskopf()");

  QString s2 = tr("NWT4000 Sondendatei (*.n4m)","FileDialog");

  QString s1 = grunddaten.filemk;
  //noch keine Datei geladen
  s1.remove(".n4m");
  if(s1 == "Messkopf" or s1.isEmpty()){
    s1 = messkopf.defDateiName();//neuen Dateinamen zusammensetzen
  }
  if(!s1.contains(".n4m"))s1+=".n4m";
  QString s = QFileDialog::getSaveFileName(this,tr("NWT4000 Sondendatei speichern","FileDialog"),
                                           grunddaten.homedir.filePath(s1), s2, &s1);
  saveMk(s);
  if(bd)qDebug("Nwt4Window::speichernMesskopf() ENDE");
}

void Nwt4Window::ClrMesskopf(){
  if(bd)qDebug("Nwt4Window::ClrMesskopf()");
  QFileDialog dialog(this, tr("Messkoepfe Loeschen"));
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setOptions(QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setLabelText(QFileDialog::Accept, tr("Loeschen"));
  dialog.setNameFilter("*.n4m");
  //dialog.setFilter("*.n4m");
  dialog.setDirectory(grunddaten.homedir.filePath(""));

  if (dialog.exec())
  {
      qDebug() << dialog.selectedFiles();
      QStringList files=dialog.selectedFiles();
      QString qs;
      QFile f;

      //qDebug()<<files.size();
      for(int i=0;i<files.size();i++){
        qs=files.at(i);
        //qDebug()<<qs;
        f.setFileName(qs);f.remove();
        qs.replace(".n4m",".n4c");
        f.setFileName(qs);f.remove();
        //qDebug()<<qs;
      }
  }
  if(bd)qDebug("Nwt4Window::ClrMesskopf() ENDE");
}

void Nwt4Window::ladenMesskopf1(){
  if(bd)qDebug("Nwt4Window::ladenMesskopf1()");
  QString s2 = tr("NWT4000 Sondendatei (*.n4m)","FileDialog");

  QString s1 = "*.n4m";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Sondendatei laden","FileDialog"),
                                           grunddaten.homedir.filePath(s1), s2);
  if (!s.isNull())
  {
    loadMk(s);
  }
  if(bd)qDebug("Nwt4Window::ladenMesskopf1() ENDE");
}

void Nwt4Window::loadMk(QString s){
  if(bd)qDebug("Nwt4Window::loadMk(QString s)");
  QString s1, s2, qs, qs1;
  QFile f;
  double d;
  double mkx=0.0, mky=0.0, mkxmw=0.0, mkymw=0.0;
  double calfrqmin=35.0, calfrqmax=4400.0, calfrqpoint=2185.25, calkorrdbm=0.0, calatt=0.0;

  if(bd)qDebug("Nwt4Window::loadMk(QString s)");

  if (!s.isNull()){
    configfile ms;
    if(ms.open(grunddaten.homedir.filePath(s))){
      int typ = ms.readInteger("typ", int(mks21));//default S21
      messkopf.setMkTyp(emktyp(typ));
      if((messkopf.MkTyp()==mks11) or (messkopf.MkTyp()==mks11var)){
        checkboxswvkanal_checked(true);
        checkboxswvkanal->setChecked(true);
        spbarminkanal->setSuffix(" dB");
        spbarmaxkanal->setSuffix(" dB");
        labelattextkanal->setEnabled(false);
        spattextkanal->setEnabled(false);
        checkboxspeki->setChecked(false);
        checkboxspeki->setEnabled(false);
        mNoFrqCalK1->setChecked(true);//Default S21 Freq.gang.korr AUS
      }else{
        checkboxswvkanal_checked(false);
        checkboxswvkanal->setChecked(false);
        checkboxspeki->setEnabled(true);
        spbarminkanal->setSuffix(" dBm");
        spbarmaxkanal->setSuffix(" dBm");
        if(checkkanal->checkState() == Qt::Checked){
          labelattextkanal->setEnabled(true);
          spattextkanal->setEnabled(true);
        }else{
          labelattextkanal->setEnabled(false);
          spattextkanal->setEnabled(false);
        }
        mNoFrqCalK1->setChecked(false);//Default S21 Freq.gang.korr EIN
      }
      mkx=ms.readDouble("calx", 0.19);//default X
      mky=ms.readDouble("caly", -87.0);//default Y
      mkxmw=ms.readDouble("calxmw", mkx);//default X
      mkymw=ms.readDouble("calymw", mky);//default Y
      messkopf.setMkXY(mkx,mky);
      messkopf.setMkXYmWMeter(mkxmw,mkymw);
      calfrqmin=ms.readDouble("calfrqmin", wobbelgrunddaten.calfrqmin);
      //qDebug()<<calfrqmin;
      messkopf.setCalFrqmin(calfrqmin);
      calfrqmax=ms.readDouble("calfrqmax", wobbelgrunddaten.calfrqmax);
      messkopf.setCalFrqmax(calfrqmax);
      calfrqpoint=ms.readDouble("calfrqpoint",(calfrqmax - calfrqmin) / 2 + calfrqmin);
      messkopf.setCalFrqPunkt(calfrqpoint);
      calkorrdbm=ms.readDouble("calkorrdBm",0.0);
      messkopf.setCalKorrdBmPegel(calkorrdbm);
      messkopf.setCalFrqSchritt((calfrqmax - calfrqmin) / 2000.0);
      calatt=ms.readDouble("caldaten_att",0.0);
      messkopf.setCalAtt(calatt);
      eS11cal sc=eS11cal(ms.readInteger("S11cal_variante",int(vrefudef)));
      messkopf.setS11cal(sc);
      s2=ms.readString("caldateiname",s1);
      //qDebug()<<s2;
      ms.close();
      f.setFileName(grunddaten.homedir.filePath(s2));
      if(f.open(QIODevice::ReadOnly)){
        QTextStream ts(&f);
        for(int i=0; i<maxmesspunkte; i++){
          ts >> d;
          //qDebug()<<"index"<<i<<"db:"<<d;
          messkopf.setcalpegelwert(i,d);
        }
        f.close();
      }
      grunddaten.filemk=s;
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4m");
      grmesskanal->setTitle("mW-Meter - [ "+s1+" ]");
      QString s3=s1;
      s3.replace('_',' ');
      labelkanal->setText(s3);
      //checkboxkanal->setText(s1);
      spbarminkanal->setValue(-85);
      setMkInfo();
    }
  }
  tip(btip);
  testFrqMinMax();
  if(bd)qDebug("Nwt4Window::loadMk(QString s) ENDE");
}

void Nwt4Window::setMkInfo(){
  QString beschr;
  QString qs,qs1;

  mkinfokanal.clear();
  mkinfokanal.append(tr("<b>Messkopf Beschreibung</b>"));
  mkinfokanal.append(tr("<b>=====================</b>"));
  mkinfokanal.append(tr("<b>Dateiname:</b> ")+grunddaten.filemk);
  switch(messkopf.MkTyp()){
  case mks21:
  case mks21var:
    //mkinfokanal.append(tr("<b>Messkopftyp:</b> Logarithmischer Messkopf"));
    mkinfokanal.append(tr("<b>Verwendung :</b> S21 Messung, Spektrumanalyse, mW-Meter"));
    beschr=tr("<b>Logarithmischer Messkopf</b><br>S21, Spektrumanalyser, mW-Meter<br>");
    break;
  case mks11:
  case mks11var:
    beschr=tr("<b>Logarithmischer Messkopf</b><br>S11-SWV Messung<br>");
    //mkinfokanal.append(tr("<b>Messkopftyp:</b> Logarithmischer Messkopf"));
    mkinfokanal.append(tr("<b>Verwendung :</b> SWV-Messung"));
    switch(messkopf.getS11cal()){
    case vref25:qs1=" 25 Ohm!";break;
    case vref75:qs1=" 75 Ohm!";break;
    case vref100:qs1=" 100 Ohm!";break;
    case vrefopen:qs1=tr(" Offen!");break;
    case vrefudef:qs1=tr(" unbekannt!");break;
    default:qs1=tr(" unbekannt!");break;
    }
    qs=tr("<b>Kalibrierung, Messbruecke Abschlusswiderstand:</b>%1").arg(qs1);
    mkinfokanal.append(qs);
    break;
  }
  mkinfokanal.append(tr("<b>Kalibrierfrequenz(min):</b> %1 MHz").arg(messkopf.CalFrqmin()));
  mkinfokanal.append(tr("<b>Kalibrierfrequenz(max):</b> %1 MHz").arg(messkopf.CalFrqmax()));
  mkinfokanal.append(tr("<b>Kalibrierfrequenz:</b> %1 MHz").arg(messkopf.CalFrqPunkt()));
  mkinfokanal.append(tr("<b>Pegelkorrektur Spektrumanzeige:</b> %1 dB").arg(messkopf.CalKorrdBmPegel()));
  labelkanal->setToolTip(beschr);
  buttonloadmessk->setToolTip(beschr);
  checkboxkanal->setToolTip(beschr);
  grmesskanal->setToolTip(beschr);
  mkinfokanal.append(tr("<b>Kalibrierwerte:</b>"));
  qs.sprintf("X-Wert = %2.8f", messkopf.CalX());
  mkinfokanal.append(qs);
  qs.sprintf("Y-Wert = %2.8f", messkopf.CalY());
  mkinfokanal.append(qs);
  qs.sprintf("X-Wert mW-Meter = %2.8f", messkopf.CalXmWMeter());
  mkinfokanal.append(qs);
  qs.sprintf("Y-Wert mW-Meter = %2.8f", messkopf.CalYmWMeter());
  mkinfokanal.append(qs);
}

void Nwt4Window::setProgramPath(const QString &ap){
  if(bd)qDebug("Nwt4Window::setProgramPath(const QString &ap)");
  //qDebug("Nwt4Window::setProgramPath(QString s)");
  programpath = ap;
  //qDebug(programpath);
  grunddaten.homedir.setPath(programpath);
  //der Programname ist noch mit im Pfad: deshalb cdUp
  grunddaten.homedir.cdUp();
  //QString s = grunddaten.homedir.absolutePath();
  //qDebug(s);
  //Test ob im Progrannverzeichnis das Verzeichnis nwt befindet
  if(!grunddaten.homedir.cd(grunddaten.spath)){
    //KEIN nwt Verzeichnis: HOME ist das neue Verzeichnis
    grunddaten.homedir = QDir::home();
    if(!grunddaten.homedir.cd(grunddaten.spath)){
      //Befindet sich im HOME kein nwt4-Verzeichnis
      //hfm9 erzeugen
      grunddaten.homedir.mkdir(grunddaten.spath);
    }
    //in das nwt4 gehen
    grunddaten.homedir.cd(grunddaten.spath);
  }
  QDir dir=grunddaten.homedir;
  dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);
  readSettings();
  grafiksetzen();
  //QString s = grunddaten.homedir.absolutePath();
  //qDebug("grunddaten.homedir:");
  //qDebug()<<s;
  if(bd)qDebug("Nwt4Window::setProgramPath(const QString &ap) ENDE");
}

void Nwt4Window::setConfigPath(const QString& s){
  if(bd)qDebug("Nwt4Window::setConfigPath(const QString& s)");
  grunddaten.spath=s;
  settingspath=s;
  settingspath=settingspath.toUpper();//Grossschreibung
  //qDebug()<<"settingspath.toUpper()"<<settingspath;
  if(bd)qDebug("Nwt4Window::setConfigPath(const QString& s) ENDE");
}

void Nwt4Window::KurvenLaden(){
  if(bd)qDebug("Nwt4Window::KurvenLaden()");
  QFile f;
  QString zeile, w1, w2, info;
  double d;
  bool ok, binfo=false;
  QSettings settings("AFU", settingspath);

  if(!bkurvegeladen){
    wgrunddatenmerk=wobbelgrunddaten; //alte Grunddaten merken
    messkurvemerk=messkurve;//alte Messkurve K1+K2 merken
    bkurvegeladen=true;
    //alle Analysen aus
    messkurve.b3db=false;
    messkurve.b6db=false;
    messkurve.b60db=false;
    messkurve.bdbmax=false;
    messkurve.bdbmin=false;
    messkurve.bguete=false;
    messkurve.bshape=false;
    messkurve.bswv=false;
    checkbox3db->setChecked(false);
    checkbox6db->setChecked(false);
    checkbox60db->setChecked(false);
    checkboxdbmax->setChecked(false);
    checkboxdbmin->setChecked(false);
    checkboxguete->setChecked(false);
    checkboxshape->setChecked(false);
    checkboxswvkanal->setChecked(false);
  }
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      int index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          messkurve.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)messkurve.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              messkurve.dbk1[index]=d;
              //qDebug()<<"index:"<<index<<"; messkurve.dbk1[index]:"<<messkurve.dbk1[index];
              index++;
            }
          }
        }
        //qDebug()<<w1;
        //qDebug()<<w2;
      }while(!ts.atEnd());
      //index--;
      //qDebug()<<"index:"<<index;
      messkurve.mpunkte=index;
      messkurve.bkanal=true;
      //messkurve.bkanal2=false;
      messkurve.bkurvegueltig=true;
      checkboxkanal->setChecked(true);
      wobbelgrunddaten.frequenzanfang=messkurve.frequenz[0];
      wobbelgrunddaten.mpunkte=messkurve.mpunkte;
      wobbelgrunddaten.frequenzschritt=(messkurve.frequenz[messkurve.mpunkte-1]-messkurve.frequenz[0])/(messkurve.mpunkte-1);
      WobbelGrundDatenToDisplay();
      setInfoText();
      display->setMessKurve(messkurve);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this, tr("Kurven-Information!"), kurvebeschr);
    }
    f.close();
  }
  if(bd)qDebug("Nwt4Window::KurvenLaden() ENDE");
}

void Nwt4Window::SaveKurve(){
  KurvenSichern(messkurve);
}

void Nwt4Window::KurvenSichern(TMessKurve mk){
  if(bd)qDebug("Nwt4Window::KurvenSichern()");
  QFile f;
  QString sm, qs;
  QString s1 = "*.n4k";
  TMessKurve mkurve=mk;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  //bzweidateien=(mkurve.bkanal and mkurve.bkanal2);
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  //Dateiname erzeugen
  if(mkurve.caption.isEmpty()){
    s1=datetime2filename();// Caption leer Defaultdateiname wird generiert aus Date und Time
  }else{
    s1=mkurve.caption;//Caption wird als Dateiname verwendet
  }
  s1=fnamenormal(s1);
  QString s = QFileDialog::getSaveFileName(this,tr("NWT4000 Kurven-Datei speichern","FileDialog"),
                                           path.filePath(s1), s2, &s1);
  if (!s.isNull()){
    if(s.contains(".n4k")) s.remove(".n4k");//Dateiname Suffix entfernen
    sm=s;// Dateiname merken für Kanal 2
    if(mkurve.bkanal){
      //Sufix hinzu
      s += ".n4k";
      f.setFileName(grunddaten.homedir.filePath(s));
      if(f.open(QIODevice::WriteOnly)){
        QTextStream ts(&f);
        ts.setRealNumberPrecision(9);
        mkurve.caption.replace(' ', '_');// Alle Leerzeichen ersetzen da sonst Probleme beim wieder lesen
        ts << "#" << mkurve.caption << endl;//Zuerst Caption
        if((ksettings.Displayshift()+ksettings.Kurvenshift())>0.0){
          qs.sprintf("$$$S21 Messdämpfung:%1.0f_db", ksettings.Displayshift()+ksettings.Kurvenshift());
          ts << qs << endl;
        }
        if((messkurve.mktyp == mks11) or (messkurve.mktyp==mks11var)){
          qs="$$$S11_Kurve";
          ts << qs << endl;
          qs="$$$dB";
          ts << qs << endl;
        }else{
          qs="$$$S21_Kurve";
          ts << qs << endl;
          if(wobbelgrunddaten.bdBm){
            qs="$$$dBm";
          }else{
            qs="$$$dB";
          }
          ts << qs << endl;
        }
        for(int i=0; i<mkurve.mpunkte; i++){
          ts << mkurve.frequenz[i] << ";" << mkurve.dbk1[i] << endl;
        }
        f.close();
      }
    }
    if((infolist.count()>0) or ((ksettings.Displayshift()+ksettings.Kurvenshift())>0.0)){
      s=sm + ".txt";
      f.setFileName(grunddaten.homedir.filePath(s));
      if(f.open(QIODevice::WriteOnly)){
        QTextStream ts(&f);
        if((ksettings.Displayshift()+ksettings.Kurvenshift())>0.0){
          qs.sprintf("S21 Messdämpfung:%1.0f_db", ksettings.Displayshift()+ksettings.Kurvenshift());
          ts << qs << endl;
        }
        if(!mkurve.caption.isNull()){
          mkurve.caption.replace(' ', '_');// Alle Leerzeichen ersetzen da sonst Probleme beim wieder lesen
          ts <<mkurve.caption << endl;
          int a=mkurve.caption.count();
          qs="=";
          for(int i=0;i<a;i++)qs+="=";
          ts << qs << endl;
        }
        for(int i=0; i<infolist.size(); i++){
          qs = infolist.at(i);
          ts << qs << endl;
        }
      }
      f.close();
    }
    //Pfad setzen und in Registry speichern
    path.setPath(s);
    s=path.dirName();
    s1=path.absolutePath();
    s1.remove(s);
    settings.setValue("kurvenpath",s1);
  }
  if(bd)qDebug("Nwt4Window::KurvenSichern() ENDE");
}

void Nwt4Window::delay(int zeit)
{
  if(zeit == 0)return;
  tstop = false;
  delaytimer->start(zeit);
  while(!tstop){
    qApp->processEvents();//Austritt aus der Funktion fuer Events
  }
}

void Nwt4Window::delaytimerstop()
{
  tstop = true;
  delaytimer->stop();
}

void Nwt4Window::wobwiederholtimerstop(){
  wobwiederholtimer->stop();
  sendbefwobbeln();
}

void Nwt4Window::MesskopfInfoK1(){
  QString qs="";
  for(int i=0;i<mkinfokanal.count();i++){
    qs+=mkinfokanal.at(i);
    qs+="<br>";
  }
  QMessageBox::about(this, tr("Messkopf Info","MK Info"), qs);
}

void Nwt4Window::Info()
{
  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());
  QDir imgpath;
  imgpath.setPath(settings.value("imgpath", QDir::homePath()).toString());

  QString qs=tr("<b>Home:</b> ")+ grunddaten.homedir.path();
  QString qs1=tr("<b>Kurven:</b> ")+ path.absolutePath();
  QString qs2=tr("<b>Bilder:</b> ")+ imgpath.absolutePath();
#ifdef Q_OS_WIN
  QMessageBox::about(this, "PC Software-NWT2win", infotext+qs+"<br>"+qs1+"<br>"+qs2);
#else
  QMessageBox::about(this, "PC Software-NWT2lin", infotext+qs+"<br>"+qs1+"<br>"+qs2);
#endif
}

void Nwt4Window::fwversion()
{
  QString fwtext;
  QString qsfw;
  qsfw.sprintf("FW-Version: %i.%02i", grunddaten.version/100,grunddaten.version-100);
  //Variante im Display anzeigen
  switch(grunddaten.variante){
  case vnwt4_1:qsfw+=":NWT4000-1";break;
  case vnwt4_2:qsfw+=":NWT4000_2";break;
  case vnwt_ltdz:qsfw+=":LTDZ 35-4400M";break;
  case vnwt6:qsfw+=":NWT6000";break;
  case vnwt_nn:qsfw+=":NWT noname";break;
  }
  QMessageBox::about(this, "Firmware und Hardware", qsfw);
}

void Nwt4Window::korrKurveGeladen(){
  if(bkurvegeladen){
    wobbelgrunddaten=wgrunddatenmerk; //alte Grunddaten wieder aktualisieren
    messkurve=messkurvemerk;
    bkurvegeladen=false;
    if(messkurve.bkanal)checkboxkanal->setChecked(true);
    if(messkurve.b3db)checkbox3db->setChecked(true);
    if(messkurve.b3dbinv)checkbox3dbinv->setChecked(true);
    if(messkurve.b6db)checkbox6db->setChecked(true);
    if(messkurve.b60db)checkbox60db->setChecked(true);
    if(messkurve.bdbmax)checkboxdbmax->setChecked(true);
    if(messkurve.bdbmin)checkboxdbmin->setChecked(true);
    if(messkurve.bguete)checkboxguete->setChecked(true);
    if(messkurve.bshape)checkboxshape->setChecked(true);
  }
}


void Nwt4Window::tip(bool atip){
  QString tip_buttonloadk = tr("Laden Messkopf-Datei in Kanal","tooltip_text");
  QString tip_grafik = tr(
        "<b>Linke Maustaste:</b><br>"
        "-----------------------<br>"
        "Kursorpostion = Frequenzinfo <br>"
        "oder Marker setzen/verschieben<br>"
        "oder Markertext positionieren<br>"
        "-----------------------<br>"
        "<b>Rechte Maustaste:</b><br>"
        "-----------------------<br>"
        "Auswahl PopUp-Menue"
        ,"tooltip text");
  QString tip_edit = tr(
        "<b>Eingabe:</b><br>"
        "3m5 oder 3,5m oder 3.5m = 3,500000 MHz<br>"
        "<b>Eingabe:</b><br>"
        "350k5 oder 350,5k oder 350.5k = 350,500 kHz"
        ,"tooltip text");
  QString tip_steptime = tr(
        "<b>Zusaetzliche Zwischenzeit pro Step</b><br>"
        "#1 = etwa 12 mSekunden,<br>"
        "#2 = etwa 24 mSekunden,<br>"
        "...<br>"
        "#9 = etwa 108 mSekunden"
        ,"tooltip text");
  QString tip_dbm = tr(
        "<b>Spektrumanzeige</b><br>"
        "-----------------------<br>"
        "Umschalten von dB auf dBm.<br>"
        "Der Pegel \"dB-Korrektur Spektumanzeige\" <br>"
        "wird mit eingerechnet. <br>"
        "Die Frequenzgangkorr. wird deaktiviert. "
        ,"tooltip text");
  QString tip_ydb = tr(
        "<b>dB Scalierung des Displays</b><br>"
        "-----------------------<br>"
        "Veraenderung der Aufloesung des<br>"
        "Displays Bereich (+30dB bis -120dB)"
        ,"tooltip text");
  QString tip_hrdb = tr(
        "<b>Horizontale dB-Linie</b><br>"
        "-----------------------<br>"
        "Es kann eine zusaetzliche<br>"
        "dB-Linie eingeblendet werden."
        ,"tooltip text");
  QString tip_messminimum = tr(
        "<b>Minimum der Bargraphanzeige</b><br>"
        "-------------------------------<br>"
        "Mit dieser Box wird das Minimum <br>"
        "der Bargraphanzeige eingestellt.<br>"
        "Somit kann der Anzeigebereich <br>"
        "eingeschr&auml;nkt werden. Die <br>"
        "Anzeige wird empfindlicher."
        ,"tooltip text");
  QString tip_messmaximum = tr(
        "<b>Maximum der Bargraphanzeige</b> <br>"
        "-------------------------------<br>"
        "Mit dieser Box wird das Maximum <br>"
        "der Bargraphanzeige eingestellt. <br>"
        "Somit kann der Anzeigebereich <br>"
        "eingeschr&auml;nkt werden. Die <br>"
        "Anzeige wird empfindlicher."
        ,"tooltip text");
  QString tip_messattext = tr(
        "<b>Externes Leistungsd&auml;mpfungsglied</b> <br>"
        "-----------------------------------------<br>"
        "Wird ein D&auml;mpfungsglied vor dem Messeingang <br>"
        "geschaltet, z.B. Dummyload mit Messausgang, <br>"
        "kann hier der D&auml;mpfungswert eingestellt <br>"
        "werden. Der D&auml;mpfswert geht mit in die <br>"
        "Berechnung der Anzeigewerte ein."
        ,"tooltip text");
  QString tip_messhang = tr(
        "<b>H&auml;ngezeit der Textanzeigen</b> <br>"
        "-----------------------------------<br>"
        "Die Textanzeige der Messwerte sind sehr <br>"
        "unruhig. Deshalb habe ich eine Peak&Hold<br>"
        "Zeit programmiert. Die Anzeigen erscheinen <br>"
        "Etwas ruhiger. Die Balkenanzeige wird davon <br>"
        "nicht beeinflusst."
        ,"tooltip text");
  QString tip_messprecision = tr(
        "<b>Kommastellen der dBm-Anzeige</b> <br>"
        "--------------------------------<br>"
        "Einstellung der Nachkommastellen."
        ,"tooltip text");
  QString tip_swvlinie = tr(
        "<b>Einblendung SWV Linie im Display</b><br>"
        "--------------------------------<br>"
        "Schnelles Einstellen mit dem Mausrad.<br>"
        "Dabei &auml;ndert sich automatisch <br>"
        "der Wert des SWV und die Linie<br>"
        "(Bereich 1.001 bis 5.0)"
        ,"tooltip text");
  QString tip_swv_a_100 = tr(
        "<b>Koaxspeiseleitung heraus rechnen</b><br>"
        "--------------------------------<br>"
        "Eingabe der Kabeld&auml;mpfungswerte. Die <br>"
        "Werte bitte aus den technischen Daten des<br>"
        "Kabels entnehmen. Es erscheint in den Markern<br>"
        "zus&auml;tzlich das SWV direkt an der Antenne."
        ,"tooltip text");
  QString tip_swv_alaenge = tr(
        "<b>Koaxspeiseleitung heraus rechnen</b><br>"
        "--------------------------------<br>"
        "Eingabe der Kabell&auml;nge in Meter. Daraus <br>"
        "die tats&auml;chliche Kabeld&auml;mpfung<br>"
        "errechnet. Es erscheint in den Markern<br>"
        "zus&auml;tzlich das SWV direkt an der Antenne."
        ,"tooltip text");
  QString tip_mw_dbm = tr(
        "<b>dBm Leistungsanzeige</b><br>"
        "--------------------------------<br>"
        "Die Stellen hinter den Komma werden in<br>"
        "Precision eingestellt. Default ist eine<br>"
        "Stelle nach dem Komma."
        ,"tooltip text");
  QString tip_mw_dbm_rl = tr(
        "<b>Return Loss, SWV-Messung</b><br>"
        "--------------------------------<br>"
        "Die Stellen hinter den Komma werden in<br>"
        "Precision eingestellt. Default ist eine<br>"
        "Stelle nach dem Komma."
        ,"tooltip text");
  QString tip_mw_volt = tr(
        "<b>Spannunganzeige</b><br>"
        "--------------------------------<br>"
        "Der Pegel dBm wird in Spannung <br>"
        "umgerechnet. Es wird ein Lastwiderstand<br>"
        "von 50Ohm zu Grunde gelegt."
        ,"tooltip text");
  QString tip_mw_volt_swv = tr(
        "<b>SWV</b><br>"
        "--------------------------------<br>"
        "Das Return Loss wird in SWV<br>"
        "umgerechnet."
        ,"tooltip text");
  QString tip_mw_watt = tr(
        "<b>Leistungsanzeige Watt</b><br>"
        "--------------------------------<br>"
        "Aus dem Pegel wird die Leistung in Watt<br>"
        "errechnet. Es wird ein Lastwiderstand<br>"
        "von 50Ohm zu Grunde gelegt."
        ,"tooltip text");
  QString tip_mw_adc = tr(
        "<b>Anzeige ADC-Wert</b><br>"
        "--------------------------------<br>"
        "Das ist der Wert des AD/Wandlers<br>"
        "vom NWTxxxx. Zahlenwert<br>"
        "0 bis 1023."
        ,"tooltip text");
  QString tip_mkinfo = tr(
        "<b>Messkopf-Info</b>"
        ,"tooltip text");
  QString tip_displayshift = tr(
        "<b>Display Shift oder</b><br>"
        "<b>S21 Messdaempfung</b><br>"
        "--------------------------------<br>"
        "Bei Verwendung eines externen ATT<br>"
        "kann die Displayanzeige und die <br>"
        "Berechnungen um diesen dB-Wert <br>"
        "verschoben werden. Wir erhalten <br>"
        "eine richtige Anzeige der dB-Werte <br>"
        "oder dBm-Werte."
        ,"tooltip text");

  btip = atip;

  //qDebug()<<btip;
  if(btip){
    buttoninfomessk->setToolTip(tip_mkinfo);
    checkboxspeki->setToolTip(tip_dbm);
    buttonloadmessk->setToolTip(tip_buttonloadk);
    labeldbmdigitkanal->setToolTip(tip_messprecision);
    spdbmdigitkanal->setToolTip(tip_messprecision);
    labelhangkanal->setToolTip(tip_messhang);
    sphangkanal->setToolTip(tip_messhang);
    spattextkanal->setToolTip(tip_messattext);
    labelattextkanal->setToolTip(tip_messattext);
    spbarminkanal->setToolTip(tip_messminimum);
    spbarmaxkanal->setToolTip(tip_messmaximum);
    display->setToolTip(tip_grafik);
    lineEdit_frq_anfang->setToolTip(tip_edit);
    lineEdit_frq_ende->setToolTip(tip_edit);
    lineEdit_frq_span->setToolTip(tip_edit);
    lineEdit_frq_mitte->setToolTip(tip_edit);
    lineEdit_schrittweite->setToolTip(tip_edit);
    boxztime->setToolTip(tip_steptime);
    label_ztime->setToolTip(tip_steptime);
    grygrenzen->setToolTip(tip_ydb);
    boxdblinie->setToolTip(tip_hrdb);
    boxswvlinie1->setToolTip(tip_swvlinie);
    boxswvlinie2->setToolTip(tip_swvlinie);
    labelboxswvlinie1->setToolTip(tip_swvlinie);
    labelboxswvlinie2->setToolTip(tip_swvlinie);
    checkboxswvlinie1->setToolTip(tip_swvlinie);
    checkboxswvlinie2->setToolTip(tip_swvlinie);
    boxalaenge->setToolTip(tip_swv_alaenge);
    labelalaenge->setToolTip(tip_swv_alaenge);
    boxa_100->setToolTip(tip_swv_a_100);
    labela_100->setToolTip(tip_swv_a_100);
    labeladckanal->setToolTip(tip_mw_adc);
    if(messkopf.MkTyp()==mks11 or messkopf.MkTyp()==mks11var){
      labeldbmkanal->setToolTip(tip_mw_dbm_rl);
      labelvoltkanal->setToolTip(tip_mw_volt_swv);
      labelwattkanal->setToolTip("");
    }else{
      labeldbmkanal->setToolTip(tip_mw_dbm);
      labelvoltkanal->setToolTip(tip_mw_volt);
      labelwattkanal->setToolTip(tip_mw_watt);
    }
    grdshift->setToolTip(tip_displayshift);
  }else{
    buttoninfomessk->setToolTip("");
    checkboxspeki->setToolTip("");
    buttonloadmessk->setToolTip("");
    labeldbmdigitkanal->setToolTip("");
    spdbmdigitkanal->setToolTip("");
    labelhangkanal->setToolTip("");
    sphangkanal->setToolTip("");
    labelattextkanal->setToolTip("");
    spattextkanal->setToolTip("");
    spbarminkanal->setToolTip("");
    spbarmaxkanal->setToolTip("");
    display->setToolTip("");
    lineEdit_frq_anfang->setToolTip("");
    lineEdit_frq_ende->setToolTip("");
    lineEdit_frq_span->setToolTip("");
    lineEdit_frq_mitte->setToolTip("");
    lineEdit_schrittweite->setToolTip("");
    boxztime->setToolTip("");
    label_ztime->setToolTip("");
    grygrenzen->setToolTip("");
    boxdblinie->setToolTip("");
    boxswvlinie1->setToolTip("");
    boxswvlinie2->setToolTip("");
    labelboxswvlinie1->setToolTip("");
    labelboxswvlinie2->setToolTip("");
    checkboxswvlinie1->setToolTip("");
    checkboxswvlinie2->setToolTip("");
    boxalaenge->setToolTip("");
    labelalaenge->setToolTip("");
    boxa_100->setToolTip("");
    labela_100->setToolTip("");
    labeldbmkanal->setToolTip("");
    labelvoltkanal->setToolTip("");
    labelwattkanal->setToolTip("");
    labeladckanal->setToolTip("");
    grdshift->setToolTip("");
  }
}

void Nwt4Window::setFrequenzmarken(){
  QFmarkeDlg *dlg = new QFmarkeDlg(this);

  dlg->setFrqmarken(frqmarken);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    frqmarken=dlg->getFrqmarken();
    display->setFrqMarken(frqmarken);
  }
  delete dlg;
}

void Nwt4Window::KurveInHintergrund(THMessKurve &hmk){
  hmk.mpunkte=messkurve.mpunkte;
  hmk.bkanal=true;
  hmk.bmarker=false;
  hmk.bS11=(messkopf.MkTyp()==mks11) or (messkopf.MkTyp()==mks11var);//S11
  if(wobbelgrunddaten.bdBm){
    hmk.bswv=false;
    hmk.bwatt=wobbelgrunddaten.bwatt;
    hmk.bvolt=wobbelgrunddaten.bvolt;
  }else{
    hmk.bswv=checkboxswvkanal->isChecked();
  }
  for(int i=0;i<hmk.mpunkte;i++){
    hmk.frequenz[i]=messkurve.frequenz[i];
    hmk.db[i]=messkurve.dbk1[i];
  }
  //als Default das Datum eintragen
  if(messkurve.caption.isEmpty()){
    hmk.caption=datetime2filename();
  }else{
    hmk.caption=messkurve.caption;
  }
}

void Nwt4Window::setColorH1(){
  bool ok;
  QColor color = ksettings.penhlinek1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("setColorH1()");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    colorhline1=color;
    ksettings.setcolorhline1(color);
    QPalette palette = buttonc1->palette();
    palette.setColor(QPalette::ButtonText, color);
    buttonc1->setPalette(palette);
    display->setPlotSettings(ksettings);
  }
}

void Nwt4Window::setColorH2(){
  bool ok;
  QColor color = ksettings.penhlinek2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("setColorH2()");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    colorhline2=color;
    ksettings.setcolorhline2(color);
    QPalette palette = buttonc2->palette();
    palette.setColor(QPalette::ButtonText, color);
    buttonc2->setPalette(palette);
    display->setPlotSettings(ksettings);
  }
}

void Nwt4Window::setColorH3(){
  bool ok;
  QColor color = ksettings.penhlinek3.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("setColorH3()");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    colorhline3=color;
    ksettings.setcolorhline3(color);
    QPalette palette = buttonc3->palette();
    palette.setColor(QPalette::ButtonText, color);
    buttonc3->setPalette(palette);
    display->setPlotSettings(ksettings);
  }
}

void Nwt4Window::setColorH4(){
  bool ok;
  QColor color = ksettings.penhlinek4.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("setColorH4()");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    colorhline4=color;
    ksettings.setcolorhline4(color);
    QPalette palette = buttonc4->palette();
    palette.setColor(QPalette::ButtonText, color);
    buttonc4->setPalette(palette);
    display->setPlotSettings(ksettings);
  }
}

void Nwt4Window::setColorH5(){
  bool ok;
  QColor color = ksettings.penhlinek5.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("setColorH5()");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    colorhline5=color;
    ksettings.setcolorhline5(color);
    QPalette palette = buttonc5->palette();
    palette.setColor(QPalette::ButtonText, color);
    buttonc5->setPalette(palette);
    display->setPlotSettings(ksettings);
  }
}

void Nwt4Window::clickbuttone1(){
  QHKurveDlg *dlg = new QHKurveDlg(this);
  dlg->tip(btip);

  QObject::connect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  dlg->setHMesskurve(hmesskurve1);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    hmesskurve1 = dlg->getHMesskurve();
    display->setHMessKurve(hmesskurve1,1);
    checkboxh1->setText(hmesskurve1.caption);
  }
  QObject::disconnect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  delete dlg;
}

void Nwt4Window::clickbuttone2(){
  QHKurveDlg *dlg = new QHKurveDlg(this);
  dlg->tip(btip);

  QObject::connect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  dlg->setHMesskurve(hmesskurve2);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    hmesskurve2 = dlg->getHMesskurve();
    display->setHMessKurve(hmesskurve2,2);
    checkboxh2->setText(hmesskurve2.caption);
  }
  QObject::disconnect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  delete dlg;
}

void Nwt4Window::clickbuttone3(){
  QHKurveDlg *dlg = new QHKurveDlg(this);
  dlg->tip(btip);

  QObject::connect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  dlg->setHMesskurve(hmesskurve3);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    hmesskurve3 = dlg->getHMesskurve();
    display->setHMessKurve(hmesskurve3,3);
    checkboxh3->setText(hmesskurve3.caption);
  }
  QObject::disconnect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  delete dlg;
}

void Nwt4Window::clickbuttone4(){
  QHKurveDlg *dlg = new QHKurveDlg(this);
  dlg->tip(btip);

  QObject::connect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  dlg->setHMesskurve(hmesskurve4);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    hmesskurve4 = dlg->getHMesskurve();
    display->setHMessKurve(hmesskurve4,4);
    checkboxh4->setText(hmesskurve4.caption);
  }
  QObject::disconnect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  delete dlg;
}

void Nwt4Window::clickbuttone5(){
  QHKurveDlg *dlg = new QHKurveDlg(this);
  dlg->tip(btip);

  QObject::connect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  dlg->setHMesskurve(hmesskurve5);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    hmesskurve5 = dlg->getHMesskurve();
    display->setHMessKurve(hmesskurve5,5);
    checkboxh5->setText(hmesskurve5.caption);
  }
  QObject::disconnect( dlg, SIGNAL( savemk(TMessKurve)), this, SLOT(KurvenSichern(TMessKurve)));
  delete dlg;
}

void Nwt4Window::clickbuttonh1(){
  KurveInHintergrund(hmesskurve1);
  display->setHMessKurve(hmesskurve1,1);
  buttone1->setEnabled(true);
  checkboxh1->setEnabled(true);
  checkboxh1->setChecked(true);
  checkboxh1m->setEnabled(true);
  checkboxh1->setText(hmesskurve1.caption);
}

void Nwt4Window::clickbuttonh2(){
  KurveInHintergrund(hmesskurve2);
  display->setHMessKurve(hmesskurve2,2);
  buttone2->setEnabled(true);
  checkboxh2->setEnabled(true);
  checkboxh2->setChecked(true);
  checkboxh2m->setEnabled(true);
  checkboxh2->setText(hmesskurve2.caption);
}

void Nwt4Window::clickbuttonh3(){
  KurveInHintergrund(hmesskurve3);
  display->setHMessKurve(hmesskurve3,3);
  buttone3->setEnabled(true);
  checkboxh3->setEnabled(true);
  checkboxh3->setChecked(true);
  checkboxh3m->setEnabled(true);
  checkboxh3->setText(hmesskurve3.caption);
}

void Nwt4Window::clickbuttonh4(){
  KurveInHintergrund(hmesskurve4);
  display->setHMessKurve(hmesskurve4,4);
  buttone4->setEnabled(true);
  checkboxh4->setEnabled(true);
  checkboxh4->setChecked(true);
  checkboxh4m->setEnabled(true);
  checkboxh4->setText(hmesskurve4.caption);
}

void Nwt4Window::clickbuttonh5(){
  KurveInHintergrund(hmesskurve5);
  display->setHMessKurve(hmesskurve5,5);
  buttone5->setEnabled(true);
  checkboxh5->setEnabled(true);
  checkboxh5->setChecked(true);
  checkboxh5m->setEnabled(true);
  checkboxh5->setText(hmesskurve5.caption);
}

void Nwt4Window::clickbuttonl1(){
  QString w1,w2,zeile;
  int index;
  bool ok, binfo=false;
  double d;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QFile f;
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          hmesskurve1.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)hmesskurve1.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              hmesskurve1.db[index]=d;
              index++;
            }
          }
        }
      }while(!ts.atEnd());
      //index--;
      //qDebug()<<"index:"<<index;
      hmesskurve1.mpunkte=index;
      hmesskurve1.bkanal=true;
      hmesskurve1.bmarker=false;
      if(wobbelgrunddaten.bdBm){
        hmesskurve1.bswv=false;
      }else{
        hmesskurve1.bswv=checkboxswvkanal->isChecked();
      }
      display->setHMessKurve(hmesskurve1,1);
      buttone1->setEnabled(true);
      checkboxh1->setEnabled(true);
      checkboxh1->setChecked(true);
      checkboxh1m->setEnabled(true);
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4k");
      checkboxh1->setText(s1);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this, tr("Wobbel-Einstellungen","WKM Button"), kurvebeschr);
    }
    f.close();
  }
}

void Nwt4Window::clickbuttonl2(){
  QString w1,w2,zeile;
  int index;
  bool ok, binfo=false;
  double d;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QFile f;
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          hmesskurve2.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)hmesskurve2.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              hmesskurve2.db[index]=d;
              index++;
            }
          }
        }
      }while(!ts.atEnd());
      //index--;
      hmesskurve2.mpunkte=index;
      hmesskurve2.bkanal=true;
      hmesskurve2.bmarker=false;
      if(wobbelgrunddaten.bdBm){
        hmesskurve2.bswv=false;
      }else{
        hmesskurve2.bswv=checkboxswvkanal->isChecked();
      }
      display->setHMessKurve(hmesskurve2,2);
      buttone2->setEnabled(true);
      checkboxh2->setEnabled(true);
      checkboxh2->setChecked(true);
      checkboxh2m->setEnabled(true);
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4k");
      checkboxh2->setText(s1);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this,tr("Wobbel-Einstellungen","MessageBox"), kurvebeschr);
    }
    f.close();
  }
}

void Nwt4Window::clickbuttonl3(){
  QString w1,w2,zeile;
  int index;
  bool ok, binfo=false;
  double d;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QFile f;
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          hmesskurve3.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)hmesskurve3.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              hmesskurve3.db[index]=d;
              index++;
            }
          }
        }
      }while(!ts.atEnd());
      //index--;
      hmesskurve3.mpunkte=index;
      hmesskurve3.bkanal=true;
      hmesskurve3.bmarker=false;
      if(wobbelgrunddaten.bdBm){
        hmesskurve3.bswv=false;
      }else{
        hmesskurve3.bswv=checkboxswvkanal->isChecked();
      }
      display->setHMessKurve(hmesskurve3,3);
      buttone3->setEnabled(true);
      checkboxh3->setEnabled(true);
      checkboxh3->setChecked(true);
      checkboxh3m->setEnabled(true);
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4k");
      checkboxh3->setText(s1);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this,tr("Wobbel-Einstellungen","MessageBox"), kurvebeschr);
    }
    f.close();
  }
}

void Nwt4Window::clickbuttonl4(){
  QString w1,w2,zeile;
  int index;
  bool ok, binfo=false;
  double d;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QFile f;
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          hmesskurve4.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)hmesskurve4.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              hmesskurve4.db[index]=d;
              index++;
            }
          }
        }
      }while(!ts.atEnd());
      //index--;
      hmesskurve4.mpunkte=index;
      hmesskurve4.bkanal=true;
      hmesskurve4.bmarker=false;
      if(wobbelgrunddaten.bdBm){
        hmesskurve4.bswv=false;
      }else{
        hmesskurve4.bswv=checkboxswvkanal->isChecked();
      }
      display->setHMessKurve(hmesskurve4,4);
      buttone4->setEnabled(true);
      checkboxh4->setEnabled(true);
      checkboxh4->setChecked(true);
      checkboxh4m->setEnabled(true);
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4k");
      checkboxh4->setText(s1);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this,tr("Wobbel-Einstellungen","MessageBox"), kurvebeschr);
    }
    f.close();
  }
}

void Nwt4Window::clickbuttonl5(){
  QString w1,w2,zeile;
  int index;
  bool ok, binfo=false;
  double d;

  QSettings settings("AFU", settingspath);
  //QSettings settings("AFU", "NWT2");
  QDir path;
  path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

  QFile f;
  QString s2 = tr("NWT4000 Kurven-Datei (*.n4k)","FileDialog");
  QString s1 = "*.n4k";
  QString s = QFileDialog::getOpenFileName(this, tr("NWT4000 Kurven-Datei laden","FileDialog"),
                                           path.filePath(s1), s2);
  if (!s.isNull()){
    f.setFileName(grunddaten.homedir.filePath(s));
    if(f.open(QIODevice::ReadOnly)){
      kurvebeschr="";//Info String loeschen
      QTextStream ts(&f);
      index=0;
      do{
        ts >> zeile;
        if(zeile.left(1)=="#"){
          zeile.remove(0,1);//# entfernen
          hmesskurve5.caption=zeile;
          //qDebug()<<zeile;
        }else{
          if(zeile.left(3)=="$$$"){
            binfo=true;
            zeile.remove(0,3);//$$$ entfernen
            zeile+="; ";
            kurvebeschr+=zeile;//Info eintragen
          }else{
            w1 = zeile.section(';',0,0);
            w2 = zeile.section(';',1,1);
            d=w1.toDouble(&ok);
            if(ok)hmesskurve5.frequenz[index]=d;
            d=w2.toDouble(&ok);
            if(ok){
              hmesskurve5.db[index]=d;
              index++;
            }
          }
        }
      }while(!ts.atEnd());
      //index--;
      hmesskurve5.mpunkte=index;
      hmesskurve5.bkanal=true;
      hmesskurve5.bmarker=false;
      if(wobbelgrunddaten.bdBm){
        hmesskurve5.bswv=false;
      }else{
        hmesskurve5.bswv=checkboxswvkanal->isChecked();
      }
      display->setHMessKurve(hmesskurve5,5);
      buttone5->setEnabled(true);
      checkboxh5->setEnabled(true);
      checkboxh5->setChecked(true);
      checkboxh5m->setEnabled(true);
      QDir dir(s);
      s1 = dir.dirName();
      s1.remove(".n4k");
      checkboxh5->setText(s1);
      path.setPath(s);
      s=path.dirName();
      s1=path.absolutePath();
      s1.remove(s);
      settings.setValue("kurvenpath",s1);
      kurvebeschr.replace('_', ' ');
      if(binfo)QMessageBox::information(this, tr("Wobbel-Einstellungen","MessageBox"), kurvebeschr);
    }
    f.close();
  }
}

void Nwt4Window::checkboxh1_checked(bool b){
  hmesskurve1.bkanal=b;
  display->setHMessKurve(hmesskurve1,1);
}

void Nwt4Window::checkboxh2_checked(bool b){
  hmesskurve2.bkanal=b;
  display->setHMessKurve(hmesskurve2,2);
}

void Nwt4Window::checkboxh3_checked(bool b){
  hmesskurve3.bkanal=b;
  display->setHMessKurve(hmesskurve3,3);
}

void Nwt4Window::checkboxh4_checked(bool b){
  hmesskurve4.bkanal=b;
  display->setHMessKurve(hmesskurve4,4);
}

void Nwt4Window::checkboxh5_checked(bool b){
  hmesskurve5.bkanal=b;
  display->setHMessKurve(hmesskurve5,5);
}

void Nwt4Window::checkboxh1m_checked(bool b){
  hmesskurve1.bmarker=b;
  display->setHMessKurve(hmesskurve1,1);
}

void Nwt4Window::checkboxh2m_checked(bool b){
  hmesskurve2.bmarker=b;
  display->setHMessKurve(hmesskurve2,2);
}

void Nwt4Window::checkboxh3m_checked(bool b){
  hmesskurve3.bmarker=b;
  display->setHMessKurve(hmesskurve3,3);
}

void Nwt4Window::checkboxh4m_checked(bool b){
  hmesskurve4.bmarker=b;
  display->setHMessKurve(hmesskurve4,4);
}

void Nwt4Window::checkboxh5m_checked(bool b){
  hmesskurve5.bmarker=b;
  display->setHMessKurve(hmesskurve5,5);
}

void Nwt4Window::infoh1(){
  QString sinfo="Keine Beschreibung!";
  if(!hmesskurve1.caption.isEmpty())sinfo=hmesskurve1.caption;
  QMessageBox::information( this, tr("Kurven Caption","in Dialogbox"), sinfo);
}

void Nwt4Window::infoh2(){
  QString sinfo="Keine Beschreibung!";
  if(!hmesskurve2.caption.isEmpty())sinfo=hmesskurve2.caption;
  QMessageBox::information( this, tr("Kurven Caption","in Dialogbox"), sinfo);
}

void Nwt4Window::infoh3(){
  QString sinfo="Keine Beschreibung!";
  if(!hmesskurve3.caption.isEmpty())sinfo=hmesskurve3.caption;
  QMessageBox::information( this, tr("Kurven Caption","in Dialogbox"), sinfo);
}

void Nwt4Window::infoh4(){
  QString sinfo="Keine Beschreibung!";
  if(!hmesskurve4.caption.isEmpty())sinfo=hmesskurve4.caption;
  QMessageBox::information( this, tr("Kurven Caption","in Dialogbox"), sinfo);
}

void Nwt4Window::infoh5(){
  QString sinfo="Keine Beschreibung!";
  if(!hmesskurve5.caption.isEmpty())sinfo=hmesskurve5.caption;
  QMessageBox::information( this, tr("Kurven Caption","in Dialogbox"), sinfo);
}

void Nwt4Window::kurveCaption(){
  bool ok;
  QString qs;

  //als Default das Datum eintragen
  if(messkurve.caption.isEmpty()){
    qs=datetime2filename();
  }else{
    qs=messkurve.caption;
  }

  QString text = QInputDialog::getText(this, tr("Kurve Beschreibung"),
                                       tr("Beschreibung:"), QLineEdit::Normal,
                                       qs, &ok);
  if (ok){
    text.replace(' ', '_');
    messkurve.caption=text;//Caption in Messkurve eintragen
    display->setMessKurve(messkurve);//Caption in Display eintragen
  }
}

void Nwt4Window::setSetup(){
  TGrunddaten grunddatenalt;
  TWobbelGrundDaten wobbelgrunddatenalt;
  bool warncalfrq=false;

  setupdlg *dlg = new setupdlg(this);
  //Verbindungen dlg->NWT Firmware hestellen
  QObject::connect( dlg, SIGNAL( setNWTTakt(double)), this, SLOT(setNWTTakt(double)));
  QObject::connect( dlg, SIGNAL( setVfo(double)), this, SLOT(setVFONWT(double)));
  QFont f;

  // alte Grunddaten merken
  grunddatenalt=grunddaten;
  wobbelgrunddatenalt=wobbelgrunddaten;

  //Grunddaten in den DLG kopieren
  f=dlg->font();
  f.setPixelSize(grunddaten.fontsize);
  if(grunddaten.bfontsize)dlg->setFont(f);
  dlg->setgrunddaten(grunddaten);
  dlg->setwobdaten(wobbelgrunddaten);
  dlg->tip(btip);
  int r = dlg->exec();
  if(r == QDialog::Accepted){
    //OK gedrueckt
    bsetup=true;
    grunddaten=dlg->getgrunddaten();//Daten vom SETUP abholen
    mCalibFlat->setDisabled(grunddaten.bflatcal);
    label_ztime->setEnabled(!grunddaten.bnozwtime);
    boxztime->setEnabled(!grunddaten.bnozwtime);
    if(grunddaten.bnozwtime)boxztime->setValue(0);

    wobbelgrunddaten=dlg->getwobdaten();//Daten vom SETUP abholen
    //qDebug()<<"messkopf.CalFrqmin():"<<messkopf.CalFrqmin();
    //qDebug()<<"messkopf.CalFrqmax():"<<messkopf.CalFrqmax();
    //qDebug()<<"wobbelgrunddaten.calfrqmin:"<<wobbelgrunddaten.calfrqmin;
    //qDebug()<<"wobbelgrunddaten.calfrqmax:"<<wobbelgrunddaten.calfrqmax;
    //qDebug()<<"grunddaten.filemk:"<<grunddaten.filemk;
    if(grunddaten.filemk == "Messkopf"){//noch kein Messkopf generiert, "Messkopf" == default Name
      messkopf.setCalFrqmin(wobbelgrunddaten.calfrqmin);//Frequenzgrenzen auch im Messkopf setzen
      messkopf.setCalFrqmax(wobbelgrunddaten.calfrqmax);
    }
    if(messkopf.CalFrqmin() < wobbelgrunddaten.calfrqmin)warncalfrq=true;
    if(messkopf.CalFrqmax() > wobbelgrunddaten.calfrqmax)warncalfrq=true;
    if(warncalfrq)
      QMessageBox::information(this, tr("Arbeitsfrequenz"), tr("<b>Die Frequenzgrenzen im Messkopf ueberschreiten die Grenzwerte des NWT!</b><br>"
                                                               "Bitte einen neuen Messkopf erzeugen oder einen anderen Messkopf laden."));
    //Colordaten in das Display kopieren
    ksettings.setcolorhintergrund(grunddaten.colorhintergrund);
    ksettings.setcolorschrift(grunddaten.colorschrift);
    ksettings.setcolormarkerschrift(grunddaten.colormarkerschrift);
    ksettings.setcolorlinek(grunddaten.colorlinie);
    ksettings.setcolorlinekmarker(grunddaten.colorliniemarker);
    ksettings.setcolorfmarken(grunddaten.colorfmarken);
    ksettings.setmarkerfontsize(grunddaten.markerfontsize);
    ksettings.setfontsize(grunddaten.fontsize);
    ksettings.setcolorspur(grunddaten.colorwobbelspur);
    ksettings.set0dBLine(wobbelgrunddaten.b0dBline);
    display->setPlotSettings(ksettings);
    if(grunddaten.bfontsize)setFonts();
    bsetup=false;
    wobnormalisieren();
    setAText(srs232);
  }else{
    // SETUP Abbruch; alte Daten wieder zurueck speichern
    grunddaten=grunddatenalt;
    wobbelgrunddaten=wobbelgrunddatenalt;
  }
  // Alle SLOTs für das SETUP wieder trennen
  QObject::disconnect( dlg, SIGNAL( setNWTTakt(double)), this, SLOT(setNWTTakt(double)));
  QObject::disconnect( dlg, SIGNAL( setVfo(double)), this, SLOT(setVFONWT(double)));
  // alles wieder richtig einstellen, je nach aktiven Widget
  tabumschalten(tabwidget->currentIndex());
  delete dlg;
}

void Nwt4Window::setNWTTakt(double d){
  grunddaten.nwttakt=d;
  ddstakttimer->start(1000);
}

void Nwt4Window::setVFONWT(double d){
  grunddaten.nwttaktcalfrq=d;
  svfotimer->start(1000);
}

void Nwt4Window::kdaten(Tkabeldaten kd){
  kabeldaten = kd;
  /*
  qDebug()<<"kabeldaten.ca"<<kabeldaten.ca;
  qDebug()<<"kabeldaten.dk"<<kabeldaten.dk;
  qDebug()<<"kabeldaten.laenge"<<kabeldaten.laenge;
  qDebug()<<"kabeldaten.lfrequenz"<<kabeldaten.lfrequenz;
  qDebug()<<"kabeldaten.vf"<<kabeldaten.vf;
  qDebug()<<"kabeldaten.z"<<kabeldaten.z;
  */
  mKabeldaten->setEnabled(true);
}

//Einstellung des DDS-Takt vom SETUP aus
void Nwt4Window::ddstakttimerstop(){
  double dkonst;
  long double ddskonst;
  qulonglong test;
  QString qs;
  QString qpll;

  ddstakttimer->stop();
  qpll="00";
  dkonst = 4294967296.0;  // 2 hoch 32
  ddskonst = dkonst * dkonst / grunddaten.nwttakt;
  test = qulonglong(ddskonst);
  qs = QString("%1").arg(test,0,16);
  if(qs.length() < 10)qs="0"+qs;
  //qDebug("wgrunddaten.ddstakt:%f", grunddaten.nwttakt);
  //qDebug("test:%lli", test);
  qs = qs.toUpper();
  qs = "e" + qs + qpll;
  //qDebug()<<"Befehl:"<<qs;
  if(bnwtgefunden)picmodem->writeChar(0x8F);
  if(bnwtgefunden)picmodem->writeLine(qPrintable(qs));
}

//Steuerung des DDS vom SETUP aus
void Nwt4Window::svfotimerstop(){
  QString qs;

  svfotimer->stop();
  qs.sprintf("f%09.0f",grunddaten.nwttaktcalfrq / 10.0);
  if(bnwtgefunden)picmodem->writeChar(0x8F);
  if(bnwtgefunden)picmodem->writeLine(qPrintable(qs));
  catbefehl='f';
  //qDebug()<<qs;
}

void Nwt4Window::setmessvfo()
{
  int i;
  double fr;
  QObject::disconnect( sp1hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp10hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp100hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp1khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp10khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp100khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp1mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp10mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp100mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::disconnect( sp1ghz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));

  if(bd)qDebug("setmessvfo()");
  if(sp1hz->value()==10){
    if(bd)qDebug("setmessvfo(1)");
    sp1hz->setValue(0);
    i = sp10hz->value();
    i++;
    sp10hz->setValue(i);
  }
  if(sp10hz->value()==10){
    if(bd)qDebug("setmessvfo(2)");
    sp10hz->setValue(0);
    i = sp100hz->value();
    i++;
    sp100hz->setValue(i);
  }
  if(sp100hz->value()==10){
    if(bd)qDebug("setmessvfo(3)");
    sp100hz->setValue(0);
    i = sp1khz->value();
    i++;
    sp1khz->setValue(i);
  }
  if(sp1khz->value()==10){
    if(bd)qDebug("setmessvfo(4)");
    sp1khz->setValue(0);
    i = sp10khz->value();
    i++;
    sp10khz->setValue(i);
  }
  if(sp10khz->value()==10){
    if(bd)qDebug("setmessvfo(5)");
    sp10khz->setValue(0);
    i = sp100khz->value();
    i++;
    sp100khz->setValue(i);
  }
  if(sp100khz->value()==10){
    if(bd)qDebug("setmessvfo(6)");
    sp100khz->setValue(0);
    i = sp1mhz->value();
    i++;
    sp1mhz->setValue(i);
  }
  if(sp1mhz->value()==10){
    if(bd)qDebug("setmessvfo(7)");
    sp1mhz->setValue(0);
    i = sp10mhz->value();
    i++;
    sp10mhz->setValue(i);
  }
  if(sp10mhz->value()==10){
    if(bd)qDebug("setmessvfo(8)");
    sp10mhz->setValue(0);
    i = sp100mhz->value();
    i++;
    sp100mhz->setValue(i);
  }
  if(sp100mhz->value()==10){
    if(bd)qDebug("setmessvfo(9)");
    sp100mhz->setValue(0);
    i = sp1ghz->value();
    i++;
    sp1ghz->setValue(i);
  }
  fr = getMessVfoFrq();
  if(fr < 0.0){
    if(sp1hz->value()==-1)sp1hz->setValue(0);
    if(sp10hz->value()==-1)sp10hz->setValue(0);
    if(sp100hz->value()==-1)sp100hz->setValue(0);
    if(sp1khz->value()==-1)sp1khz->setValue(0);
    if(sp10khz->value()==-1)sp10khz->setValue(0);
    if(sp100khz->value()==-1)sp100khz->setValue(0);
    if(sp1mhz->value()==-1)sp1mhz->setValue(0);
    if(sp10mhz->value()==-1)sp10mhz->setValue(0);
    if(sp100mhz->value()==-1)sp100mhz->setValue(0);
    if(sp1ghz->value()==-1)sp1ghz->setValue(0);
  }
  if(sp1hz->value()==-1){
    if(bd)qDebug("setmessvfo(10)");
    sp1hz->setValue(9);
    i = sp10hz->value();
    i--;
    sp10hz->setValue(i);
  }
  if(sp10hz->value()==-1){
    if(bd)qDebug("setmessvfo(11)");
    sp10hz->setValue(9);
    i = sp100hz->value();
    i--;
    sp100hz->setValue(i);
  }
  if(sp100hz->value()==-1){
    if(bd)qDebug("setmessvfo(12)");
    sp100hz->setValue(9);
    i = sp1khz->value();
    i--;
    sp1khz->setValue(i);
  }
  if(sp1khz->value()==-1){
    if(bd)qDebug("setmessvfo(13)");
    sp1khz->setValue(9);
    i = sp10khz->value();
    i--;
    sp10khz->setValue(i);
  }
  if(sp10khz->value()==-1){
    if(bd)qDebug("setmessvfo(14)");
    sp10khz->setValue(9);
    i = sp100khz->value();
    i--;
    sp100khz->setValue(i);
  }
  if(sp100khz->value()==-1){
    if(bd)qDebug("setmessvfo(15)");
    sp100khz->setValue(9);
    i = sp1mhz->value();
    i--;
    sp1mhz->setValue(i);
  }
  if(sp1mhz->value()==-1){
    if(bd)qDebug("setmessvfo(16)");
    sp1mhz->setValue(9);
    i = sp10mhz->value();
    i--;
    sp10mhz->setValue(i);
  }
  if(sp10mhz->value()==-1){
    if(bd)qDebug("setmessvfo(17)");
    sp10mhz->setValue(9);
    i = sp100mhz->value();
    i--;
    sp100mhz->setValue(i);
  }
  if(sp100mhz->value()==-1){
    if(bd)qDebug("setmessvfo(18)");
    sp100mhz->setValue(9);
    i = sp1ghz->value();
    i--;
    sp1ghz->setValue(i);
  }
  if(sp1ghz->value()==-1){
    if(bd)qDebug("setmessvfo(19)");
    sp1ghz->setValue(0);
  }
  fr = getMessVfoFrq();
  messvfotimer->start(100);
  //qDebug()<<"setmessvfo()  fr:"<<fr;
  QObject::connect( sp1hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp10hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp100hz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp1khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp10khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp100khz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp1mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp10mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp100mhz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
  QObject::connect( sp1ghz, SIGNAL( valueChanged(int)), this, SLOT(setmessvfo()));
}

void Nwt4Window::setMessVfoFrq(double d){
  double fr=d;
  int a;
  //qDebug()<<"setMessVfoFrq(double d)="<<fr;
  a = int(fr/1000000000.0);
  sp1ghz->setValue(a);
  fr = fr - (double(a * 1000000000.0));
  a = int(fr/100000000.0);
  sp100mhz->setValue(a);
  fr = fr - (double(a * 100000000.0));
  a = int(fr/10000000.0);
  sp10mhz->setValue(a);
  fr = fr - (double(a * 10000000.0));
  a = int(fr/1000000.0);
  sp1mhz->setValue(a);
  fr = fr - (double(a * 1000000.0));
  a = int(fr/100000.0);
  sp100khz->setValue(a);
  fr = fr - (double(a * 100000.0));
  a = int(fr/10000.0);
  sp10khz->setValue(a);
  fr = fr - (double(a * 10000.0));
  a = int(fr/1000.0);
  sp1khz->setValue(a);
  fr = fr - (double(a * 1000.0));
  a = int(fr/100.0);
  sp100hz->setValue(a);
  fr = fr - (double(a * 100.0));
  a = int(fr/10.0);
  sp10hz->setValue(a);
  fr = fr - (double(a * 10.0));
  sp1hz->setValue(int(fr));
}

//Frequenz in Hz
double Nwt4Window::getMessVfoFrq(){
  if(bd)qDebug()<<"Nwt4Window::getMessVfoFrq()";
  double fr = sp1hz->value() +
              (sp10hz->value()   * 10.0) +
              (sp100hz->value()  * 100.0) +
              (sp1khz->value()   * 1000.0) +
              (sp10khz->value()  * 10000.0) +
              (sp100khz->value() * 100000.0) +
              (sp1mhz->value()   * 1000000.0) +
              (sp10mhz->value()  * 10000000.0) +
              (sp100mhz->value() * 100000000.0) +
              (sp1ghz->value()   * 1000000000.0);
  if(bd)qDebug()<<"ENDE Nwt4Window::getMessVfoFrq()";
  //qDebug()<<"getMessVfoFrq()="<<fr;
  return fr;
}

void Nwt4Window::messvfotimerstop(){
  double fr = getMessVfoFrq();

  messvfotimer->stop();
  QString qs;
  qs.sprintf("f%09.0f",fr/10.0);
  if(bnwtgefunden){
    picmodem->writeChar(0x8f);
    picmodem->writeLine(qPrintable(qs));
    catbefehl='f';
    qs = "messvfotimer: "+qs;
    //qDebug()<<bnwtgefunden;
    //qDebug(qPrintable(qs));
  }
  fr=fr/1000000.0;
  int index=round(fr*10)-1;
  if(index>=maxmesspunkte)index=maxmesspunkte-1;
  //qDebug("index:%i",index);
}

void Nwt4Window::tabumschalten(int wg){

  if(wg==0 or wg==2 or wg==3){
    if(wg==0){
      delay(500);
    }
    bmessenmw=false;//mW-Meter AUS
    kurvenToolBar->setEnabled(true);
    menuKurven->setEnabled(true);
    mNeuK1->setEnabled(true);
    mCalibK1->setEnabled(true);
    mSondeSpeichernK1->setEnabled(true);
    mNoFrqCalK1->setEnabled(true);
    mSpeichernpng->setEnabled(true);
    mSetup->setEnabled(true);
    mCaldBm->setEnabled(true);
    mCalmWm->setEnabled(false);
    mhangtimer->stop();
    set0hz();
    wachtimer->stop();//Datenfluss Ueberwachung STOP
    messvfotimer->stop();
  }
  if(wg==1){
    if(wg!=0)wobbeinmal=true;
    delay(1000);
    kurvenToolBar->setEnabled(false);
    menuKurven->setEnabled(false);
    mNeuK1->setEnabled(false);
    mCalibK1->setEnabled(false);
    mSondeSpeichernK1->setEnabled(true);
    mCaldBm->setEnabled(false);
    mCalmWm->setEnabled(true);
    mNoFrqCalK1->setEnabled(false);
    mSpeichernpng->setEnabled(false);
    progressbarkanal->setValue(-850);
    messvfotimerstop();
    bmessenmw=true;//mW-Meter ein
    bbefwobbeln=false;
    //Messung mW-Meter starten
    clrrxbuffer();//RS232 RX Buffer loeschen
    if(bnwtgefunden){
      picmodem->writeChar(0x8F);
      picmodem->writeChar('m');
      catbefehl='m';
    }
    //qDebug()<<'m';
    befantworttimer->start(10);//Antwort nach 10mSek zu erwarten
    wachtimer->stop();
    wachtimer->start(500);//Datenfluss Ueberwachung starten 500mSek
    mhangtimer->start(hangtk1);
  }
  actrepainttimer();//Windows neu darstellen;
}

void Nwt4Window::wachtimerstop(){
  QString qs;

  wachtimer->stop();
  //Messung mW-Meter neu starten
  //qDebug()<<"wachtimerstop()"<<qs;
  clrrxbuffer();//RS232 RX Buffer loeschen
  if(bnwtgefunden){
    picmodem->writeChar(0x8F);
    picmodem->writeChar('m');
    catbefehl='m';
  }
  befantworttimer->start(10);//Antwort nach 10mSek zu erwarten
  wachtimer->start(500);//Datenfluss Ueberwachung parallel mit starten 500mSek
  mhangtimer->start(hangtk1);
}

void Nwt4Window::setbarkanal(){
  int max=spbarmaxkanal->value();
  int min=spbarminkanal->value();
  if(max<=min){
    if(max<5)max=min+5;
    spbarminkanal->setValue(min);
    spbarmaxkanal->setValue(max);
  }
  progressbarkanal->setMinimum(min*10);
  progressbarkanal->setMaximum(max*10);
}

void Nwt4Window::setMessLabel1(){
  double volt, mwatt, swv;
  QString qs,einh,qs1;
  QString beschr;
  double dbm, rldb;
  bool ok;

  if(checkkanal->isChecked()){
    //Progressbar Balken anzeigen
    //mittelwert *10 = Balken
    progressbarkanal->setValue(int(mdbmK1anzeige*10.0));
    //index++, 00..99
    mdbmk1idx++;if(mdbmk1idx>99)mdbmk1idx=0;
    //messwert in messreihe einfügen
    messreiheK1[mdbmk1idx]=mdbmK1;
    messreiheADC[mdbmk1idx]=adckanal;
    //mittelwert umrechnen in rldb ReturnLoss, Kalibrierpegel wieder abziehen
    rldb=mdbmK1anzeige;
    //qDebug()<<"att_rldb:"<<att_rldb;
    swv=db2swv(rldb);
    //mittelwert + externer ATT = dbm
    dbm=mdbmK1anzeige+messattext;//
    //qDebug()<<"dbm:"<<dbm;
    //dbm in Leistung umrechnen
    mwatt = pow(10.0 , dbm/10.0);
    //Leistung in Spannung umrechnen
    volt = sqrt(mwatt * 0.05);
    if(bdbmk1){
      einh="dBm";
    }else{
      dbm-=13.01029996;
      einh="dBV";
    }
    int p=spdbmdigitkanal->value();
    if(messkopf.MkTyp()==mks11 or messkopf.MkTyp()==mks11var){
      if(p==0)qs.sprintf("RL: %3.0f dB",rldb);
      if(p==1)qs.sprintf("RL: %3.1f dB",rldb);
      if(p==2)qs.sprintf("RL: %3.2f dB",rldb);
      if(p==3)qs.sprintf("RL: %3.3f dB",rldb);
      labeldbmkanal->setText(qs);
      if(p==0)qs1.sprintf("%2.0f",swv);
      if(p==1)qs1.sprintf("%2.1f",swv);
      if(p==2)qs1.sprintf("%2.2f",swv);
      if(p==3)qs1.sprintf("%2.3f",swv);
      qs=tr("SWV: %1").arg(qs1);
      labelvoltkanal->setText(qs);
      labelwattkanal->setText("");
    }else{
      if(p==0)qs.sprintf("%3.0f ",dbm);
      if(p==1)qs.sprintf("%3.1f ",dbm);
      if(p==2)qs.sprintf("%3.2f ",dbm);
      if(p==3)qs.sprintf("%3.3f ",dbm);
      labeldbmkanal->setText(qs+einh);
      if(bdbmk1){
        qs=mwatt2str(mwatt);
        labelwattkanal->setText(qs);
      }else{
        qs=volt2str(volt*sqrt(2));
        labelwattkanal->setText(qs+"s");
      }
      qs=volt2str(volt);
      labelvoltkanal->setText(qs);
    }
    qs.sprintf("%i",adckanal);
    labeladckanal->setText(qs);
    if(bmwcalp2){
      bmwcalp2=false;
      ymkwert2mw=mwadcwert;
      qDebug("Pegel1: %1.3f dBm",mwcalpegel1);
      qDebug("Pegel2: %1.3f dBm",mwcalpegel2);
      qDebug("ADC1: %1.3f",ymkwert1mw);
      qDebug("ADC2: %1.3f",ymkwert2mw);
      double mkxmw = (mwcalpegel1 - mwcalpegel2)/(ymkwert1mw - ymkwert2mw);
      double mkymw = (ymkwert1mw * mkxmw * -1.0) + mwcalpegel1;
      qDebug("mkx: %1.8f",mkxmw);
      qDebug("mky: %1.8f",mkymw);
      messkopf.setMkXYmWMeter(mkxmw,mkymw);
      setMkInfo();
      beschr = tr("<b><u>Kalibrieren mW-Meter</u></b><br>"
                      "Speichern des Kalibrierergebnis<br>"
                      "in der Messkopfdatei:<br>"
                      "<b>%1</b>").arg(grunddaten.filemk);
      if(QMessageBox::information(this,"Kalibrieren mW-Meter",beschr,QMessageBox::Save,QMessageBox::Cancel)==QMessageBox::Save){
        saveMk(grunddaten.filemk);
      }
    }
    if(bmwcalp1){
      bmwcalp1=false;
      ymkwert1mw=mwadcwert;
      mwcalpegel2=30.0;
      beschr = tr("<b><u>Kalibrieren mW-Meter</u></b><br>"
                      "1. Den Pegel mit einem Daempfungsglied verringern.<br>"
                      "   Das Daempfungsglied sollte min. 20dB betragen.<br>"
                      "   <br><br>"
                      "Wert des Daempfungsgliedes (dB):","InformationsBox");
      mwcalpegel2 = QInputDialog::getDouble(this, tr("Kalibrieren mW-Meter","InputBox"),
                                                beschr, mwcalpegel2, 20, 90, 2, &ok);
      mwcalpegel2 = mwcalpegel1 - mwcalpegel2; //Daempfungsglied in dBm umrechnen.
      if(ok){
        bmwcalp2 = true;
      }
    }
  }else{
    int min=progressbarkanal->minimum();
    progressbarkanal->setValue(min);
    labeldbmkanal->setText("");
    labelwattkanal->setText("");
    labelvoltkanal->setText("");
    labeladckanal->setText("");
  }
}

QString Nwt4Window::mwatt2str(double d){
  QString qs="0.0 ";
  QString mh="";
  double w=0.0;
  bool g=true;

  if(g and d>1000000.0){w=d/1000000.0;mh="kW";g=false;}
  if(g and d>1000.0){w=d/1000.0;mh="W";g=false;}
  if(g and d>1){w=d;mh="mW";g=false;}
  if(g and d>0.001){w=d*1000.0;mh="uW";g=false;}
  if(g and d>0.000001){w=d*1000000.0;mh="nW";g=false;}
  if(g and d>0.000000001){w=d*1000000000.0;mh="pW";g=false;}
  if(g and d>0.000000000001){w=d*1000000000000.0;mh="fW";g=false;}
  if(g and d>0.000000000000001){w=d*1000000000000000.0;mh="aW";g=false;}

  if(w>100){
    qs.sprintf("%3.1f ",w);
    return qs+mh;
  }
  if(w>10){
    qs.sprintf("%2.2f ",w);
    return qs+mh;
  }
  if(w>1){
    qs.sprintf("%1.3f ",w);
    return qs+mh;
  }
  return qs+mh;
}

QString Nwt4Window::volt2str(double d){
  QString qs="0.0 ";
  QString mh="";
  double w=0.0;
  bool g=true;

  if(g and d>1){w=d;mh="V";g=false;}
  if(g and d>0.001){w=d*1000.0;mh="mV";g=false;}
  if(g and d>0.000001){w=d*1000000.0;mh="uV";g=false;}
  if(g and d>0.000000001){w=d*1000000000.0;mh="nV";g=false;}
  if(w>100){
    qs.sprintf("%3.1f ",w);
    return qs+mh;
  }
  if(w>10){
    qs.sprintf("%2.2f ",w);
    return qs+mh;
  }
  if(w>1){
    qs.sprintf("%1.3f ",w);
    return qs+mh;
  }
  return qs+mh;
}

void Nwt4Window::mhangk1timerstop(){
  mhangtimer->stop();
  double mittelwert=0.0;
  double mittelwertADC=0.0;

  for(int i=0;i<100;i++){
    mittelwert+=messreiheK1[i];
    mittelwertADC+=messreiheADC[i];
  }
  //aus messreihe mittelwert bilden = mdbmK1anzeige
  mdbmK1anzeige = mittelwert/100.0;
  mwadcwert = mittelwertADC/100.0;
  mhangtimer->start(hangtk1);
}

void Nwt4Window::setsphang(int i){
  hangtk1=i;
  mhangtimer->stop();
  mhangtimer->start(hangtk1);
}

void Nwt4Window::checkmwkanal_checked(bool b){
  progressbarkanal->setEnabled(b);
  spbarminkanal->setEnabled(b);
  spbarmaxkanal->setEnabled(b);
  labeldbmkanal->setEnabled(b);
  labelvoltkanal->setEnabled(b);
  labelwattkanal->setEnabled(b);
  sphangkanal->setEnabled(b);
  labelhangkanal->setEnabled(b);
  spdbmdigitkanal->setEnabled(b);
  labeldbmdigitkanal->setEnabled(b);
  if(messkopf.MkTyp() == mks21 or messkopf.MkTyp()==mks21var){
    spattextkanal->setEnabled(b);
    labelattextkanal->setEnabled(b);
  }
  grunddaten.bmwmeter=b;
}

void Nwt4Window::setspattext(int i){
  messattext=i;
  //qDebug()<<messattext;
  setMessLabel1();
}

void Nwt4Window::setalaenge(double d){
  a_laenge=d;
  messkurve.adaempfung=a_100 * a_laenge/100.0;
  setInfoText();
  display->setMessKurve(messkurve);
}

void Nwt4Window::seta_100(double d){
  a_100=d;
  messkurve.adaempfung=a_100 * a_laenge/100.0;
  setInfoText();
  display->setMessKurve(messkurve);
}

double Nwt4Window::db2swv(double d){
  double db=d;
  double ref;
  double swv;
  ref=pow(10.0,db/20.0);
  swv=(1.0+ref)/(1.0-ref);
  return swv;
}

void Nwt4Window::alwindungen()
{
  QString sal;
  QString sl;
  QString sw;
  double al;
  double l;
  double w;
  bool ok;

  sal = edital3->text();
  al = sal.toDouble(&ok);
  if(!ok)return;
  sl = editl3->text();
  l = sl.toDouble(&ok);
  if(!ok)return;
  l = l * 1000.0;
  //qDebug("l=%g",l);
  //qDebug("al=%g",al);
  w = sqrt(l/al);
  //qDebug("al * l=%g",al * l);
  //qDebug("w=%g",w);
  sw = QString("%1").arg(w);
  if((sal.length() > 0) and (sl.length() > 0))ergebnisw->setText(sw);
}

void Nwt4Window::schwingkreisc()
{
  QString sf;
  QString sc;
  QString sl;
  QString sxc;
  QString sxl;
  double f;
  double l;
  double c;
  double xc;
  double xl;
  bool ok;

  sf = editf1->text();
  f = sf.toDouble(&ok);
  if(!ok)return;
  sc = editc1->text();
  c = sc.toDouble(&ok);
  if(!ok)return;

  schwingkreis=kapazitiv;
  f = f * 1000000.0;
  c = c / 1000000000000.0;
  l = 1/(4.0*M_PI*M_PI*c*f*f);
  xc = 1.0/(2.0*M_PI*f*c);
  xl = 2.0*M_PI*f*l;
  l = l * 1000000.0;
  sl = QString("%1").arg(l);
  QObject::disconnect( editl1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisl()));
  editl1->setText(sl);
  QObject::connect( editl1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisl()));
  sxc = QString("%1").arg(xc);
  ergebnisxc->setText(sxc);
  sxl = QString("%1").arg(xl);
  ergebnisxl->setText(sxl);
  schwingkreisal();
}

void Nwt4Window::schwingkreisl()
{
  QString sf;
  QString sc;
  QString sl;
  QString sxc;
  QString sxl;
  double f;
  double l;
  double c;
  double xc;
  double xl;
  bool ok;

  sf = editf1->text();
  f = sf.toDouble(&ok);
  if(!ok)return;
  sl = editl1->text();
  l = sl.toDouble(&ok);
  if(!ok)return;

  schwingkreis=induktiv;
  f = f * 1000000.0;
  l = l / 1000000.0;
  c = 1.0/(4.0*M_PI*M_PI*l*f*f);
  xc = 1.0/(2.0*M_PI*f*c);
  xl = 2.0*M_PI*f*l;
  c = c * 1000000000000.0;
  sc = QString("%1").arg(c);
  QObject::disconnect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisc()));
  editc1->setText(sc);
  QObject::connect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisc()));
  sxc = QString("%1").arg(xc);
  ergebnisxc->setText(sxc);
  sxl = QString("%1").arg(xl);
  ergebnisxl->setText(sxl);
  schwingkreisal();
}

void Nwt4Window::schwingkreisf()
{
  QString sf;
  QString sc;
  QString sl;
  QString sxc;
  QString sxl;
  double f;
  double l;
  double c;
  double xc;
  double xl;
  bool ok;

  sf = editf1->text();
  f = sf.toDouble(&ok);
  if(!ok)return;
  sl = editl1->text();
  l = sl.toDouble(&ok);
  if(!ok)return;
  sc = editc1->text();
  c = sc.toDouble(&ok);
  if(!ok)return;

  f = f * 1000000.0;
  if(schwingkreis==induktiv){
    l = l / 1000000.0;
    c = 1.0/(4.0*M_PI*M_PI*l*f*f);
    sc = QString("%1").arg(c * 1000000000000.0);
    QObject::disconnect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisc()));
    editc1->setText(sc);
    QObject::connect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisc()));
  }
  if(schwingkreis==kapazitiv){
    c = c / 1000000000000.0;
    l = 1.0/(4.0*M_PI*M_PI*c*f*f);
    sl = QString("%1").arg(l * 1000000.0);
    QObject::disconnect( editl1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisl()));
    editl1->setText(sl);
    QObject::connect( editl1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisl()));
  }
  xc = 1.0/(2.0*M_PI*f*c);
  xl = 2.0*M_PI*f*l;
  sxc = QString("%1").arg(xc);
  ergebnisxc->setText(sxc);
  sxl = QString("%1").arg(xl);
  ergebnisxl->setText(sxl);
  schwingkreisal();
}

void Nwt4Window::schwingkreisal()
{
  QString sl;
  QString sn;
  QString sal;
  double l;
  double n;
  double al;
  bool ok;

  sl = editl1->text();
  l = sl.toDouble(&ok);
  if(!ok)return;
  sn = editn->text();
  n = sn.toDouble(&ok);
  if(!ok)return;

  l = l /1000000.0;
  al = l / (n*n);
  al = al * 1000000000.0;
  sal = QString("%1").arg(al);
  edital->setText(sal);
}

void Nwt4Window::zrausrechnen(){
  double r1;
  double r2;
  double r3;
  double r4;
  double daempf;
  double z1;
  double z2;
  double z3;
  double z4;
  double d1;
  double d2;
  QString qs;
  bool ok;

  qs = editz1->text();
  z1 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz2->text();
  z2 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz3->text();
  z3 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz4->text();
  z4 = qs.toDouble(&ok);
  if(!ok)return;
  r1 = z1/(sqrt(1.0 - (z1/z2)));
  r2 = z2*sqrt(1.0 - (z1/z2));
  r3 = z3*sqrt(1.0 - (z4/z3));
  r4 = z4/(sqrt(1.0 - (z4/z3)));

  qs.sprintf("%3.2f",r1);
  lr1->setText(qs);
  qs.sprintf("%3.2f",r2);
  lr2->setText(qs);
  qs.sprintf("%3.2f",r3);
  lr3->setText(qs);
  qs.sprintf("%3.2f",r4);
  lr4->setText(qs);

  d1 = 10.0 * log10(((r1*z1)/(r1+z1))/(2.0*r2+(r1*z1/(r1+z1))));
  //qDebug("%f",d1);
  d2 = 10.0 * log10(((r4*z4)/(r4+z4))/(2.0*r3+(r4*z4/(r4+z4))));
  //qDebug("%f",d2);

  daempf = d1+d2;
  qs.sprintf("%3.2f",daempf);
  ldaempfung->setText(qs);
}

void Nwt4Window::zlcausrechnen(){
  double xl;
  double xc;
  double l;
  double c;
  double f;
  double z1;
  double z2;
  QString qs;
  bool ok;

  qs = editzlc1->text();
  z1 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editzlc2->text();
  z2 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editzlc3->text();
  f = qs.toDouble(&ok);
  if(!ok)return;
  f = f*1000000.0;

  xl = sqrt(z1*z2-z1*z1);
  xc = (z1*z2)/xl;

  //qDebug("xl %f",xl);
  //qDebug("xc %f",xc);

  l = (xl/(2.0*3.1415*f))*1000000.0;
  c = (1.0/(2.0*3.1415*f*xc))*1000000000000.0;

  //qDebug("l %f",l);
  //qDebug("c %f",c);

  qs.sprintf("%3.2f",l);
  ll->setText(qs);
  qs.sprintf("%3.2f",c);
  lc->setText(qs);
}

void Nwt4Window::setimp(){
  if(rbr->isChecked()){
    gzr->show();
    bild1->show();
    gzlc->hide();
    bild2->hide();
  }else{
    gzr->hide();
    bild1->hide();
    gzlc->show();
    bild2->show();
  }
}

void Nwt4Window::setvw(double d){
  double volt, mwatt;
  QString qs,einh;
  double dbm=d;

  mwatt = pow(10.0 , dbm/10.0);
  volt = sqrt(mwatt * 0.05);
  qs=mwatt2str(mwatt);
  edwatt->setText(qs);
  qs=volt2str(volt);
  edvolt->setText(qs);
}

void Nwt4Window::cbdbm01_checked(bool b){
  if(b){
    spdbm->setSingleStep(0.1);
  }else{
    spdbm->setSingleStep(1.0);
  }
}

void Nwt4Window::setrz(double db){
  double ref=pow(10.0,db/20.0);
  double swv=(1.0+ref)/(1.0-ref);
  double zh=swv*50.0;
  double zl=50.0/swv;
  QString qs;
  qs.sprintf("r: %2.3f",ref);
  labelrfaktor->setText(qs);
  qs.sprintf("SWV: %2.3f",swv);
  labelswv->setText(qs);
  qs.sprintf("Z: %2.3f Ohm",zh);
  labelzhigh->setText(qs);
  qs.sprintf("Z: %2.3f Ohm",zl);
  labelzlow->setText(qs);
}

void Nwt4Window::cbrloss01_checked(bool b){
  if(b){
    sprloss->setSingleStep(0.1);
  }else{
    sprloss->setSingleStep(1.0);
  }
}

QString Nwt4Window::datetime2filename(){
  QString sdt;
  QDateTime dtime=QDateTime::currentDateTime();
  sdt=dtime.toString(grunddaten.formatfilename);
  return sdt;
}

QString Nwt4Window::fnamenormal(QString &qs){
  QString s=qs;
  s.replace('/','_');
  s.replace(',','_');
  s.replace(':','_');
  s.replace('\\','_');
  s.replace(';','_');
  return s;
}

void Nwt4Window::set0hz(){
  if(grunddaten.bset0hz){
    QString qs;
    qs.sprintf("f%09.0f",0.0);//DDS auf 0Hz prog.
    if(bnwtgefunden)picmodem->writeChar(0x8F);
    if(bnwtgefunden)picmodem->writeLine(qPrintable(qs));
    catbefehl='f';
  }
}

void Nwt4Window::DmesskurveClr(){
  messkurveClr();
  buttone1->setEnabled(false);
  buttone2->setEnabled(false);
  buttone3->setEnabled(false);
  buttone4->setEnabled(false);
  buttone5->setEnabled(false);
  buttonh1->setEnabled(false);
  buttonh2->setEnabled(false);
  buttonh3->setEnabled(false);
  buttonh4->setEnabled(false);
  buttonh5->setEnabled(false);
  display->setMessKurve(messkurve);
}

void Nwt4Window::messkurveClr(){
  for(int i=0;i<maxmesspunkte;i++){
    messkurve.dbk1[i]=-200.0;
  }
  messkurve.bkurvegueltig=false;
  spurClr();
}

void Nwt4Window::spurClr(){
  for(int i=0;i<maxmesspunkte;i++){
    for(int j=0; j<spurmax; j++){
      messkurve.dbsp[j][i]=-200.0;
    }
  }
  for(int i=0;i<spurmax;i++){
    messkurve.bdbsp[i] = false;
  }
  messkurve.dbspidx = 0; //Index = 0
}

void Nwt4Window::setKurvenSpur(bool b){
  messkurve.bspur = b;
  mKurvenAnalyse->setEnabled(b);
  display->setMessKurve(messkurve);//Im Display neu anzeigen
}

void Nwt4Window::fKurvenAnalyse(){
  QKurvenAnalyseDlg *kadlg = new QKurvenAnalyseDlg(this);
  QObject::connect(kadlg, SIGNAL(setSpurNr(int)), this, SLOT(DisplaySetSpur(int)));
  int s=0;
  for(int i=0;i<spurmax;i++){
    if(messkurve.bdbsp[i])s=i;
  }
  kadlg->setSpurMax(s);
  int r=kadlg->exec();
  if(r == QDialog::Accepted){
    int s = 0;
    s = kadlg->getSpurNr();
    DisplaySetSpur(s);
  }
  display->setMessKurve(messkurve);
  QObject::disconnect(kadlg, SIGNAL(setSpurNr(int)), this, SLOT(DisplaySetSpur(int)));
  delete kadlg;
}

void Nwt4Window::DisplaySetSpur(int s){
  int spur=s;
  if(messkurve.bdbsp[spur]){
    for(int i=0;i<messkurve.mpunkte;i++){
      messkurve.dbk1[i]=messkurve.dbsp[spur][i];
    }
    display->setMessKurve(messkurve);
  }
}


void Nwt4Window::messkurveSetSpur(){
  for(int i=0; i<messkurve.mpunkte; i++){
    messkurve.dbsp[messkurve.dbspidx][i]=messkurve.dbk1[i];//Messkurve Spur merken
  }
  messkurve.bdbsp[messkurve.dbspidx]=true;//Spur ist gueltig
  messkurve.dbspidx++;//Index Spur eins weiter
  if(messkurve.dbspidx == spurmax)messkurve.dbspidx = 0;//Alles belegt wieder von vorn
  //qDebug()<<messkurve.dbspidx;
}

void Nwt4Window::setNoFrqCalK1(bool b){
  messkopf.setNoFrqCal(b);
}

void Nwt4Window::fKabeldaten(){
  QString str;
  QString qs, s1, qs1;
  if(bd)qDebug("Nwt4Window::fKabeldaten()");

  qs1.sprintf("%1.6f",kabeldaten.lfrequenz);
  qs=tr("Laengenfrequenz: %1 MHz").arg(qs1);
  str = qs + "<br>";
  qs1.sprintf("%1.3f",kabeldaten.vf);
  qs=tr("Verkuerzungsfaktor:  %1").arg(qs1);
  str = str + qs + "<br>";
  qs1.sprintf("%1.2f",kabeldaten.laenge);
  qs=tr("Kabellaenge:  %1").arg(qs1);
  str = str + qs + "<br>";
  qs1.sprintf("%1.1f",kabeldaten.ca);
  qs=tr("Kabelkapazitaet:  %1 pF").arg(qs1);
  str = str + qs + "<br>";
  qs1.sprintf("%1.3f",kabeldaten.dk);
  qs=tr("Dielektrizitaetszahl:%1").arg(qs1);
  str = str + qs + "<br>";
  qs1.sprintf("%1.3f",kabeldaten.z);
  qs=tr("Impedanz: %1 Ohm").arg(qs1);
  str = str + qs;

  QMessageBox *msgBox;
  msgBox = new QMessageBox(this);
  QPushButton *ok = msgBox->addButton(tr("OK"), QMessageBox::ActionRole);
  QPushButton *save = msgBox->addButton(tr("Speichern"), QMessageBox::ActionRole);

  msgBox->setText(str);
  msgBox->setInformativeText(tr("<B>Kabeldaten speichern!!</B>"));
  msgBox->setDefaultButton(ok);
  msgBox->exec();
  if (msgBox->clickedButton() == ok) {
    return;
  }
  if (msgBox->clickedButton() == save) {
    QFile f;

    QSettings settings("AFU", settingspath);
    //QSettings settings("AFU", "NWT2");
    QDir path;
    path.setPath(settings.value("kurvenpath", QDir::homePath()).toString());

    QString s2 = tr("NWT4000 Kabeldaten-Datei (*.txt)","FileDialog");
    //Dateiname erzeugen
    if(messkurve.caption.isEmpty()){
      s1=datetime2filename();// Caption leer Defaultdateiname wird generiert aus Date und Time
    }else{
      s1=messkurve.caption;//Caption wird als Dateiname verwendet
    }
    s1=fnamenormal(s1);
    QString s = QFileDialog::getSaveFileName(this,tr("NWT4000 Kabeldaten-Datei speichern","FileDialog"),
                                             path.filePath(s1), s2, &s1);
    if (!s.isNull()){
      if(s.contains(".txt")) s.remove(".txt");//Dateiname Suffix entfernen
      s += ".txt";//Suffix hinzu
      f.setFileName(s);
      if(f.open(QIODevice::WriteOnly)){
        QTextStream ts(&f);
        QString sdt;
        QDateTime dtime=QDateTime::currentDateTime();
        sdt=dtime.toString();
        ts << sdt << endl;
        ts << "-------------------------"<<endl;
        ts << tr("Kabeldaten")<<endl;
        ts << "-------------------------"<<endl;
        qs.sprintf("Laengenfrequenz:     %1.6f MHz",kabeldaten.lfrequenz);
        ts << qs <<endl;
        qs.sprintf("Verkuerzungsfaktor:  %1.3f",kabeldaten.vf);
        ts << qs <<endl;
        qs.sprintf("Kabellaenge:         %1.2f m",kabeldaten.laenge);
        ts << qs <<endl;
        qs.sprintf("Kabelkapazitaet:     %1.1f pF",kabeldaten.ca);
        ts << qs <<endl;
        qs.sprintf("Dielektrizitaetszahl:%1.3f",kabeldaten.dk);
        ts << qs <<endl;
        qs.sprintf("Impedanz:            %1.3f Ohm",kabeldaten.z);
        ts << qs <<endl;
        f.close();
      }
    }
  }
  delete msgBox;
  if(bd)qDebug("Nwt4Window::fKabeldaten() ENDE");
}

double Nwt4Window::frunden(double d, double dmax, double dmin){
  double rd=d;
  if(rd<dmin){
    rd=dmin;
  }else{
    if(rd>dmax){
      rd=dmax;
    }else{
      rd=round(d);
    }
  }
  return rd;
}

void Nwt4Window::setdbmdigit(int i){
  grunddaten.precisionmw=i;
}

void Nwt4Window::getinfolist(QStringList list){
  QString qs;

  infolist.clear();
  infolist = list;
}

void Nwt4Window::lcd1clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(true);
  rb2->setChecked(false);
  rb3->setChecked(false);
  rb4->setChecked(false);
  rb5->setChecked(false);

  LCD1->setPalette(dp);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

  lcdspeicher=1;
  //double f=LCD1->value();
  //setMessVfoFrq(f*1000.0);
}

void Nwt4Window::lcd2clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(false);
  rb2->setChecked(true);
  rb3->setChecked(false);
  rb4->setChecked(false);
  rb5->setChecked(false);

  LCD1->setPalette(dpd);
  LCD2->setPalette(dp);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

  lcdspeicher=2;
}

void Nwt4Window::lcd3clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(false);
  rb2->setChecked(false);
  rb3->setChecked(true);
  rb4->setChecked(false);
  rb5->setChecked(false);

  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dp);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

  lcdspeicher=3;
}

void Nwt4Window::lcd4clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(false);
  rb2->setChecked(false);
  rb3->setChecked(false);
  rb4->setChecked(true);
  rb5->setChecked(false);

  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dp);
  LCD5->setPalette(dpd);

  lcdspeicher=4;
}

void Nwt4Window::lcd5clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(false);
  rb2->setChecked(false);
  rb3->setChecked(false);
  rb4->setChecked(false);
  rb5->setChecked(true);

  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dp);

  lcdspeicher=5;
}

void Nwt4Window::clickbvfo2sp(){
  double f;

  f=getMessVfoFrq() / 1000000.0;
  //qDebug("frq:%f",f);
  switch(lcdspeicher){
  case 1:LCD1->display(f);
    break;
  case 2:LCD2->display(f);
    break;
  case 3:LCD3->display(f);
    break;
  case 4:LCD4->display(f);
    break;
  case 5:LCD5->display(f);
    break;
  }
}

void Nwt4Window::clickbsp2vfo(){
  double f=0.0;

  switch(lcdspeicher){
  case 1:f=LCD1->value();
    break;
  case 2:f=LCD2->value();
    break;
  case 3:f=LCD3->value();
    break;
  case 4:f=LCD4->value();
    break;
  case 5:f=LCD5->value();
    break;
  }
  setMessVfoFrq(f*1000000.0);
}

void Nwt4Window::fCaldBm(){
  bool ok;
  double pegel=0.0;//0.0dBm
  double frq;
  if(messkopf.CalFrqmin() > 100.0){
    frq=150.0;
  }else{
    frq=100.0;
  }
  QString beschr1(tr("<center><b><u>Kalibrieren der Spektrumanzeige</u></b><br>"
                    "Bitte die Kalibrierfrequenz noch mit angeben. Diese<br>"
                    "wurde noch nicht mit gespeichert.</center>"
                    "-------------------------------------------------------<br>"
                    "Kalibrierfrequenz in (MHz)","Inputdialog in der Grafik"));

  if(messkopf.CalFrqPunkt() == 0.0){
    frq = QInputDialog::getDouble(this, tr(" Kalibrierfrequenz (MHz):","Inputdialog in der Grafik"),
                                     beschr1, frq, messkopf.CalFrqmin(), messkopf.CalFrqmax(), 0, &ok);
    if(ok){
      messkopf.setCalFrqPunkt(frq);
      //qDebug()<<"Kalibrierfrequenz:"<<frq;
    }else{
      return;
    }
  }
  QString beschr(tr("<center><b><u>Kalibrieren der Spektrumanzeige</u></b><br>"
                    "Dazu muss der genaue Pegel bei der <br>"
                    "<b>Kalibrierfrequenz %1 MHz</b><br>"
                    "am NWT Output gemessen werden.<br>"
                    "-------------------------------------------------------</center>"
                    "Pegel in (dBm)","Inputdialog in der Grafik").arg(messkopf.CalFrqPunkt()));

  pegel = messkopf.CalKorrdBmPegel();
  pegel = QInputDialog::getDouble(this, tr(" NWT Output Pegel (dBm):","Inputdialog in der Grafik"),
                                   beschr, pegel, -10, +10, 2, &ok);
  if(ok){
    //qDebug()<<"Pegel:"<<pegel;
    messkopf.setCalKorrdBmPegel(pegel);
    setMkInfo();//Info neu setzen
    saveMk(grunddaten.filemk);//zusaetzlich Speichern
  }
}

void Nwt4Window::fCalmWm(){
  bool ok;
  mwcalpegel1 = 0.0;
  QString beschr(tr("<b><u>Kalibrieren des mW-Meters</u></b><br>"
                 "1. Einen bekannten HF-Pegel an den Messeingang anlegen.<br>"
                 "   Das kann auch der HF-Ausgang des Netzwerktesters sein.<br>"
                 "   Der Defaultwert von %1dBm wird unten eingeblendet<br>."
                 "<br>"
                 "2. Im Ablauf der Kalibrieren wird ein genau bekanntes Daempfungsglied<br>"
                 "   eingeschleift. Das Daempfungsglied sollte min. 20dB betragen.<br>"
                 "   Aus diesen beiden Messergebnissen werden die Funktionssteilheit <br>"
                 "   errechnet die anschliessend in der Messsondendatei abgespeichert <br>"
                 "   werden muss.<br>"
                 "<br>"
                 "<br>"
                 "Pegel des HF-Generators (dBm):","InputBox").arg(mwcalpegel1));
  mwcalpegel1 = QInputDialog::getDouble(this, tr("Kalibrieren mW-Meter","InputBox"),
                                           beschr, mwcalpegel1, -20, 10, 2, &ok);
  if(ok)bmwcalp1 = true;
  ymkwert1mw = 0.0;
  ymkwert1mw = 0.0;
}

void Nwt4Window::fRS232Stop(){
  openttytimer->stop();
  mRS232Stop->setVisible(false);
  rs232stoptimer->stop();
}

void Nwt4Window::set_ztime(int i){
  int z=i;
  QString qs;

  if(z==0){
    qs = tr("Zwischenzeit");
  }else{
    qs = tr("Zwischenz. %1 mSek").arg(z*12);
  }
  label_ztime->setText(qs);
  wobbelgrunddaten.ztime = z * 111;
}

void Nwt4Window::setSpeki(bool b){
  bool bdbm=b;
  hmesskurve1.bdbm=bdbm;
  hmesskurve2.bdbm=bdbm;
  hmesskurve3.bdbm=bdbm;
  hmesskurve4.bdbm=bdbm;
  hmesskurve5.bdbm=bdbm;
  if(bdbm){
    grswvant->setEnabled(false);
    boxswvlinie1->setEnabled(false);
    boxswvlinie2->setEnabled(false);
    labelboxswvlinie1->setEnabled(false);
    labelboxswvlinie2->setEnabled(false);
    checkboxswvlinie1->setEnabled(false);
    checkboxswvlinie2->setEnabled(false);
    checkboxswvlinie1->setChecked(false);
    checkboxswvlinie2->setChecked(false);
    labelboxydbmax->setText(tr("max(dBm)"));
    labelboxydbmin->setText(tr("min(dBm)"));
    labelboxdblinie->setText(tr("dBm Linie"));
    checkboxdbmin->setText(tr("dBm Minimum"));
    checkboxdbmax->setText(tr("dBm Maximum"));
    checkbox3db->setHidden(true);
    checkbox3dbinv->setHidden(true);
    checkboxguete->setHidden(true);
    checkbox6db->setHidden(true);
    checkbox60db->setHidden(true);
    checkboxshape->setHidden(true);
    checkboxswvkanal->setHidden(true);
    grbandbreite->setTitle( tr("Display / Werte","GroupBox Bandbreiten / Werte"));
    checkboxwatt->setHidden(false);
    checkboxvolt->setHidden(false);
    mNoFrqCalK1->setEnabled(false);
    mNoFrqCalK1->setChecked(false);
  }else{
    grswvant->setEnabled(true);
    boxswvlinie1->setEnabled(true);
    boxswvlinie2->setEnabled(true);
    labelboxswvlinie1->setEnabled(true);
    labelboxswvlinie2->setEnabled(true);
    checkboxswvlinie1->setEnabled(true);
    checkboxswvlinie2->setEnabled(true);
    labelboxydbmax->setText(tr("max(dB)"));
    labelboxydbmin->setText(tr("min(dB)"));
    labelboxdblinie->setText("dB Linie");
    checkboxdbmin->setText(tr("dB Minimum"));
    checkboxdbmax->setText(tr("dB Maximum"));
    checkbox3db->setHidden(false);
    checkbox3dbinv->setHidden(false);
    checkboxguete->setHidden(false);
    checkbox6db->setHidden(false);
    checkbox60db->setHidden(false);
    checkboxshape->setHidden(false);
    checkboxswvkanal->setHidden(false);
    grbandbreite->setTitle( tr("Bandbreiten / Werte","GroupBox Bandbreiten / Werte"));
    checkboxwatt->setHidden(true);
    checkboxvolt->setHidden(true);
    mNoFrqCalK1->setEnabled(true);
    mNoFrqCalK1->setChecked(false);
  }
}

/*============================================================================================================================*/

QMesskopf::QMesskopf(){

}

void QMesskopf::init(){
  typ = mks21;// {mks21,mks21var,mks11,mks11var};
  for(int i=0; i<maxmesspunkte; i++) calpegel[i]=0.0;//DDS Frequenzgang zuruecksetzen
  calx= 0.19; //Kalibrierung Voreinstellung X Wert
  caly=-87.0; //Kalibrierung Voreinstellung Y Wert
  hwvariante=vnwt4_1; //HW default NWT4000-1
  bnofrqcal=false;
  calfrqmin=35.0;//NWT4000-2
  calfrqmax=4400.0;
  calkorrdbm=0.0;
  calfrqpunkt=0.0;
  calfrqschritt=(calfrqmax - calfrqmin) / (maxmesspunkte - 1);
  calatt=0.0;
  S11cal=vrefudef;//Kalibriervariante nicht definiert
}

void QMesskopf::setNoFrqCal(bool b){
  bnofrqcal=b;//Frequenzgang Korrektur AUS/EIN
}


//Uebertragung der Messkurve in die Pegelkalibrierung
void QMesskopf::setCalPegelvonbis(TMessKurve mk, double fanf, double fende){
  int idxa, idxb;
  bool bfrqkorr=true; // Frequenzgangkorr EIN

  idxa=round(fanf*10.0);
  idxb=round(fende*10.0);
  if((idxb - idxa) > 2000.0)bfrqkorr=false; // mehr Messpunkte als 2000 keine FrqKorr.
  for(int i=0; i<maxmesspunkte; i++) calpegel[i]=0.0;//DDS Frequenzgang zuruecksetzen
  if(!bfrqkorr){
    return; //keine FrqKorr, vorzeitig die Funktion verlassen
  }
  for(int i=idxa, j=0; i<idxb; i++,j++){
    calpegel[j]=mk.dbk1[j];
  }
}


//Uebertragung der Messkurve in die Pegelkalibrierung
void QMesskopf::setCalPegel(TMessKurve mk){
  for(int i=0;i<maxmesspunkte;i++)calpegel[i]=mk.dbk1[i];
}

void QMesskopf::clrcalpegel(){
  for(int i=0; i<maxmesspunkte; i++) calpegel[i]=0.0;
}

//Holen der Pegelabweichung per Frequenz
double QMesskopf::getCalPegel(double frq){
  int index=0;
  double dindex=0.0, diff=0.0;
  double ergebnis=0.0, ergebnis1=0.0;
  bool dbu=false;

  if(dbu)qDebug("---------------------------------------");
  if(bnofrqcal){
    ergebnis=0.0;//keine Korrektur wenn bnofrqcal
  }else{
    if(frq==0.0){//Frequenz 0.0 keine Pegelkorrektur
      ergebnis=0.0;//im mW-Meter keine Korrektur
    }else{
      dindex=(frq-calfrqmin)/calfrqschritt;
      index=int(dindex);
      if(dbu)qDebug("dindex:%f, index:%i",dindex,index);
      if(index>maxmesspunkte-1)index=maxmesspunkte-1;//sicherheitshalber index begrenzen
      if(index==0){
        ergebnis=calpegel[index];//nur der erste Pegel wird uebernommen ohne interpolieren
        if(dbu)qDebug("ergebnis:%f",ergebnis);
      }else{//interpolieren
        ergebnis=calpegel[index-1];//vorhergehender Pegel
        ergebnis1=calpegel[index];//Pegel am Index
        if(dbu)qDebug("ergebnis[index-1]:%f, ergebnis1[index]:%f",ergebnis,ergebnis1);
        diff = ergebnis1-ergebnis;//Differenz pegel[index] - pegel[index-1]
        if(dbu)qDebug("diff = ergebnis1-ergebnis:%f",diff);
        diff = diff * (dindex - double(index));//interpolieren
        if(dbu)qDebug("diff = diff * (dindex - double(index)):%f",diff);
        ergebnis = ergebnis + diff;//und aufaddieren
        if(dbu)qDebug("ergebnis = ergebnis + diff:%f",ergebnis);
      }
    }
  }
  if(dbu)qDebug("ergebnis:%f",ergebnis);
  return ergebnis;
}

//Input ADC-Wert, Output db mit Pegelabweichung per Frequenz in MHz
double QMesskopf::adc2Pegel(int adc,double frq){
  double pegel;
  double calpegel;

  calpegel=getCalPegel(frq);
  pegel=calx*adc+caly;
  pegel-=calpegel;
  return pegel;
}

//Input ADC-Wert, Output db mit Pegelabweichung per Frequenz in MHz
double QMesskopf::adc2mWMeter(int adc,double frq){
  double pegel;
  double calpegel;

  calpegel=getCalPegel(frq);
  pegel=calxmw*adc+calymw;
  pegel-=calpegel;
  return pegel;
}

void QMesskopf::setMkTyp(emktyp t){
  typ=t;
}

//Kalibrierwerte des Messkopfes
void QMesskopf::setMkXY(double x,double y){
  calx=x;caly=y;
}

//Kalibrierwerte des Messkopfes
void QMesskopf::setMkXYmWMeter(double x,double y){
  calxmw=x;calymw=y;
}

double QMesskopf::calpegelwert(int idx){
  return calpegel[idx];
}

emktyp QMesskopf::MkTyp(){
  return typ;
}

double QMesskopf::CalX(){
  return calx;
}

double QMesskopf::CalY(){
  return caly;
}

double QMesskopf::CalXmWMeter(){
  return calxmw;
}

double QMesskopf::CalYmWMeter(){
  return calymw;
}

void QMesskopf::setcalpegelwert(int idx,double d){
  calpegel[idx]=d;
  //qDebug()<<"idx:"<<idx<<"dB:"<<calpegel[idx];
}

evariante QMesskopf::HWVariante(){
  return hwvariante;
}

void QMesskopf::setHWVariante(evariante hw){
  hwvariante=hw;
}

QString QMesskopf::defDateiName(){
  QString styp, sfrqmin, sfrqmax;

  switch(typ){
  case mks21:
  case mks21var:styp="S21_";break;
  case mks11:
  case mks11var:styp="S11_";break;
  default:styp="log";break;
  }
  sfrqmin.sprintf("%2.0fMHz_",CalFrqmin());
  sfrqmax.sprintf("%2.0fMHz",CalFrqmax());
  return styp + sfrqmin + sfrqmax + ".n4m";
}

void QMesskopf::setCalFrqmin(double d){
  calfrqmin = d;
}

void QMesskopf::setCalFrqmax(double d){
  calfrqmax = d;
}

double QMesskopf::CalFrqmin(){
  return calfrqmin;
}

double QMesskopf::CalFrqmax(){
  return calfrqmax;
}

void QMesskopf::setCalFrqPunkt(double d){
  calfrqpunkt = d;
}

double QMesskopf::CalFrqPunkt(){
  return calfrqpunkt;
}

void QMesskopf::setCalKorrdBmPegel(double d){
  calkorrdbm = d;
}

double QMesskopf::CalKorrdBmPegel(){
  return calkorrdbm;
}

void QMesskopf::setCalFrqSchritt(double d){
  calfrqschritt = d;
}

double QMesskopf::CalFrqSchritt(){
  return calfrqschritt;
}

void QMesskopf::setCalAtt(double d){
  calatt = d;
}

double QMesskopf::CalAtt(){
  return calatt;
}

void QMesskopf::setS11cal(eS11cal sc){
  S11cal = sc;
}

eS11cal QMesskopf::getS11cal(){
  return S11cal;
}
