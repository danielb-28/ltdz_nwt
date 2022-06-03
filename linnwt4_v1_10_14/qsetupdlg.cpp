//#include <QWidget>
//#include <QDialog>

#include "qsetupdlg.h"


class QDialog;

//#####################################################################################
// Class fuer Setup
//#####################################################################################
setupdlg::setupdlg(QWidget *parent) :  QDialog(parent)
{
  int h=560;
  int w=520;

  tabwg = new QTabWidget(this);
  tabwg->setGeometry(0,0,w-120,h);
  wgblatt1 = new QWidget();
  tabwg->addTab(wgblatt1, tr("HW/Allgemein","TabWindow"));

  this->resize(w,h);
  this->setMinimumSize(w,h);
  this->setMaximumSize(w,h);
  this->setWindowTitle("SETUP Software/Hardware");
  ok = new QPushButton(tr("OK","Setup Dialog"), this);
  ok->setGeometry(w-110,20,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Abbruch","Setup Dialog"), this);
  cancel->setGeometry(w-110,60,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));

  grddstakt = new QGroupBox(wgblatt1);
  grddstakt->setTitle(tr("NWT Taktfrequenz (Hz)","GroupBox"));
  grddstakt->setGeometry(10,10,210,120);
  sboxddstakt = new QDoubleSpinBox(grddstakt);
  QObject::connect( sboxddstakt, SIGNAL( valueChanged ( double)), this, SLOT(setTakt(double)));
  sboxddstakt->setGeometry(10,30,100,30);
  sboxddstakt->setSingleStep(1.0);
  sboxddstakt->setMaximum(2000000000);
  sboxddstakt->setMinimum(500000000);
  sboxddstakt->setDecimals(0);
  cbtaktx10 = new QCheckBox(grddstakt);
  QObject::connect( cbtaktx10, SIGNAL( toggled (bool)), this, SLOT(cbtaktx10_checked(bool)));
  cbtaktx10->setGeometry(120,30,50,30);
  cbtaktx10->setText("x50");
  cbtaktx10->setChecked(false);
  sboxvfo = new QDoubleSpinBox(grddstakt);
  QObject::connect( sboxvfo, SIGNAL( valueChanged ( double)), this, SLOT(setVFO(double)));
  sboxvfo->setGeometry(10,70,100,30);
  sboxvfo->setSingleStep(1.0);
  sboxvfo->setMaximum(1500000000);
  sboxvfo->setValue(1000000000);
  sboxvfo->setDecimals(0);
  labelddsvfo = new QLabel("(Hz)  VFO-Frq",grddstakt);
  labelddsvfo->setGeometry(120,70,120,30);

  grnwtauswahl = new QGroupBox(wgblatt1);
  grnwtauswahl->setTitle(tr("NWT Auswahl","GroupBox"));
  grnwtauswahl->setGeometry(230,10,150,180);
  rbnwt4000_1 = new QRadioButton("NWT4000-1",grnwtauswahl);
  rbnwt4000_1->setGeometry(10,20,120,30);
  QObject::connect(rbnwt4000_1, SIGNAL(clicked(bool)), this, SLOT(setNWT4000_1_CalFrq()));
  rbnwt4000_2 = new QRadioButton("NWT4000-2",grnwtauswahl);
  rbnwt4000_2->setGeometry(10,50,120,30);
  QObject::connect(rbnwt4000_2, SIGNAL(clicked(bool)), this, SLOT(setNWT4000_2_CalFrq()));
  rbnwt_ltdz = new QRadioButton("LTDZ (old)",grnwtauswahl);
  rbnwt_ltdz->setGeometry(10,80,120,30);
  QObject::connect(rbnwt_ltdz, SIGNAL(clicked(bool)), this, SLOT(setNWT_ltdz_CalFrq()));
  rbnwt6000 = new QRadioButton("NWT6000",grnwtauswahl);
  rbnwt6000->setGeometry(10,110,120,30);
  QObject::connect(rbnwt6000, SIGNAL(clicked(bool)), this, SLOT(setNWT6000_CalFrq()));
  rbnwt_nn = new QRadioButton("NWT no name",grnwtauswahl);
  rbnwt_nn->setGeometry(10,140,120,30);
  QObject::connect(rbnwt_nn, SIGNAL(clicked(bool)), this, SLOT(setNWT_nn_CalFrq()));

  grwobbel0hz = new QGroupBox(tr("Wobbeln inaktiv","GroupBox"), wgblatt1);
  grwobbel0hz->setGeometry(230, 200, 150, 55);
  cbset0hz = new QCheckBox(grwobbel0hz);
  QObject::connect( cbset0hz, SIGNAL( toggled (bool)), this, SLOT(cbset0hz_checked(bool)));
  cbset0hz->setGeometry(10,20,140,30);
  cbset0hz->setText("SET 0 Hz");
  cbset0hz->setChecked(false);

  grhwcall = new QGroupBox(tr("HW Flatnes Kalibrierung","GroupBox"), wgblatt1);
  grhwcall->setGeometry(230, 265, 150, 55);
  cberledigt = new QCheckBox(grhwcall);
  QObject::connect( cberledigt, SIGNAL( toggled (bool)), this, SLOT(cberledigt_checked(bool)));
  cberledigt->setGeometry(10,20,140,30);
  cberledigt->setText(tr("Durchgefuehrt"));
  cberledigt->setChecked(false);

  grnozwtime = new QGroupBox(tr("Wobbeln Zwischenzeit","GroupBox"), wgblatt1);
  grnozwtime->setGeometry(230, 330, 150, 55);
  cbnozwtime = new QCheckBox(grnozwtime);
  QObject::connect( cbnozwtime, SIGNAL( toggled (bool)), this, SLOT(cbnozwtime_checked(bool)));
  cbnozwtime->setGeometry(10,20,140,30);
  cbnozwtime->setText(tr("Sperren"));
  cbnozwtime->setChecked(false);

  gr0dBLine = new QGroupBox(tr("0dB/dBm Linie","GroupBox"), wgblatt1);
  gr0dBLine->setGeometry(230, 395, 150, 55);
  cb0dBLine = new QCheckBox(gr0dBLine);
  QObject::connect( cb0dBLine, SIGNAL( toggled (bool)), this, SLOT(cb0dBLine_checked(bool)));
  cb0dBLine->setGeometry(10,20,140,30);
  cb0dBLine->setText(tr("Markieren"));
  cb0dBLine->setChecked(false);

  grcalfrq = new QGroupBox(wgblatt1);
  grcalfrq->setTitle(tr("NWT Kalibrierfrequenz-Grenzen","GroupBox"));
  grcalfrq->setGeometry(10,140,210,120);
  lecalfrqmin = new QLineEdit(grcalfrq);
  lecalfrqmin->setGeometry(10,20,80,30);
  lacalfrqmin = new QLabel(tr("untere Grenze"),grcalfrq);
  lacalfrqmin->setGeometry(100,20,120,30);
  lecalfrqmax = new QLineEdit(grcalfrq);
  lecalfrqmax->setGeometry(10,70,80,30);
  lacalfrqmax = new QLabel(tr("obere Grenze"),grcalfrq);
  lacalfrqmax->setGeometry(100,70,120,30);
  //--------------------------------------------------------------------------------------
  wgblatt2 = new QWidget();
  tabwg->addTab(wgblatt2, tr("Color","TabWindow"));
  grcolor = new QGroupBox(wgblatt2);
  grcolor->setTitle(tr("Display - Color - Einstellungen","GroupBox"));
  grcolor->setGeometry(10,5,350,350);
  buttonhintergrund = new QPushButton(tr("","Setup Dialog"), grcolor);
  buttonhintergrund->setGeometry(10,20,100,30);
  connect(buttonhintergrund, SIGNAL(clicked()), SLOT(setbuttonhintergrund()));
  labelhintergrund = new QLabel(grcolor);
  labelhintergrund->setText(tr("Display Hintergrund","Label"));
  labelhintergrund->setGeometry(120,20,200,30);
  buttonschrift = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttonschrift->setGeometry(10,60,100,30);
  connect(buttonschrift, SIGNAL(clicked()), SLOT(setbuttonschrift()));
  labelschrift = new QLabel(grcolor);
  labelschrift->setText(tr("X/Y Werte/Gitter/Beschriftung","Label"));
  labelschrift->setGeometry(120,60,200,30);
  buttonmarkerschrift = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttonmarkerschrift->setGeometry(10,100,100,30);
  connect(buttonmarkerschrift, SIGNAL(clicked()), SLOT(setbuttonmarkerschrift()));
  labelmarkerschrift = new QLabel(grcolor);
  labelmarkerschrift->setText(tr("Schrift Marker im Display","Label"));
  labelmarkerschrift->setGeometry(120,100,200,30);
  buttonliniek1 = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttonliniek1->setGeometry(10,140,100,30);
  connect(buttonliniek1, SIGNAL(clicked()), SLOT(setbuttonliniek1()));
  labelliniek1 = new QLabel(grcolor);
  labelliniek1->setText(tr("Linie Kurve","Label"));
  labelliniek1->setGeometry(120,140,200,30);
  buttonlinekmarker = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttonlinekmarker->setGeometry(10,180,100,30);
  connect(buttonlinekmarker, SIGNAL(clicked()), SLOT(setbuttonlinekmarker()));
  labelliniek1 = new QLabel(grcolor);
  labelliniek1->setText(tr("Linie Kurve Marker","Label"));
  labelliniek1->setGeometry(120,180,200,30);
  buttonfmarken = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttonfmarken->setGeometry(10,220,100,30);
  connect(buttonfmarken, SIGNAL(clicked()), SLOT(setbuttonfmarken()));
  labelfmarken = new QLabel(grcolor);
  labelfmarken->setText(tr("Frequenzmarken/Kursorkreuz","Label"));
  labelfmarken->setGeometry(120,220,200,30);

  buttoncolorspur = new QPushButton(tr("Color","Setup Dialog Button"), grcolor);
  buttoncolorspur->setGeometry(10,260,100,30);
  connect(buttoncolorspur, SIGNAL(clicked()), SLOT(setbuttoncolorspur()));
  labelcolorspur = new QLabel(grcolor);
  labelcolorspur->setText(tr("Wobbel Spuren","Label"));
  labelcolorspur->setGeometry(120,260,200,30);

  buttonsetdefault = new QPushButton(tr("RESET","Setup Dialog Button"), grcolor);
  buttonsetdefault->setGeometry(10,300,100,30);
  connect(buttonsetdefault, SIGNAL(clicked()), SLOT(setbuttonsetdefault()));
  labelsetdefault = new QLabel(grcolor);
  labelsetdefault->setText(tr("RESET auf Default-Werte","Label"));
  labelsetdefault->setGeometry(120,300,200,30);

  grfnformat = new QGroupBox(tr("Einstellung des \"default\" Dateiname","GroupBox"),wgblatt2);
  grfnformat->setGeometry(10,360,300,90);
  labelfnformat = new QLabel(tr("Format-Beschreibung","Label"),grfnformat);
  labelfnformat->setGeometry(10, 20, 150, 30);
  bfnformat = new QPushButton(tr("Wiederherst.","Button"),grfnformat);
  bfnformat->setGeometry(190,20,100,25);
  QObject::connect( bfnformat, SIGNAL(clicked()), this, SLOT(setfndef()));
  leditfnformat = new QLineEdit(grfnformat);
  leditfnformat->setGeometry(10, 50, 280, 30);

  //--------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------
  wgblatt4 = new QWidget();
  tabwg->addTab(wgblatt4, tr("Fonts","TabWindow"));

  grfontsize = new QGroupBox(tr("Schriftgroessen","GroupBox"),wgblatt4);
  grfontsize->setGeometry(10,10,370,300);
  cbfontext = new QCheckBox(grfontsize);
  QObject::connect( cbfontext, SIGNAL( toggled (bool)), this, SLOT(cbfontextern_checked(bool)));
  cbfontext->setGeometry(10,25,270,30);
  cbfontext->setText(tr("Schriftgroessen (Default) vom System","Checkbox"));
  cbfontext->setChecked(false);

  sboxfsize = new QSpinBox(grfontsize);
  sboxfsize->setGeometry(10,60,50,30);
  sboxfsize->setRange(8,20);
  sboxfsize->setValue(12);
  QObject::connect( sboxfsize, SIGNAL( valueChanged (int)), this, SLOT(setFontSize(int)));
  labelfsize = new QLabel(tr("Programmschrift allgemein","Label"),grfontsize);
  labelfsize->setGeometry(70,60,290,30);
  labelfsize->setAlignment(Qt::AlignCenter);

  sboxmfsize = new QSpinBox(grfontsize);
  sboxmfsize->setGeometry(10,95,50,30);
  sboxmfsize->setRange(8,20);
  sboxmfsize->setValue(12);
  QObject::connect( sboxmfsize, SIGNAL( valueChanged (int)), this, SLOT(setmFontSize(int)));
  labelmfsize = new QLabel(tr("Markerschrift der Wobbelmarker","Label"),grfontsize);
  labelmfsize->setGeometry(70,95,290,30);
  labelmfsize->setAlignment(Qt::AlignCenter);

  sboxmwfsize = new QSpinBox(grfontsize);
  sboxmwfsize->setGeometry(10,130,50,30);
  sboxmwfsize->setRange(8,20);
  sboxmwfsize->setValue(14);
  QObject::connect( sboxmwfsize, SIGNAL( valueChanged (int)), this, SLOT(setmwFontSize(int)));
  labelmwfsize = new QLabel(tr("Werte/Ueberschriften im mW-Meter","Label"),grfontsize);
  labelmwfsize->setGeometry(70,130,290,30);
  labelmwfsize->setAlignment(Qt::AlignCenter);

  sboxvfofsize = new QSpinBox(grfontsize);
  sboxvfofsize->setGeometry(10,165,50,30);
  sboxvfofsize->setRange(8,20);
  sboxvfofsize->setValue(14);
  QObject::connect( sboxvfofsize, SIGNAL( valueChanged (int)), this, SLOT(setvfoFontSize(int)));
  labelvfofsize = new QLabel(tr("Werte/Ueberschriften im VCO","Label"),grfontsize);
  labelvfofsize->setGeometry(70,165,290,30);
  labelvfofsize->setAlignment(Qt::AlignCenter);

  sboxberfsize = new QSpinBox(grfontsize);
  sboxberfsize->setGeometry(10,200,50,30);
  sboxberfsize->setRange(8,20);
  sboxberfsize->setValue(13);
  QObject::connect( sboxberfsize, SIGNAL( valueChanged (int)), this, SLOT(setberFontSize(int)));
  labelberfsize = new QLabel(tr("Werte in Berechnungen (rechts)","Label"),grfontsize);
  labelberfsize->setGeometry(70,200,290,30);
  labelberfsize->setAlignment(Qt::AlignCenter);

  sboxgrberfsize = new QSpinBox(grfontsize);
  sboxgrberfsize->setGeometry(10,235,50,30);
  sboxgrberfsize->setRange(8,20);
  sboxgrberfsize->setValue(13);
  QObject::connect( sboxgrberfsize, SIGNAL( valueChanged (int)), this, SLOT(setgrberFontSize(int)));
  labelgrberfsize = new QLabel(tr("Ueberschrift in Berechnungen (rechts)","Label"),grfontsize);
  labelgrberfsize->setGeometry(70,235,290,30);
  labelgrberfsize->setAlignment(Qt::AlignCenter);
  //--------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------
  hwvariante=vnwt4_1;//default keine Zuordnung
  cbvfosperre=false;
};

setupdlg::~setupdlg()
{
}

void setupdlg::setgrunddaten(const TGrunddaten &gdaten){

  grunddaten = gdaten;
  sboxddstakt->setValue(grunddaten.nwttakt);
  sboxvfo->setValue(grunddaten.nwttaktcalfrq);
  switch (grunddaten.variante) {
  case vnwt4_1:
    rbnwt4000_1->setChecked(true);
    break;
  case vnwt4_2:
    rbnwt4000_2->setChecked(true);
    break;
  case vnwt_ltdz:
    rbnwt_ltdz->setChecked(true);
    break;
  case vnwt6:
    rbnwt6000->setChecked(true);
    break;
  case vnwt_nn:
    rbnwt_nn->setChecked(true);
    break;
  default:
    rbnwt_nn->setChecked(true);
    break;
  }
  cberledigt->setChecked(grunddaten.bflatcal);
  if(grunddaten.variante==vnwt_ltdz or grunddaten.variante==vnwt_nn){
    grunddaten.bflatcal=true;
    cberledigt->setChecked(grunddaten.bflatcal);
    cberledigt->setDisabled(true);
  }
  cbnozwtime->setChecked(grunddaten.bnozwtime);
  //Schriftgroessen Werte kopieren
  sboxfsize->setValue(grunddaten.fontsize);
  sboxmfsize->setValue(grunddaten.markerfontsize);
  sboxmwfsize->setValue(grunddaten.mwfontsize);
  sboxvfofsize->setValue(grunddaten.vfofontsize);
  sboxberfsize->setValue(grunddaten.berfontsize);
  sboxgrberfsize->setValue(grunddaten.grberfontsize);

  //Schriftgroessen im Dialog anzeigen
  setFontSize(grunddaten.fontsize);
  setmFontSize(grunddaten.markerfontsize);
  setmwFontSize(grunddaten.mwfontsize);
  setvfoFontSize(grunddaten.vfofontsize);
  setberFontSize(grunddaten.berfontsize);
  setgrberFontSize(grunddaten.grberfontsize);

  cbfontext->setChecked(!grunddaten.bfontsize);
  sboxfsize->setEnabled(grunddaten.bfontsize);
  labelfsize->setEnabled(grunddaten.bfontsize);
  sboxmfsize->setEnabled(grunddaten.bfontsize);
  labelmfsize->setEnabled(grunddaten.bfontsize);
  sboxmwfsize->setEnabled(grunddaten.bfontsize);
  labelmwfsize->setEnabled(grunddaten.bfontsize);
  sboxvfofsize->setEnabled(grunddaten.bfontsize);
  labelvfofsize->setEnabled(grunddaten.bfontsize);
  sboxberfsize->setEnabled(grunddaten.bfontsize);
  labelberfsize->setEnabled(grunddaten.bfontsize);
  sboxgrberfsize->setEnabled(grunddaten.bfontsize);
  labelgrberfsize->setEnabled(grunddaten.bfontsize);
  if(grunddaten.version==0){
    grddstakt->setEnabled(false);
  }else{
    grddstakt->setEnabled(true);
  }
  leditfnformat->setText(grunddaten.formatfilename);
  cbset0hz->setChecked(grunddaten.bset0hz);
  setColorButton();
  setVFO(1000000000.0);//VCO auf 1000,000000 MHz voreinstellen
};

void setupdlg::setwobdaten(const TWobbelGrundDaten &wdaten){
  QString qs;

  wobbelgrunddaten = wdaten;
  qs = frq2str(wobbelgrunddaten.calfrqmin);
  lecalfrqmin->setText(qs);
  qs = frq2str(wobbelgrunddaten.calfrqmax);
  lecalfrqmax->setText(qs);
  cb0dBLine->setChecked(wobbelgrunddaten.b0dBline);
}

TGrunddaten setupdlg::getgrunddaten(){

  grunddaten.nwttakt=sboxddstakt->value();
  grunddaten.nwttaktcalfrq=sboxvfo->value();
  grunddaten.bset0hz=cbset0hz->isChecked();
  if(rbnwt4000_1->isChecked() == true)
    grunddaten.variante=vnwt4_1;
  if(rbnwt4000_2->isChecked() == true)
    grunddaten.variante=vnwt4_2;
  if(rbnwt_ltdz->isChecked() == true)
    grunddaten.variante=vnwt_ltdz;
  if(rbnwt6000->isChecked() == true)
    grunddaten.variante=vnwt6;
  if(rbnwt_nn->isChecked() == true)
    grunddaten.variante=vnwt_nn;
  grunddaten.bflatcal=cberledigt->isChecked();
  grunddaten.bnozwtime=cbnozwtime->isChecked();

  grunddaten.fontsize=sboxfsize->value();
  grunddaten.markerfontsize=sboxmfsize->value();
  grunddaten.mwfontsize=sboxmwfsize->value();
  grunddaten.vfofontsize=sboxvfofsize->value();
  grunddaten.berfontsize=sboxberfsize->value();
  grunddaten.grberfontsize=sboxgrberfsize->value();
  grunddaten.formatfilename=leditfnformat->text();
  grunddaten.bfontsize = !cbfontext->isChecked();
  return grunddaten;
};

TWobbelGrundDaten setupdlg::getwobdaten(){
  QString qs;

  qs = lecalfrqmin->text();
  wobbelgrunddaten.calfrqmin = str2frq(qs);
  qs = lecalfrqmax->text();
  wobbelgrunddaten.calfrqmax = str2frq(qs);
  return wobbelgrunddaten;
}

void setupdlg::setHWVariante(evariante hw){
  hwvariante=hw;
}

void setupdlg::tip(bool atip){
  QString tip_svfo = tr(
        "<b>Einstellen der VFO-Frequenz f&uuml;r</b><br>"
        "<b>das Kalibrieren.</b><br>"
        "---------------------------<br>"
        "Voreinstellung sind 1000 MHz. Besitzt man<br>"
        "einen genauen Frequenzzaehler, kann diese<br>"
        "Frequenz die hier eingestellt wurde, gemessen<br>"
        "werden. Jede &Auml;nderung wird sofort zum NWT<br>"
        "&uuml;bertragen und die Frequenz &auml;ndert sich."
        ,"tooltip text");
  QString tip_fontsize = tr(
        "<b>Schriftgr&ouml;&szlig;en</b> <br>"
        "-------------------------------------<br>"
        "Einstellung der Schriftgr&ouml;&szlig;e im<br>"
        "im Display f&uuml;r die Marker und Bandbreiten<br>"
        "und im Programm. Passt das nicht kann man die<br>"
        "Schriftgr&ouml;ssen von BS verwenden. Dazu muss<br>"
        "aber anschliessend das Programm neu gestartet werden."
        ,"tooltip text");
  QString tip_fnformat = tr(
        "<b>d</b> the day as number without a leading zero (1 to 31)<br>"
        "<b>dd</b> the day as number with a leading zero (01 to 31)<br>"
        "<b>ddd</b>	the abbreviated localized day name (e.g. 'Mon' to 'Sun')<br>"
        "<b>dddd</b>	the long localized day name (e.g. 'Monday' to 'Qt::Sunday')<br>"
        "<b>M</b>	the month as number without a leading zero (1-12)<br>"
        "<b>MM</b>	the month as number with a leading zero (01-12)<br>"
        "<b>MMM</b>	the abbreviated localized month name (e.g. 'Jan' to 'Dec')<br>"
        "<b>MMMM</b>	the long localized month name (e.g. 'January' to 'December')<br>"
        "<b>yy</b>	the year as two digit number (00-99)<br>"
        "<b>yyyy</b>	the year as four digit number<br>"
        "--------------------------------------<br>"
        "<b>h</b>	the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)<br>"
        "<b>hh</b>	the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)<br>"
        "<b>m</b>	the minute without a leading zero (0 to 59)<br>"
        "<b>mm</b>	the minute with a leading zero (00 to 59)<br>"
        "<b>s</b>	the second without a leading zero (0 to 59)<br>"
        "<b>ss</b>	the second with a leading zero (00 to 59)<br>"
        "<b>z</b>	the milliseconds without leading zeroes (0 to 999)<br>"
        "<b>zzz</b>	the milliseconds with leading zeroes (000 to 999)<br>"
        "<b>AP</b>	use AM/PM display. AP will be replaced by either AM or PM.<br>"
        "<b>ap</b>	use am/pm display. ap will be replaced by either am or pm.<br>"
        ,"tooltip_text");
  QString tip_wobbel0hz = tr(
        "<b>Wobbeln inaktiv</b> <br>"
        "--------------------------------------------<br>"
        "Nach dem Wobbeln wird dauerhaft die Frequenz<br>"
        "des letzten Messpunktes ausgegeben <br>"
        "Soll das verhindert werden ist diese <br>"
        "Checkbox zu aktivieren. Die Frequenz wird <br>"
        "auf 0 Hz eingestellt und gibt keine HF <br>"
        "mehr aus."
        ,"tooltip text");
  QString tip_frqedit = tr(
        "<b>Eingabe:</b><br>"
        "2g1 oder 2,1g oder 2.1g = 2100 MHz<br>"
        "<b>Eingabe:</b><br>"
        "3m5 oder 3,5m oder 3.5m = 3,500000 MHz<br>"
        "<b>Eingabe:</b><br>"
        "350k5 oder 350,5k oder 350.5k = 350,500 kHz"
        ,"tooltip text");

  bool btip=atip;

  if(btip){
    sboxvfo->setToolTip(tip_svfo);
    grfontsize->setToolTip(tip_fontsize);
    grfnformat->setToolTip(tip_fnformat);
    leditfnformat->setToolTip(tip_fnformat);
    labelfnformat->setToolTip(tip_fnformat);
    bfnformat->setToolTip(tip_fnformat);
    grwobbel0hz->setToolTip(tip_wobbel0hz);
  }else{
    sboxvfo->setToolTip("");
    grfontsize->setToolTip("");
    grfnformat->setToolTip("");
    leditfnformat->setToolTip("");
    labelfnformat->setToolTip("");
    bfnformat->setToolTip("");
    grwobbel0hz->setToolTip("");
  }

}

void setupdlg::setTakt(double d){
  emit setNWTTakt(d);
}

void setupdlg::setVFO(double d){
  emit setVfo(d);
}

void setupdlg::cbtaktx10_checked(bool b){
  if(b){
    sboxddstakt->setSingleStep(50.0);
  }else{
    sboxddstakt->setSingleStep(1.0);
  }
}

void setupdlg::setbuttonhintergrund(){
  bool ok;
  QColor color = grunddaten.colorhintergrund;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorhintergrund=color;
    setColorButton();
  }
}

void setupdlg::setbuttoncolorspur(){
  bool ok;
  QColor color = grunddaten.colorwobbelspur;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorwobbelspur=color;
    setColorButton();
  }
}

void setupdlg::setbuttonschrift(){
  bool ok;
  QColor color = grunddaten.colorschrift;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorschrift=color;
    setColorButton();
  }
}

void setupdlg::setbuttonmarkerschrift(){
  bool ok;
  QColor color = grunddaten.colormarkerschrift;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colormarkerschrift=color;
    setColorButton();
  }
}

void setupdlg::setbuttonliniek1(){
  bool ok;
  QColor color = grunddaten.colorlinie;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorlinie=color;
    setColorButton();
  }
}

void setupdlg::setbuttonlinekmarker(){
  bool ok;
  QColor color = grunddaten.colorliniemarker;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorliniemarker=color;
    setColorButton();
  }
}

void setupdlg::setbuttonfmarken(){
  bool ok;
  QColor color = grunddaten.colorfmarken;
  QRgb rgb;
  rgb = color.rgb();
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    grunddaten.colorfmarken=color;
    setColorButton();
  }
}

void setupdlg::setColorButton(){
  QPalette palette;
  palette = buttonhintergrund->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  buttonhintergrund->setPalette(palette);
  palette = buttonschrift->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colorschrift);
  buttonschrift->setPalette(palette);
  palette = buttonmarkerschrift->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colormarkerschrift);
  buttonmarkerschrift->setPalette(palette);
  palette = buttonliniek1->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colorlinie);
  buttonliniek1->setPalette(palette);
  palette = buttonlinekmarker->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colorliniemarker);
  buttonlinekmarker->setPalette(palette);
  palette = buttonfmarken->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colorfmarken);
  buttonfmarken->setPalette(palette);
  palette = buttoncolorspur->palette();
  palette.setColor(QPalette::Active, QPalette::Button, grunddaten.colorhintergrund);
  palette.setColor(QPalette::ButtonText, grunddaten.colorwobbelspur);
  buttoncolorspur->setPalette(palette);
}

void setupdlg::setbuttonsetdefault(){
  int r,g,b,a;

  r = 255; g = 255; b = 152; a = 255;
  grunddaten.colorhintergrund = QColor(r,g,b,a);
  r = 118; g = 117; b = 136; a = 255;
  grunddaten.colorschrift = QColor(r,g,b,a);
  r = 0; g = 0; b = 0; a = 255;
  grunddaten.colormarkerschrift = QColor(r,g,b,a);
  r = 255; g = 0; b = 0; a = 255;
  grunddaten.colorlinie = QColor(r,g,b,a);
  r = 255; g = 0; b = 0; a = 255;
  grunddaten.colorliniemarker = QColor(r,g,b,a);
  r = 0; g = 255; b = 255; a = 255;
  grunddaten.colorfmarken = QColor(r,g,b,a);
  setColorButton();
}

void setupdlg::setmFontSize(int i){
  QFont font=labelmfsize->font();
  font.setBold(false);
  font.setPixelSize(i);
  labelmfsize->setFont(font);
}

void setupdlg::setFontSize(int i){
  QFont font=labelfsize->font();
  font.setBold(false);
  font.setPixelSize(i);
  labelfsize->setFont(font);
}

void setupdlg::setmwFontSize(int i){
  QFont font=labelmwfsize->font();
  font.setBold(false);
  font.setPixelSize(i);
  labelmwfsize->setFont(font);
}

void setupdlg::setvfoFontSize(int i){
  QFont font=labelvfofsize->font();
  font.setBold(false);
  font.setPixelSize(i);
  labelvfofsize->setFont(font);
}

void setupdlg::setberFontSize(int i){
  QFont font=labelberfsize->font();
  font.setBold(false);
  font.setPixelSize(i);
  labelberfsize->setFont(font);
}

void setupdlg::setgrberFontSize(int i){
  QFont font=labelgrberfsize->font();
  font.setBold(true);
  font.setPixelSize(i);
  labelgrberfsize->setFont(font);
}

void setupdlg::cbfontextern_checked(bool b){
  grunddaten.bfontsize=!b;
  sboxfsize->setEnabled(!b);
  labelfsize->setEnabled(!b);
  sboxmfsize->setEnabled(!b);
  labelmfsize->setEnabled(!b);
  sboxmwfsize->setEnabled(!b);
  labelmwfsize->setEnabled(!b);
  sboxvfofsize->setEnabled(!b);
  labelvfofsize->setEnabled(!b);
  sboxberfsize->setEnabled(!b);
  labelberfsize->setEnabled(!b);
  sboxgrberfsize->setEnabled(!b);
  labelgrberfsize->setEnabled(!b);
}

void setupdlg::setfndef(){
  leditfnformat->setText("yyMMdd_hhmmss");
}

void setupdlg::cbset0hz_checked(bool b){
  grunddaten.bset0hz=b;
}

double setupdlg::linenormalisieren(const QString &line)
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
  qDebug()<<ergebnis;
  //qDebug("linenormalisieren ENDE");
  return ergebnis;
}

void  setupdlg::setNWT4000_1_CalFrq(){
  dca=138.0, dce=4400;

  set_lea_lee();
  cberledigt->setDisabled(false);
  if(grunddaten.variante!=vnwt4_1)cberledigt->setChecked(false);
}

void  setupdlg::setNWT4000_2_CalFrq(){
  dca=35.0, dce=4400;

  set_lea_lee();
  cberledigt->setDisabled(false);
  if(grunddaten.variante!=vnwt4_2)cberledigt->setChecked(false);
}

void setupdlg::setNWT_ltdz_CalFrq(){
  dca=35.0, dce=4400;

  set_lea_lee();
  grunddaten.bflatcal=true;
  cberledigt->setChecked(grunddaten.bflatcal);
  cberledigt->setDisabled(true);
}

void  setupdlg::setNWT6000_CalFrq(){
  dca=21.0, dce=6200;

  set_lea_lee();
  cberledigt->setDisabled(false);
  if(grunddaten.variante!=vnwt6)cberledigt->setChecked(false);

}

void setupdlg::set_lea_lee(){
  QString qs;

  qs=frq2str(dca);
  lecalfrqmin->setText(qs);
  qs=frq2str(dce);
  lecalfrqmax->setText(qs);
}

void  setupdlg::setNWT_nn_CalFrq(){
  dca=138.0, dce=4400;
  QString qs;

  qs=frq2str(dca);
  lecalfrqmin->setText(qs);
  qs=frq2str(dce);
  lecalfrqmax->setText(qs);
  grunddaten.bflatcal=true;
  cberledigt->setChecked(grunddaten.bflatcal);
  cberledigt->setDisabled(true);
}

double setupdlg::str2frq(const QString &s){
  QString qs=s;
  bool ok;
  double d,r;

  //qDebug("str2frq");
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
    return d/1000000.0;
  }else{
    return (0.0);
  }
}

QString setupdlg::frq2str(double d){
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

void setupdlg::cberledigt_checked(bool b){
  cberledigt->setChecked(b);
}

void setupdlg::cbnozwtime_checked(bool b){
  cbnozwtime->setChecked(b);
}

void setupdlg::cb0dBLine_checked(bool b){
  wobbelgrunddaten.b0dBline=b;
}
/*============================================================================================================================*/

