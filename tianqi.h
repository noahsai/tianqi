#ifndef TIANQI_H
#define TIANQI_H

#include <QWidget>
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QRegularExpression>
#include<QSettings>
#include<QDesktopWidget>
#include<QSystemTrayIcon>
#include<search.h>
#include<QMenu>
#include<QMouseEvent>
#include<QTimer>
#include<QPainter>
#include<QGraphicsColorizeEffect>
#include<QPixmap>
#include<QStandardPaths>
#include<setting.h>
#include<QDir>
#include<QLabel>
#include <X11/extensions/shape.h>
#include <QtX11Extras/QX11Info>
namespace Ui {
class tianqi;
}

class tianqi : public QWidget
{
    Q_OBJECT

public:
    explicit tianqi(QWidget *parent = 0);
    ~tianqi();
    void getskdata();
    void geticons();

public slots:
    void get7data();
    void settheme(QString fc, QString bgc, QString ic, int alp);
private slots:
    void set7();
    void setsk();
    void seticons();
    void gotcode(QString&);
    void on_city_clicked();
    void trayiconactive();
    void callsetting();
    void tolock();
    void setbgimg();

private:
    Ui::tianqi *ui;
    QString citycode;

    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QSystemTrayIcon *trayIcon;
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void saveset();
    void readset();
    void creattrayicon();
    void updateicon(QImage& im,QLabel* icon);
    QImage  colorchange(QImage * origin);

    search *sear;
    setting* set;
    QAction *quitAction ;
    QMenu *trayIconMenu;
    bool mousepressed;
    QPoint oldpos;
    QTimer *timer;
QGraphicsColorizeEffect *e1;
    QString fcolor,bgcolor,icolor;
    int alph;
    bool locked;
    QAction *lockitem;
    QPixmap bgpix;

protected :
         void paintEvent(QPaintEvent *);
};

#endif // TIANQI_H
