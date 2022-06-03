#include <QApplication>
#include "nwt4window.h"

int main(int argc, char *argv[])
{
  QDir dir(argv[0]);
  QString programpath(dir.absolutePath());
  bool bl=false;//Languarge
  bool bp=false;//Path
  QString qsl, qsp;

  QString qs;
  //Alle Argumente durchsuchen
  for(int i=0;i<argc;i++){
    qs=argv[i];
    if(!bp and qs.contains("-p") and qs.length() > 2){
      qsp=qs;
      bp=true;
    }
    if(!bl and qs.contains("-l") and qs.length() > 2){
      qsl=qs;
      bl=true;
    }
  }

  QApplication a(argc, argv);
  QTranslator nwt4translator;
  QString fname;

  if(bl){
    qsl.remove(0,2);
    fname = qsl;
    if(!fname.contains("nwt4_")){
      fname = QString("nwt4_%1").arg(qsl);
    }

    qDebug()<<"Translator-File:"<<fname;
#ifdef Q_OS_WIN
    if(nwt4translator.load(fname,"transl/"))qDebug("Translator OK");
#else
    if(nwt4translator.load(fname,"/usr/share/nwt4transl/"))qDebug("Translator OK");
#endif
    a.installTranslator(&nwt4translator);
  }

  if(bp){
    qsp.remove(0,2);
  }

  Nwt4Window w;



#ifdef Q_OS_WIN
  a.setWindowIcon(QIcon(":images/appicon.png"));
  w.setWindowIcon(QIcon(":images/appicon.png"));
#else
  a.setWindowIcon(QIcon("/usr/share/pixmaps/NWT4x.png"));
  w.setWindowIcon(QIcon("/usr/share/pixmaps/NWT4x.png"));
#endif
  if(bl){
    a.installTranslator(&nwt4translator);
  }
  if(bp){
    w.setConfigPath(qsp);
    qDebug()<<"main.setConfigPath:"<<qsp;
  }

  w.setProgramPath(programpath);
  //qDebug()<<"main.setProgramPath:"<<programpath;
  w.cfgladen();
  w.ProgramInit();
  w.show();

  return a.exec();
}

