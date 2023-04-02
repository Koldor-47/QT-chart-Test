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

    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

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


void Widget::getNicksData(QString &fileName, QList<QLineSeries *> &datalines) const
{
    QList<QLineSeries *> nicksData;
    QList<QString> fileData;
    QList<QListWidgetItem *> signalNames = ui->listWidget->selectedItems();
    QString logStart = fileName.split("_")[1].split(".")[0];
    QDateTime logStartTime = QDateTime::fromString(logStart, "HHmmss");



    QFile data(fileName);

    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No File Found";
    }

    QTextStream dataStream(&data);

    while(!dataStream.atEnd()) {
        QString line = dataStream.readLine();
        fileData.append(line);
    }

    data.close();

    //the data file has been read and saved in a Qlist

    for (auto signal : signalNames){
        QLineSeries *dataLine = new QLineSeries;
        QList<QString> sensorItems = signal->text().split(" ");
        QString sensorID = sensorItems[0];
        QString lookingExpression = QString("^[0-9]*.[0-9]{3} %1 [0-9]*.[0-9]*").arg(sensorID);
        QRegularExpression lookingSensorValue(lookingExpression);
        QRegularExpressionMatch match;
        qreal lastSensorValue = 0;

        for (QString line : fileData){
            if (lookingSensorValue.match(line).hasMatch()){
                QList<QString> sensorLineData = line.split(" ");
                QDateTime time = QDateTime::fromString(sensorLineData[0], "HHmmss.zzz");
                qreal value = sensorLineData[2].toDouble();

                if (ui->StepGraphCheckBox->isChecked()){
                    dataLine->append(time.toMSecsSinceEpoch(), lastSensorValue);
                }

                dataLine->append(time.toMSecsSinceEpoch(), value);
                lastSensorValue = value;
            }

        }

        datalines.append(dataLine);
    }

}


QChart *Widget::createSigLogChart(QString filename) const
{
    QChart *chart = new QChart();
    //QLineSeries *test_series = makeDataSeries(filename);
    QList<QLineSeries *> test;
    getNicksData(filename, test);

    QDateTimeAxis* axisX = new QDateTimeAxis;
    axisX->setTickCount(30);
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Time");




    qDebug() << "hello " << test.count();
    //chart->addSeries(test_series);
    for (int i = 0; i < test.count(); i++){
        qDebug() << i << test[i];
        QString signal_name = ui->listWidget->selectedItems()[i]->text().split(" ")[1];
        QLineSeries *theLine = test[i];
        theLine->setName(signal_name);
        chart->addSeries(theLine);




        QValueAxis* axisY = new QValueAxis;
        if (ui->StepGraphCheckBox->isChecked()){
            axisY->setLabelFormat("%d");
            axisY->setTickCount(10);

        } else {
            axisY->setLabelFormat("%4.3f");
            axisY->setTickCount(10);
        }
        axisY->setTitleText("data");

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);

        theLine->attachAxis(axisX);
        theLine->attachAxis(axisY);


    }

    chart->setTitle("Line Graph of Things");

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
        //ui->horizontalLayout->addWidget(chartView, 1);
        ui->GraphArea->addWidget(chartView, 1);
    }
}


void Widget::on_clearGraph_clicked()
{

    QMessageBox itemsInLayout;
    QLayoutItem* item = ui->GraphArea->itemAt(0);
    QWidget* graphWidget = nullptr;

    if (ui->GraphArea->count() >= 1){
        graphWidget = item->widget();
    }


    if (!graphWidget && ui->GraphArea->count() == 0){
        itemsInLayout.setText("No More Graphs!!");
        itemsInLayout.exec();
    } else if (graphWidget && ui->GraphArea->count() <= 1) {
        delete graphWidget;
        this->resize(10, 500);
    } else if(graphWidget && ui->GraphArea->count() >= 2) {
        delete graphWidget;
        //this->resize(1200, 500);
    }

}



