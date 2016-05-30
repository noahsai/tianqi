#include "setting.h"
#include "ui_setting.h"
extern QString cfgpath;
setting::setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    selectcolor= new QColorDialog(this);
}

setting::~setting()
{
    delete ui;
}

void setting::on_pushButton_2_clicked()
{
    QFile file;
    QDataStream out;
    QString filepath=cfgpath+"theme";
    out.setDevice(&file);
    file.setFileName(filepath);
    if(file.open(QIODevice::WriteOnly))
    {
        out<<ui->fontcolor->text();
        out<<ui->bgcolor->text();
        out<<ui->icolor->text();
        out<<(int) ui->alph->value();
    }
    else {
        qDebug()<<"setting:setting file open error!";
    }
    file.close();
    close();
}

void setting::init(QString &fc, QString &bgc, QString &ic, int alp)
{
    ofc=fc;
    obgc=bgc;
    oic=ic;
    oalp=alp;
    //QPalette pe;
    pe.setColor(QPalette::Button,QColor(fc));
    ui->fontcolor->setPalette(pe);
    ui->fontcolor->setText(fc);
  //  ui->fontcolor->setStyleSheet("background-color:"+fc);
    pe.setColor(QPalette::Button,QColor(bgc));
    ui->bgcolor->setPalette(pe);
    ui->bgcolor->setText(bgc);
  //  ui->bgcolor->setStyleSheet("background-color:"+bgc);
    pe.setColor(QPalette::Button,QColor(ic));
    ui->icolor->setPalette(pe);
    ui->icolor->setText(ic);
  //  ui->icolor->setStyleSheet("background-color:"+ic);
    ui->alph->setValue(alp);
  //  this->setStyleSheet("color:#000000");
}


void setting::on_fontcolor_clicked()
{
    color="";
    colorname="";
    color = selectcolor->getColor(QColor(ui->fontcolor->text()), this);
    if(color.isValid())
    {
        colorname=color.name();
        ui->fontcolor->setText(colorname);
    //    ui->fontcolor->setStyleSheet("background-color:"+colorname);
        //QPalette pe = ui->fontcolor->palette();
        pe = ui->fontcolor->palette();
        pe.setColor(QPalette::Button,QColor(colorname));
        ui->fontcolor->setPalette(pe);
       emit settheme(colorname,"","",-1);
    }
}

void setting::on_bgcolor_clicked()
{
    color="";
    colorname="";
    color =selectcolor->getColor(QColor(ui->bgcolor->text()), this);
    if(color.isValid())
    {
        colorname=color.name();
        ui->bgcolor->setText(colorname);
     //   ui->bgcolor->setStyleSheet("background-color:"+colorname);
        //QPalette pe = ui->bgcolor->palette();
        pe = ui->bgcolor->palette();
        pe.setColor(QPalette::Button,QColor(colorname));
        ui->bgcolor->setPalette(pe);
       emit settheme("",colorname,"",-1);
    }
}

void setting::on_icolor_clicked()
{
    color="";
    colorname="";
    color = selectcolor->getColor(QColor(ui->icolor->text()), this);
    if(color.isValid())
    {
        colorname=color.name();
        ui->icolor->setText(colorname);
    //    ui->icolor->setStyleSheet("background-color:"+colorname);
        //QPalette pe = ui->icolor->palette();
        pe = ui->icolor->palette();
        pe.setColor(QPalette::Button,QColor(colorname));
        ui->icolor->setPalette(pe);
       emit settheme("","",colorname,-1);
    }
}

void setting::on_alph_valueChanged(int value)
{
   emit settheme("","","",value);
}

void setting::on_pushButton_clicked()
{
    emit settheme(ofc,obgc,oic,oalp);
    close();
}
