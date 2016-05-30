#include "search.h"
#include "ui_search.h"
#define U_A "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36"

search::search(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::search)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

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
    key=arg1;
    timer.stop();
    if(!arg1.isEmpty()) {
        stop=false;
        timer.start(200);
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
    qDebug()<<"gotdata!";
    ui->listWidget->clear();
    QString html=reply->readAll();
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
    emit gotcitycode(code);
    close();
}

void search::closeEvent(QCloseEvent *e){
    QString nocode="";
    emit gotcitycode(nocode);
}
