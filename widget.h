#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QWidget>

#include <QtCharts/QChartGlobal>


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

    QChart *createSigLogChart(QString filename) const;

private slots:
    void on_openFile_clicked();

    void on_makeGraph_clicked();

    void on_clearGraph_clicked();


private:
    Ui::Widget *ui;

    QString thefilename;
};
#endif // WIDGET_H
