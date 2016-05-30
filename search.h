#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include<QTimer>
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QRegularExpression>
#include<QListWidgetItem>
namespace Ui {
class search;
}

class search : public QWidget
{
    Q_OBJECT

public:
    explicit search(QWidget *parent = 0);
    ~search();
signals:
    void gotcitycode(QString&);
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void timeout();
    void gotdata();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
private:
    Ui::search *ui;
    QTimer timer;
    QRegularExpression reg;
    QRegularExpressionMatchIterator matchs;
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    void closeEvent(QCloseEvent *);
    QString key;
    bool stop;
};

#endif // SEARCH_H
