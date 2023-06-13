#include "widget.h"
#include "ui_widget.h"
#include "lib/qcustomplot.h"


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
#include <QStringView>
#include <QMap>



Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->resize(300, 500);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);


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


    sortedData = getNicksData(thefilename);

    //sigData = getNicksData(thefilename);

}

double Widget::textTimeToSec(QStringView dataTime) const {
    double hour = dataTime.sliced(0,2).toDouble();
    double min = dataTime.sliced(2,2).toDouble();
    double sec = dataTime.sliced(4,2).toDouble();
    double mills = dataTime.sliced(7,3).toDouble() / 1000;

    double totalTime = (((hour * 60) + min) * 60) + sec + mills;

    return totalTime;
}

QMap<QString, QVector<QCPGraphData>> Widget::getNicksData(QString &fileName) const
{
    /*Read Each line
     *
     * Work out if the sensor is one selected
     *
     * if not in map add a new QlineSeries
     *
     * or Append data to existing QlineSeries in Qmap
     *
     *  if Step graph check add and Extra data point in
     */

    bool dataStart = false;
    QFile data(fileName);
    QString line = NULL;
    QMap<QString, QVector<QCPGraphData>> wantedData;


    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No File Found";
    }

    QTextStream dataStream(&data);

    while(!dataStream.atEnd()) {
        line = dataStream.readLine();

        //If have found start of data pass

        if (!dataStart){
            if (line == "HHMMSS.MS  ID VALUE"){
                dataStart = true;
            }
            continue;
        }

        if (line.length() < 14) {
            continue;
        }

        QList<QString> sensorLineData = line.split(" ");
        QString sensor_id  = sensorLineData[1];
        double value = sensorLineData[2].toDouble();
        double dateToSeconds = textTimeToSec(sensorLineData[0]);
        QCPGraphData mynewdata;

        if (!wantedData.contains(sensor_id)) {
            mynewdata.key = dateToSeconds;
            mynewdata.value = value;
            QVector<QCPGraphData> dataStore;
            dataStore.push_back(mynewdata);
            wantedData.insert(sensor_id, dataStore);
        } else {
            mynewdata.key = dateToSeconds;
            mynewdata.value = value;
            wantedData[sensor_id].append(mynewdata);
        }

    }

    data.close();

    //the data file has been read and saved in a Qlist
    return wantedData;

}


QCustomPlot *Widget::createSigLogChart() const
{
    QString chosenThingToLog = ui->listWidget->selectedItems()[0]->text().sliced(0, 2);
    const QString GraphName = ui->listWidget->selectedItems()[0]->text();

    qDebug() << chosenThingToLog;

    QVector<QCPGraphData> dataSelected = sortedData[chosenThingToLog];
    double maxNumber = 1.0;
    double minNumber = 0.0;

    for (QCPGraphData item : dataSelected){
        if (item.value > maxNumber) {
            maxNumber = item.value;
        }

        if (item.value < minNumber) {
            minNumber = item.value;
        }
    }

    QCustomPlot *test = new QCustomPlot();
    QColor color(61,51,189);
    test->addGraph();
    test->graph(0)->data()->set(dataSelected);

    test->graph(0)->setPen(QPen(color));
    test->graph(0)->setName("Nicks Graphing Tool");
    test->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    QSharedPointer<QCPAxisTickerTime> dateTicker(new QCPAxisTickerTime);
    dateTicker->setTimeFormat("%h:%m:%s.%z");
    dateTicker->setTickCount(15);
    test->xAxis->setTicker(dateTicker);

    if (ui->StepGraphCheckBox->isChecked()){
        test->graph(0)->setLineStyle(QCPGraph::lsStepLeft);
    } else {
        test->graph(0)->setLineStyle(QCPGraph::lsLine);
    }

    test->setMouseTracking(true);

    test->replot();
    test->axisRect()->setRangeZoomAxes(test->yAxis, test->xAxis);
    test->axisRect()->setRangeZoomFactor(1, 1.5);

    test->yAxis->setRange(minNumber - 0.01, maxNumber + 0.25);
    test->xAxis->setRange(dataSelected[0].key, dataSelected[(dataSelected.length()-1)].key);

    test->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    test->graph(0)->setName(GraphName);

    return test;

}



void Widget::on_makeGraph_clicked()
{
    QMessageBox testbox;
    QCustomPlot *nicksGraph = createSigLogChart();


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

        //ui->horizontalLayout->addWidget(chartView, 1);
        ui->GraphArea->addWidget(nicksGraph, 1);
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



