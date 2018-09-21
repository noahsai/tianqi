#include "search.h"
#include "ui_search.h"
#define U_A "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36"

search::search(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::search)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    document = new  QJsonDocument ;
    key="";
    reply=NULL;
    stop=false;
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

search::~search()
{
    delete ui;
}

void search::on_lineEdit_textChanged(const QString &arg1)
{
    qDebug()<<"textchanged!";
    if(ui->lineEdit->text().isEmpty())
    {
        ui->listWidget->clear();
        return;
    }
    stop=true;
    if(reply!=NULL)
    {
        reply->abort();
    }
    key=arg1.toLower();//统一使用小写。
    timer.stop();
    if(!arg1.isEmpty()) {
        stop=false;
        timer.start(300);
    }
    qDebug()<<"timer started!";

}

void search::timeout()
{
    qDebug()<<"timeout!";
    QNetworkRequest request;

    timer.stop();
    QString url="http://toy1.weather.com.cn/search?cityname="+key;
    QString refer="http://www.weather.com.cn";
    request.setRawHeader(QByteArray("User-Agent"), U_A);
    request.setRawHeader(QByteArray("Referer"), refer.toLatin1());
    request.setUrl(QUrl(url));
    manager = new QNetworkAccessManager;
    reply=manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(gotdata()));
    qDebug()<<"sk get started!";

}

void search::gotdata()
{
    if(stop||ui->lineEdit->text().isEmpty())
    {
        qDebug()<<"reply stop!";
        reply->deleteLater();
        manager->deleteLater();
        reply = NULL;
        stop=false;
        return;
    }
    //能到这里证明stop=false
    ui->listWidget->clear();//清空
    //搜索服务器有问题时使用下面的本地搜索
    if(reply->error()!=QNetworkReply::NoError) {
        qDebug()<<"搜索服务器出错，使用本地搜索";
        findinfile();
        return;
    }
    //findinfile();return;//测试

    qDebug()<<"gotdata!";
    QString html=reply->readAll();
    qDebug()<<html;
    reply->deleteLater();
    reply=NULL;
    manager->deleteLater();
    if(html.indexOf("([{")!=-1)
    {
        reg.setPattern("(?<=ref\":\").+?(?=\")");
        matchs=reg.globalMatch(html);
        QListWidgetItem *item;
        while(matchs.hasNext())
        {
            item=new QListWidgetItem;
            QStringList list=matchs.next().captured(0).split("~");
            QString text=list.at(2)+"-"+list.last();
            item->setText(text);
            item->setToolTip(list.at(0));
            ui->listWidget->addItem(item);
        }
    }
    qDebug()<<"data showed!";

}

void search::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString code=item->toolTip();
    QString name = item->text().split("-").at(0);
    emit gotcitycode(code , name);
    close();
}

void search::closeEvent(QCloseEvent *e){
    QString nocode="";
    QString noname="";
    emit gotcitycode(nocode , noname);
}


void search::findinfile(){

    if(document->isEmpty()) {
        QFile file(":/city_data");
        if(file.open(QIODevice::ReadOnly)){
            QString citydata = file.readAll();
            //qDebug()<<citydata;
            QJsonParseError json_error;
            *document = QJsonDocument::fromJson(citydata.toUtf8(),&json_error);
            if(json_error.error != QJsonParseError::NoError) {
                qDebug()<<"json error"<<json_error.errorString();
                return;
            }
        }
        file.close();
    }
    //QString data = "[{\"s\":[true,1,5]},{\"s\":[true,1,5]}]";
    //qt 的 json不能解析{a:"xxx"}这种，只能{"a":"xxx"}这种key为字符串的json
    QJsonObject ob;
    QString n,p,pc,pv,fl;
    QJsonArray arr;
    if((!oldkey.isEmpty())&&(key.indexOf(oldkey)>-1 ))//继续输入时使用之前的结果，但减少不可以，因为tmp是不断更新的
    {
        arr = tmp;
        //qDebug()<<"arr = tmp";
    }
    else {
         arr = document->array();
         //qDebug()<<"arr = document->array()";
    }
    tmp = QJsonArray();//清空，准备储存新的结果
    for(int i=0;i<arr.count();i++)
    {
        ob = arr.at(i).toObject();
        n = ob.value("n").toString();
        p = ob.value("p").toString();//p和pc可以随便用
        pc = ob.value("pc").toString();
        pv = ob.value("pv").toString();
        fl = ob.value("fl").toString();
        p = n + p + pc + pv + fl;//p和pc可以随便用
        if(p.indexOf(key)!=-1)
        {
            //qDebug()<<"Found:"<<p;
            tmp +=arr.at(i);
            QListWidgetItem *item;
            item=new QListWidgetItem;
            item->setText(n + "-" + pv);
            item->setToolTip(ob.value("ac").toString());
            ui->listWidget->addItem(item);
        }
        //qDebug()<<"url:"<<ob.value("n");
    }

    oldkey = key;//备份当前搜索词
}
