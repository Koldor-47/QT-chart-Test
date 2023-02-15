#include "widget.h"
#include "ui_widget.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QTextStream>
#include <QWidget>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>


Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget),
      m_listCount(3),
      m_valueMax(10),
      m_valueCount(7),
      m_dataTable(generateRandomData(m_listCount, m_valueMax, m_valueCount))
{
    ui->setupUi(this);
    this->resize(300, 500);

    auto action = new QAction{"Dummy Plot", this};
    action->setShortcut(QKeySequence{Qt::CTRL + Qt::Key_D});
    addAction(action);
    connect(action, &QAction::triggered, this, [this]{
        auto chartView = new QChartView(createLineChart());
        ui->horizontalLayout->addWidget(chartView, 1);
        m_charts << chartView;
    });


    QString thefilename;


}

Widget::~Widget()
{
    delete ui;
}

DataTable Widget::generateRandomData(int listCount, int valueMax, int valueCount) const
{
    DataTable dataTable;

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + QRandomGenerator::global()->bounded(valueMax / (qreal) valueCount);
            QPointF value((j + QRandomGenerator::global()->generateDouble()) * ((qreal) m_valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

QChart *Widget::createLineChart() const
{
    QChart *chart = new QChart();
    QLineSeries *test_series = new QLineSeries();

/*
    test_series->append(0, 6);
    test_series->append(2, 5);
    test_series->append(4, 10);
    test_series->append(6, 6);
    test_series->append(8, 5);
    test_series->append(10, 10);
    test_series->append(12, 6);
    test_series->append(14, 5);
    test_series->append(16, 10);
*/
    double x = 0.5;
    int y = 0;

    for (int i = 0; i < 5; i++)
    {
        test_series->append(x, y);
        x = x + 2.7;
        y = y + 3;

    }

    chart->addSeries(test_series);

    chart->setTitle("Nick's Line Chart");
    QString name("Series ");



    return chart;
}



void Widget::on_pushButton_clicked()
{

    thefilename = QFileDialog::getOpenFileName(this, tr("Open SigLog"), "/home/koldor", tr("Siglog Files (*.SIL)"));

    QRegularExpression looking("^[0-9][0-9] [A-Za-z_]*");

    QFile silLogData(thefilename);
    ui->listWidget->clear();

    if (!silLogData.open(QIODevice::ReadOnly | QIODevice::Text)) {

    }

    QTextStream stream(&silLogData);

    while (!stream.atEnd()){
        QString i = stream.readLine();
        QRegularExpressionMatch match = looking.match(i);

        if (match.hasMatch()){
            ui->listWidget->addItem(i);
        }

    }

    silLogData.close();

}

QChart *Widget::createSigLogChart(QString filename) const
{
    QChart *chart = new QChart();
    QLineSeries *test_series = new QLineSeries();
    QString sensorID = ui->listWidget->currentItem()->text().split(" ").first();
    QString sensorLine;
    QString lookingExpression = QString("^[0-9]*.[0-9]{3} %1 [0-9]*.[0-9]*").arg(sensorID);
    QRegularExpressionMatch match;
    QStringList sensorItems;
    qreal sensorTime;
    qreal sensorValue;

    QRegularExpression lookingSensorValue(lookingExpression);

    QFile sigLog(filename);

    if(!sigLog.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "No File Found";
        return chart;
    }

    QTextStream stream(&sigLog);
    while (!stream.atEnd()){
        sensorLine = stream.readLine();
        match = lookingSensorValue.match(sensorLine);
        if (match.hasMatch())
        {
            sensorItems = sensorLine.split(" ");

            sensorTime = sensorItems[0].toDouble();
            sensorValue = sensorItems[2].toDouble();

            test_series->append(sensorTime, sensorValue);

        }

    }

    chart->addSeries(test_series);
    chart->setTitle("Line Graph of " + ui->listWidget->currentItem()->text());
    QString name("Series ");

    return chart;

}

void Widget::on_pushButton_2_clicked()
{
    QMessageBox testbox;
    QChartView *chartView;


    if (ui->listWidget->count() == 0)
    {
        testbox.setText("Nothing Selected");
    } else if (ui->listWidget->currentRow() == -1)
    {
        testbox.setText("Nothing Selected");
    }
    else
    {
        this->resize(800, 500);
        testbox.setText(ui->listWidget->currentItem()->text());
        chartView = new QChartView(createSigLogChart(thefilename));
        ui->horizontalLayout->addWidget(chartView, 1);
        m_charts << chartView;
    }



    testbox.exec();


}


void Widget::on_pushButton_3_clicked()
{
    this->resize(800, 500);


}

