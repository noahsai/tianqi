#include "tianqi.h"
#include "ui_tianqi.h"
#define U_A "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36"
QString cfgpath=QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/tianqi/";

tianqi::tianqi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tianqi)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground );//背景透明
    setWindowFlags(Qt::FramelessWindowHint
                   |Qt::Tool);

    QDir().mkpath(cfgpath);
    e1 = new QGraphicsColorizeEffect(this);
    e1->setColor(QColor(0, 0, 0));
    e1->setStrength(1.0);

    manager= new QNetworkAccessManager(this);
    request.setRawHeader(QByteArray("User-Agent"), U_A);

    //locked=false;
    usebgc = false;
    sear=NULL;
    reply=NULL;
    set=NULL;
    //lockitem=NULL;
    mousepressed=false;
    oldpos.setX(0);
    oldpos.setY(0);
    QIcon icon = QIcon(":/icon.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayiconactive()));
    creattrayicon();
    trayIcon->show();
    //-------[默认配色]----------
    fcolor="#ffffff";
    bgcolor="#000000";
    icolor="#ffffff";
    alph=150;
    //--------------------------
    initmap();
    readset();
    setLayout(ui->verticalLayout);
    timer =new QTimer(this);
    timer->setInterval(900000);//15分钟刷新一次
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(get7data()));
    timer->start();
    //timer->start(2500);/2.5秒一次，用于测试
    get7data();
    this->resize(50,50);//设小点，让程序自己调整最适合大小
}

tianqi::~tianqi()
{
    saveset();
    delete ui;
}

void tianqi::get7data(){
    QString url;
    //======旧版直接解析网页======
    //url="http://www.weather.com.cn/weather/"+citycode+".shtml";
    //========================
     if(citycode.left(3)=="101")   url="http://d1.weather.com.cn/wap_40d/"+citycode+".html";
    else  url="http://d1.weather.com.cn/weixinfc_gw/"+citycode+".htm";
    request.setUrl(QUrl(url));
    QString refer="http://m.weather.com.cn/mweather/"+citycode+".shtml";
    request.setRawHeader(QByteArray("Referer"), refer.toLatin1());
    reply=manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(set7()));

    qDebug()<<"to get 7day data."<<url;


}

void tianqi::getskdata(){
    QString url;
     if(citycode.left(3)=="101")   url="http://d1.weather.com.cn/sk_2d/"+citycode+".html";
    else  url=url="http://d1.weather.com.cn/obs_forg/"+citycode+".htm";
    QString refer="http://m.weather.com.cn/mweather/"+citycode+".shtml";
    request.setRawHeader(QByteArray("Referer"), refer.toLatin1());
    request.setUrl(QUrl(url));
    reply=manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(setsk()));

}

void tianqi::geticons(){

}

void tianqi::set7(){
    QRegularExpression reg;
    QRegularExpressionMatch match;
    QRegularExpressionMatchIterator matchs;
    QNetworkReply *re = qobject_cast<QNetworkReply *>(sender());

    QString html=re->readAll();
    re->deleteLater();
    qDebug()<<"Got 7day data.";
    manager->clearAccessCache();
    //qDebug()<<html;

    reg.setPattern("{\".+?\"}");
    matchs=reg.globalMatch(html);
    if(!matchs.hasNext()) {
        qDebug()<<html;
        getskdata();
        return;
    }

    for(int it=0;it<4&&matchs.hasNext();it++)
    {
        QString date;
        QString image;
        QString tmp;
        QString night;
        match=matchs.next();
        QString li=match.captured(0);
       // qDebug()<<li;
         if(citycode.left(3)=="101")  {
            reg.setPattern("009\":\".+?(?=\")");//日期
            date=reg.match(li).captured(0);
            reg.setPattern("016\":\".+?(?=\")");//星期
            QString day=reg.match(li).captured(0);
           date = date.right(2) + "(" + day.right(1) + ")";
            //date.replace("（","\n（");
    //       reg.setPattern("(?<=001\":\").+?(?=\")");//天气
            reg.setPattern("(?<=001\":\").+?(?=\")");//图标
            image=reg.match(li).captured(0);
            image.remove(" ");
            QString d = image;
            reg.setPattern("(?<=002\":\").+?(?=\")");//图标
             night =  reg.match(li).captured(0);
            if(night != d)   night = map.value(d) +"转"+ map.value(night);
            else night = map.value(d);

            image = "d"+ image +".png";
            reg.setPattern("(?<=003\":\")\\d+?(?=\")");//温度
            tmp=reg.match(li).captured(0);
            reg.setPattern("(?<=004\":\")\\d+?(?=\")");//温度
            QString tmp2=reg.match(li).captured(0);
            if(it != 0) tmp = tmp +"/" +tmp2 + "°";
            qDebug()<<date<<tmp<<image;
        //-------------------------------------
        }
        else {
            reg.setPattern("fi\":\".+?(?=\")");//日期
            date=reg.match(li).captured(0);
            reg.setPattern("fj\":\".+?(?=\")");//星期
            QString day=reg.match(li).captured(0);
           date = date.right(2) + "(" + day.right(1) + ")";
            //date.replace("（","\n（");
    //       reg.setPattern("(?<=001\":\").+?(?=\")");//天气
            reg.setPattern("(?<=fa\":\").+?(?=\")");//图标
            image=reg.match(li).captured(0);
            image.remove(" ");
            QString d = image;
            reg.setPattern("(?<=fb\":\").+?(?=\")");//图标
             night =  reg.match(li).captured(0);
            if(night != d)   night = map.value(d) +"转"+ map.value(night);
            else night = map.value(d);

            image = "d"+ image +".png";
            reg.setPattern("(?<=fc\":\")\\d+?(?=\")");//温度
            tmp=reg.match(li).captured(0);
            reg.setPattern("(?<=fd\":\")\\d+?(?=\")");//温度
            QString tmp2=reg.match(li).captured(0);
            if(it != 0) tmp = tmp +"/" +tmp2 + "°";
            qDebug()<<date<<tmp<<image;
        }
        QImage im;
        im.load(":/"+image);
         colorchange(im);
        QPixmap pixmap;
        pixmap = pixmap.fromImage(im);
        int size = 40;
        if(it==0) size = 60;
        pixmap = pixmap.scaled(size,size,Qt::KeepAspectRatio,Qt::SmoothTransformation);

        switch(it)
        {
        case 0:
            ui->date->setText(date);
            tmp.remove("°");
            ui->temp->setText(tmp);
            ui->icon->setPixmap(pixmap);
            trayIcon->setIcon(QIcon(pixmap));
            ui->icon->setToolTip(night);
            break;
        case 1:
            ui->date2->setText(date);
          //  tmp+="°";
            ui->temp2->setText(tmp);
            ui->icon2->setPixmap(pixmap);
            ui->icon2->setToolTip(night);
            break;
        case 2:
            ui->date3->setText(date);
          //  tmp+="°";
            ui->temp3->setText(tmp);
            ui->icon3->setPixmap(pixmap);
            ui->icon3->setToolTip(night);
            break;
        case 3:
            ui->date4->setText(date);
           // tmp+="";
            ui->temp4->setText(tmp);
            ui->icon4->setPixmap(pixmap);
            ui->icon4->setToolTip(night);
            break;
        default:break;
        }
    }

    /*
     * 旧版直接解析网页数据
    reg.setPattern("id=\"7d\"[\\s\\S]+?分时段预报</em>");
    match=reg.match(html);
    if(!match.hasMatch()) {
        qDebug()<<html;
        getskdata();
        return;
    }
    html=match.captured(0);
    //qDebug()<<html;
    reg.setPattern("<li[\\s\\S]+?</li>");
    matchs=reg.globalMatch(html);
    for(int it=0;it<4&&matchs.hasNext();it++)
    {
        match=matchs.next();
        QString li=match.captured(0);
       // qDebug()<<li;
        reg.setPattern("(?<=<h1>).+?(?=<)");//日期
        QString date=reg.match(li).captured(0);
       date.remove(QRegularExpression("（.*）"));
        //date.replace("（","\n（");
        reg.setPattern("(?<=\"wea\">).+?(?=<)");//天气
        QString wea=reg.match(li).captured(0);
        reg.setPattern("[dn]\\d\\d");//图标
        QString image=reg.match(li).captured(0);
        image.remove(" ");
        image+=".png";
        reg.setPattern("(?<=\"tem\">\\n)[\\s\\S]+?(?=\\n</p>)");//温度
        //QRegularExpressionMatchIterator tmpmatchs =reg.globalMatch(li);
        QString tmp="";

        tmp=reg.match(li).captured(0);
        tmp.remove(QRegularExpression("<.+?>"));
        tmp.remove("\n");
        if(it==0) {
            tmp.remove(QRegularExpression("[°/].*"));
        }
        else tmp.replace("°/","/");
        qDebug()<<date<<wea<<tmp<<image;
        //-------------------------------------
        QFont ft;//根据wea长度设置字体大小
        wea.length()>4 ? ft.setPointSize(13) : ft.setPointSize(16);
        ui->weather->setFont(ft);
        //-------------------------------------
        QImage im;
        im.load(":/"+image);
        im = colorchange(&im);
        QPixmap pixmap;
        pixmap=pixmap.fromImage(im);
        int size = 40;
        if(it==0) size = 60;
        pixmap=pixmap.scaled(size,size,Qt::KeepAspectRatio,Qt::SmoothTransformation);

        switch(it)
        {
        case 0:
            ui->date->setText(date);
            ui->weather->setText(wea);
            ui->icon->setToolTip(wea);
            tmp.remove("°");
            ui->temp->setText(tmp);
            ui->icon->setPixmap(pixmap);
            trayIcon->setIcon(QIcon(pixmap));

            break;
        case 1:
            ui->date2->setText(date);
            ui->icon2->setToolTip(wea);
          //  tmp+="°";
            ui->temp2->setText(tmp);
            ui->icon2->setPixmap(pixmap);
            break;
        case 2:
            ui->date3->setText(date);
            ui->icon3->setToolTip(wea);
          //  tmp+="°";
            ui->temp3->setText(tmp);
            ui->icon3->setPixmap(pixmap);
            break;
        case 3:
            ui->date4->setText(date);
            ui->icon4->setToolTip(wea);
           // tmp+="°";
            ui->temp4->setText(tmp);
            ui->icon4->setPixmap(pixmap);
            break;
        default:break;
        }
    }
    */
    getskdata();

}

void tianqi::setsk(){
    qDebug()<<"Got sk data.";
    QStringList list;
    QNetworkReply *re = qobject_cast<QNetworkReply *>(sender());

    QString html=re->readAll();
    re->deleteLater();
    //qDebug()<<html;
    manager->clearAccessCache();

    if(html.indexOf("dataSK")!=-1 || html.indexOf("obs_forg")!=-1)
    {
        list=html.split(",");
        ui->pm25->hide();//先隐藏，有才显示
        for(int i=0;i<list.length();i++)
        {
            QString text;
            QString data=list.at(i);
            if(data.indexOf("无实")!=-1)continue;
            if(data.indexOf("cityname")!=-1)
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                ui->city->setText(text);
            }
            else if(data.indexOf("temp\"")!=-1)
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                text+="°";
                ui->temp->setText(text);
            }
            else if(data.indexOf("weather\"")!=-1)
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                QFont ft;
                text.length()>2 ? ft.setPointSize(12) : ft.setPointSize(14);
                ui->weather->setFont(ft);
                ui->weather->setText(text);
            }
            else if(data.indexOf("weathercode")!=-1)//image
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                //获取背景图片======
                QString url="http://i.tq121.com.cn/i/wap2017/bgs/"+text+".jpg";//这个api可能并不长久
                qDebug()<<"img url:"<<url;
                QString refer="http://m.weather.com.cn/mweather/"+citycode+".shtml?location=now";
                request.setRawHeader(QByteArray("Referer"), refer.toLatin1());
                request.setUrl(QUrl(url));
                reply=manager->get(request);
                connect(reply,SIGNAL(finished()),this,SLOT(setbgimg()));

                //想放在前面就只有去掉continue;
                QImage image;
                image.load(":/"+text+".png");
                if(image.isNull()) continue;
                 colorchange(image);
              //  QPixmap pixmap(":/"+wea);
                QPixmap pixmap ;
                pixmap=pixmap.fromImage(image);
               QPixmap pix=pixmap.scaled(60,60,Qt::KeepAspectRatio,Qt::SmoothTransformation);
                ui->icon->setPixmap(pix);
                trayIcon->setIcon(QIcon(pix));

            }
            else if(data.indexOf("SD")!=-1)//image
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                if(text.right(1) != "%") text += "%";
                text = "相对湿度 "+text;
                ui->shidu->setText(text);

            }
            else if(data.indexOf("WD")!=-1)//image
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                if(text.indexOf("无")!=-1) text = "无定向";
                else if(text.right(1) != " ") text += " ";

                ui->feng->setText(text);

            }
            else if(data.indexOf("WS")!=-1)//image
            {
                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                if(text.right(1) != "级") text += "级";
                text = ui->feng->text() + text;
                ui->feng->setText(text);

            }
            else if(data.indexOf("aqi")!=-1)
            {
                ui->pm25->setAutoFillBackground(true);
                QPalette pe=ui->pm25->palette();
                pe.setColor(QPalette::WindowText,QColor(255, 255, 255));

                text=data.split(":").takeLast();
                text.remove("\"");
                text.remove("}");
                if(text.isEmpty()) continue;
                else{
                    int num=text.toInt();
                    if(num<=50) {
                        text+="优";
                        pe.setColor(QPalette::Window,QColor("#44cf12"));
                        ui->pm25->setPalette(pe);
                        //ui->pm25->setStyleSheet("background-color:rgb(0, 220, 106);color: rgb(255, 255, 255);");
                    }
                    else if(num<=100)
                    {
                        text+="良";
                        pe.setColor(QPalette::Window,QColor("#87CEFA"));
                        ui->pm25->setPalette(pe);
                      //  ui->pm25->setStyleSheet("background-color:rgb(255, 220, 19);color: rgb(255, 255, 255);");

                    }
                    else if(num<=150)
                    {
                        text+="轻度";
                        pe.setColor(QPalette::Window,QColor(238, 147, 0));
                        ui->pm25->setPalette(pe);
                        //ui->pm25->setStyleSheet("background-color:rgb(238, 147, 0);color: rgb(255, 255, 255);");
                    }
                    else if(num<=200)
                    {
                        text+="中度";
                        pe.setColor(QPalette::Window,QColor(204, 95, 97));
                        ui->pm25->setPalette(pe);
                        //ui->pm25->setStyleSheet("background-color:rgb(204, 95, 97);color: rgb(255, 255, 255);");
                    }
                    else if(num<=300)
                    {
                        text+="重度";
                        pe.setColor(QPalette::Window,QColor(180, 69, 154));
                        ui->pm25->setPalette(pe);
                        //ui->pm25->setStyleSheet("background-color:rgb(180, 69, 154);color: rgb(255, 255, 255);");
                    }
                    else if(num>300)
                    {
                        text+="严重";
                        pe.setColor(QPalette::Window,QColor(138, 0, 0));
                        ui->pm25->setPalette(pe);
                        //ui->pm25->setStyleSheet("background-color:rgb(138, 0, 0);color: rgb(255, 255, 255);");
                    }
                    ui->pm25->setText(text);
                    ui->pm25->show();//有数据就显示
                }
            }
        }
    }
    QString text=ui->city->text()+" "+ui->temp->text()+" "+ui->weather->text();
    trayIcon->setToolTip(text);
   // QPalette pal = palette();

   // pal.setColor(QPalette::, QColor(0x00,0xff,0x00,100)); setPalette(pal);
}
void tianqi::paintEvent(QPaintEvent *e)
{
    if(!mousepressed)
    {
        QPainter p(this);
        QColor  color;
        color.setNamedColor(bgcolor);
        color.setAlpha(alph);
        if(!bgpix.isNull() && (usebgc == 0) )
        {
            p.drawPixmap(rect(),bgpix);
            p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        }
        p.fillRect(rect(),color);//别用else，这是用来绘制透明的
        p.end();
    }
}
void tianqi::seticons(){

}
void tianqi::saveset(){
    QSettings settings("ShengSoft", "Weather");
    settings.setValue("citycode",citycode);
    settings.setValue("cityname",cityname);
   // settings.setValue("size", size());//因为没得调大小，所以不要记录大小了
    //settings.setValue("locked",locked);
    settings.setValue("pos", pos());
    settings.setValue("usebgc", usebgc);
}

void tianqi::readset(){
    QSettings settings("ShengSoft", "Weather");
    citycode=settings.value("citycode",QString("101010100")).toString();//默认北京
    cityname=settings.value("cityname",QString("北京")).toString();
    ui->city->setText(cityname);
 //   resize(settings.value("size", QSize(154, 144)).toSize());//因为没得调大小，所以不要记录大小了
    //locked=settings.value("locked",false).toBool();
    //qDebug()<<"readset locked:"<<locked;
    //locked=!locked;//因为tolock函数是一个动作，想恢复锁定就要先设为为锁定，这样tolock就是锁定动作了
    //tolock();
    int x=QApplication::desktop()->width()-250;
    QPoint point;
    point=settings.value("pos", QPoint(x, 50)).toPoint();
    if(point.x()<0||point.x()>QApplication::desktop()->width()-20) point.setX(x);
    if(point.y()<0||point.y()>QApplication::desktop()->height()+this->height()-20) point.setY(50);
    move(point);
    //-------------------------------
    QString filepath=cfgpath+"theme";
    qDebug()<<"read setiing";
    QFile file;
    QDataStream out;
    out.setDevice(&file);
    file.setFileName(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        out>>fcolor;
        out>>bgcolor;
        out>>icolor;
        out>>alph;
         qDebug()<<"theme read."<<fcolor<<endl<<bgcolor<<endl<<icolor<<endl<<alph;
    }
    else {
        qDebug()<<"tianqi setting file open error!";
    }
    file.close();

    //QString style="QLabel,QPushButton,#line{ color:"+fcolor+";}";
    //this->setStyleSheet(style);
    QPalette pe = palette();
    pe.setColor(QPalette::WindowText,QColor(fcolor));
    pe.setColor(QPalette::ButtonText,QColor(fcolor));
    setPalette(pe);
}

void tianqi::on_city_clicked()
{
   // show();
    if(!sear)
    {
        sear=new search(this);
        sear->setWindowTitle("搜索城市");
        sear->setWindowFlags(Qt::Dialog);
        connect(sear,SIGNAL(gotcitycode(QString& , QString&)),this,SLOT(gotcode(QString& ,QString&)));
        sear->move(QApplication::desktop()->width()/2-sear->width()/2,QApplication::desktop()->height()/2-sear->height()/2);
        sear->show();
    }
    sear->show();
}
void tianqi::gotcode(QString& code , QString& name)
{
    if(code!="")
    {
        citycode=code;
        cityname = name;
        ui->city->setText(name);
        saveset();
        get7data();
    }
    sear=NULL;

}
void tianqi::creattrayicon()
{
    QAction *change=new QAction(tr("切换城市"),this);
    connect(change,SIGNAL(triggered()),ui->city,SLOT(click()));
    QAction *set=new QAction(tr("设置"),this);
    connect(set,SIGNAL(triggered()),this,SLOT(callsetting()));
    //lockitem=new QAction(tr("锁定"),this);
    //connect(lockitem,SIGNAL(triggered()),this,SLOT(tolock()));
    quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));//若触发了退出就退出程序
    trayIconMenu = new QMenu(this);//菜单
    trayIconMenu->addAction(change);//
    trayIconMenu->addAction(set);
    //trayIconMenu->addAction(lockitem);

    trayIconMenu->addAction(quitAction);//把退出加到入菜单项
    trayIcon->setContextMenu(trayIconMenu);//设置托盘上下文菜单为trayIconMenu

}
void tianqi::trayiconactive()
{
//    if(!isActiveWindow()) {
//        show();
        activateWindow();
//    }
//    else hide();
}

/*
//不要置顶的功能了
void tianqi::tolock()
{

    if(locked)//取消置顶
    {
        setWindowFlags(Qt::FramelessWindowHint
                       |Qt::Tool);
        XRectangle* myrect = new XRectangle;
        myrect->x = 0;
        myrect->y = 0;
        myrect->width = width();
        myrect->height = height();
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput,0,0, myrect, 1, ShapeSet, YXBanded);
        lockitem->setText("置顶穿透");
        qDebug()<<"unlocked";
        locked=false;
        show();
    }
    else//置顶穿透
    {
        setWindowFlags(Qt::FramelessWindowHint
                       |Qt::BypassWindowManagerHint
                       |Qt::WindowStaysOnTopHint
                       |Qt::Tool);
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput,0,0, NULL, 0, ShapeSet, YXBanded);
        qDebug()<<"locked";
        lockitem->setText("取消置顶");
        locked=true;
        show();
    }
}
*/
void tianqi::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton && !mousepressed)
    {
        mousepressed=true;
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void tianqi::mouseMoveEvent(QMouseEvent * event){
    if(mousepressed)
    {
        move(event->globalPos()-oldpos);//貌似linux要这样
        event->accept();
    }
}

void tianqi::mouseReleaseEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton && mousepressed)
    {
        int x=this->x();
        int y=this->y();
//        qDebug()<<x<<endl<<y;

        if(this->pos().x()<0) x=0;
        else if(QApplication::desktop()->width()-x<this->width()){
            x=QApplication::desktop()->width()-this->width();
        }

        if(this->pos().y()<0) y=0;
        else if(QApplication::desktop()->height()-y<this->height())
        {
            y=QApplication::desktop()->height()-this->height();
        }
//        qDebug()<<x<<endl<<y;
        move(x,y);
        mousepressed=false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }
    update();
}

void  tianqi::colorchange(QImage & origin){
  QImage newImage(origin.width(), origin.height(), QImage::Format_ARGB32);
  for(int y = 0; y<newImage.height(); y++){
    QRgb * line = (QRgb *)origin.scanLine(y);

    for(int x = 0; x<origin.width(); x++){

      int alp = qAlpha(line[x]);
      QColor color;
      color.setNamedColor(icolor);
      color.setAlpha(alp);
      if(alp>0)  newImage.setPixel(x,y, color.rgba());
      else newImage.setPixel(x,y, qRgba(255, 255, 255,0));
      //qDebug()<<origin.width()<<origin.height()<<alp;
    }
  }
origin = newImage;
  //return newImage;
}

void tianqi::updateicon(QLabel* icon)
{
    QImage im;
    im = icon->pixmap()->toImage();
    colorchange(im);
    QPixmap pixmap;
    pixmap = pixmap.fromImage(im);
    icon->setPixmap(pixmap);
}

void tianqi::settheme(QString fc, QString bgc, QString ic, int alp,int ubgc)
{
    if(!fc.isEmpty()&&fcolor!=fc) {
        fcolor=fc;
        QPalette pe = palette();
        pe.setColor(QPalette::WindowText,QColor(fcolor));
        pe.setColor(QPalette::ButtonText,QColor(fcolor));
        setPalette(pe);
        //QString style="QLabel,QPushButton,#line{ color:"+fcolor+";}";
        //this->setStyleSheet(style);
    }
    if(!bgc.isEmpty()&&bgcolor!=bgc) {
        bgcolor=bgc;
    }
    if(!ic.isEmpty()&&icolor!=ic){
        icolor=ic;
        updateicon(ui->icon);
        updateicon(ui->icon2);
        updateicon(ui->icon3);
        updateicon(ui->icon4);
    }
    if(alp!=-1&&alph!=alp){
        alph=alp;
    }
   if(ubgc!=-1) usebgc = ubgc;
   update();

}

void tianqi::callsetting()
{
   // show();
    set=new setting(this);
    set->setAttribute(Qt::WA_DeleteOnClose);
    set->setWindowTitle("设置主题");
    set->setModal(true);
    set->init(fcolor,bgcolor,icolor,alph,usebgc);
    connect(set,SIGNAL(settheme(QString,QString,QString,int,int)),this,SLOT(settheme(QString,QString,QString,int,int)));
    set->move(QApplication::desktop()->width()/2-set->width()/2,QApplication::desktop()->height()/2-set->height()/2);
    set->show();
    qDebug()<<"callsetting";

}

void tianqi::setbgimg(){
    qDebug()<<"set bg img";
    QNetworkReply *re = qobject_cast<QNetworkReply *>(sender());
    if(re->error() != QNetworkReply::NoError) {
        bgpix = QPixmap(0,0);
        qDebug()<<"get bgimg error!"<<bgpix.isNull();
        return;
    }
    //QPixmap pix;
    bgpix.loadFromData(re->readAll());
    int w , h;
    w = bgpix.width();
    h = bgpix.height();
    h = (this->height()*1.0/this->width()) * w ;
    //qDebug()<<w<<h;
    bgpix = bgpix.copy(0,0,w,h);
    re->deleteLater();
    update();
}

void tianqi::initmap()
{
    map["00"]="晴";
    map["01"]="多云";
    map["02"]="阴";
    map["03"]="阵雨";
    map["04"]="雷阵雨";
    map["05"]="雷阵雨伴有冰雹";
    map["06"]="雨夹雪";
    map["07"]="小雨";
    map["08"]="中雨";
    map["09"]="大雨";
    map["10"]="暴雨";
    map["11"]="大暴雨";
    map["12"]="特大暴雨";
    map["13"]="阵雪";
    map["14"]="小雪";
    map["15"]="中雪";
    map["16"]="大雪";
    map["17"]="暴雪";
    map["18"]="雾";
    map["19"]="冻雨";
    map["20"]="沙尘暴";
    map["21"]="小到中雨";
    map["22"]="中到大雨";
    map["23"]="大到暴雨";
    map["24"]="暴雨到大暴雨";
    map["25"]="大暴雨到特大暴雨";
    map["26"]="小到中雪";
    map["27"]="中到大雪";
    map["28"]="大到暴雪";
    map["29"]="浮尘";
    map["30"]="扬沙";
    map["31"]="强沙尘暴";
    map["53"]="霾";
    map["99"]="无";
    map["32"]="浓雾";
    map["49"]="强浓雾";
    map["54"]="中度霾";
    map["55"]="重度霾";
    map["56"]="严重霾";
    map["57"]="大雾";
    map["58"]="特强浓雾";
    map["301"]="雨";
    map["302"]="雪";
}

