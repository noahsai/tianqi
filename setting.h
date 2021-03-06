#ifndef SETTING_H
#define SETTING_H
#include<QDebug>

#include <QDialog>
#include<QFile>
#include<QColorDialog>
namespace Ui {
class setting;
}

class setting : public QDialog
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = 0);
    void init(QString& fc, QString& bgc, QString& ic, int alp ,int ubgc);
    ~setting();
signals:
    void settheme(QString fc, QString bgc, QString ic, int alp,int ubgc);
private slots:
    void on_pushButton_2_clicked();

    void on_fontcolor_clicked();

    void on_bgcolor_clicked();

    void on_icolor_clicked();

    void on_alph_valueChanged(int value);

    void on_pushButton_clicked();

    void on_usebgcolor_clicked(bool checked);

private:
    Ui::setting *ui;
    QString ofc,obgc,oic;
    int oalp;
    QColorDialog* selectcolor;
    QColor color;
    QString colorname;
    QPalette pe;
    int usebgc;
};

#endif // SETTING_H
