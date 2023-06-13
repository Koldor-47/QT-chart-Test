#ifndef NICKCUSTOMWIDGET_H
#define NICKCUSTOMWIDGET_H

#include <QObject>
#include <QWidget>

class nickCustomWidget : public QObject
{
    Q_OBJECT
public:
    explicit nickCustomWidget(QObject *parent = nullptr);

signals:

};

#endif // NICKCUSTOMWIDGET_H
