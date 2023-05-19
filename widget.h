#ifndef WIDGET_H
#define WIDGET_H

#include "lib/qcustomplot.h"

#include <QtWidgets/QWidget>
#include <QMap>




QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class QChartView;
class QChart;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QCustomPlot *createSigLogChart(QString &filename) const;
    QMap<QString, QVector<QCPGraphData>> getNicksData(QString &fileName)const;

private slots:
    void on_openFile_clicked();

    void on_makeGraph_clicked();

    void on_clearGraph_clicked();


private:
    Ui::Widget *ui;
    QString thefilename;
};
#endif // WIDGET_H
