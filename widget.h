#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE


typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

QT_USE_NAMESPACE
QT_CHARTS_USE_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QChart *createSigLogChart(QString filename) const;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;
    QChart *createLineChart() const;

    Ui::Widget *ui;

    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    DataTable m_dataTable;
    QList<QChartView *> m_charts;
    QString thefilename;
};
#endif // WIDGET_H
