#include "widget.h"
#include "ui_widget.h"
#include "koldorchartview.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QTextStream>
#include <QWidget>
#include <QDateTimeAxis>
#include <QValueAxis>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>


Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->resize(300, 500);

    auto action = new QAction{"Dummy Plot", this};
    action->setShortcut(QKeySequence{Qt::CTRL | Qt::Key_D});
    addAction(action);
    connect(action, &QAction::triggered, this, [this]{
        auto chartView = new QChartView(createSigLogChart(thefilename));
        ui->horizontalLayout->addWidget(chartView, 1);
    });


    //QString thefilename;


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_openFile_clicked()
{

    thefilename = QFileDialog::getOpenFileName(this, tr("Open SigLog"), "/home/koldor", tr("Siglog Files (*.SIL)"));

    static QRegularExpression looking("^[0-9][0-9] [A-Za-z_]*");

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

    this->setWindowTitle(thefilename);
    silLogData.close();

}

QLineSeries *Widget::makeDataSeries(QString &fileName) const
{

    QFile sigLog(fileName);
    QFileInfo *fileDetails = new QFileInfo(fileName);
    QLineSeries *test_series = new QLineSeries();

    QString sensorID = ui->listWidget->currentItem()->text().split(" ").first();
    QTime sensorTime;
    qreal sensorValue;
    qreal last_sensorValue = 0;
    QString sensorLine;
    QStringList sensorItems;
    QDateTime logStartTime = fileDetails->birthTime();

    QString lookingExpression = QString("^[0-9]*.[0-9]{3} %1 [0-9]*.[0-9]*").arg(sensorID);
    QRegularExpression lookingSensorValue(lookingExpression);
    QRegularExpressionMatch match;


    if(!sigLog.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "No File Found";
    }

    QTextStream stream(&sigLog);

    while (!stream.atEnd()){
        sensorLine = stream.readLine();
        match = lookingSensorValue.match(sensorLine);
        if (match.hasMatch())
        {
            sensorItems = sensorLine.split(" ");

            sensorTime = QTime::fromString(sensorItems[0], "HHmmss.zzz");
            QDateTime sensorTimeComplete = *new QDateTime(logStartTime.date(), sensorTime);
            sensorValue = sensorItems[2].toDouble();


            if (ui->StepGraphCheckBox->isChecked()){
                test_series->append(sensorTimeComplete.toMSecsSinceEpoch(), last_sensorValue);
                //qDebug() << "Hello " << last_sensorValue << " : " << sensorValue;
            }
            test_series->append(sensorTimeComplete.toMSecsSinceEpoch(), sensorValue);
            //qDebug() << sensorTime << " : " << sensorValue;
            last_sensorValue = sensorValue;

        }

    }


    delete fileDetails;
    sigLog.close();

    return test_series;
}

QChart *Widget::createSigLogChart(QString filename) const
{
    QChart *chart = new QChart();
    QLineSeries *test_series = makeDataSeries(filename);


    chart->addSeries(test_series);
    chart->setTitle("Line Graph of " + ui->listWidget->currentItem()->text());

    QDateTimeAxis* axisX = new QDateTimeAxis;
    axisX->setTickCount(30);
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    test_series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis;
    axisY->setLabelFormat("%4.3f");
    axisY->setTitleText("data");
    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);
    test_series->attachAxis(axisY);


    return chart;

}



void Widget::on_makeGraph_clicked()
{
    QMessageBox testbox;
    koldorChartView *chartView;


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
        chartView = new koldorChartView(createSigLogChart(thefilename));
        ui->horizontalLayout->addWidget(chartView, 1);
    }
}


void Widget::on_clearGraph_clicked()
{

    QMessageBox itemsInLayout;
    QLayoutItem* item = ui->horizontalLayout->itemAt(1);
    QWidget* graphWidget = nullptr;

    if (ui->horizontalLayout->count() > 1){
        graphWidget = item->widget();
    }


    if (!graphWidget && ui->horizontalLayout->count() <= 1){
        itemsInLayout.setText("No More Graphs!!");
        itemsInLayout.exec();
    } else if (graphWidget && ui->horizontalLayout->count() == 2) {
        delete graphWidget;
        this->resize(300, 500);
    } else if(graphWidget && ui->horizontalLayout->count() > 2) {
        delete graphWidget;
        this->resize(1200, 500);
    }

}



