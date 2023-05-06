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
#include <QStringView>
#include <QMap>

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

    sigData = getNicksData(thefilename);

}


QMap<QString, QLineSeries *> Widget::getNicksData(QString &fileName) const
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
    QMap<QString, QLineSeries *> wantedData;

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
        qreal value = sensorLineData[2].toDouble();
        QDateTime time = QDateTime::fromString(sensorLineData[0], "HHmmss.zzz");


        if (!wantedData.contains(sensor_id)) {
            QLineSeries *data = new QLineSeries();

            data->append(time.toMSecsSinceEpoch(), value);
            wantedData.insert(sensor_id, data);
        } else {
            wantedData[sensor_id]->append(time.toMSecsSinceEpoch(), value);

        }
    }

    data.close();

    //the data file has been read and saved in a Qlist
    return wantedData;

}


QChart *Widget::createSigLogChart(QString &filename) const
{
    QChart *chart = new QChart();
    //QMap<QString, QLineSeries *> test = getNicksData(filename);

    qDebug() << filename;

    QDateTimeAxis* axisX = new QDateTimeAxis;
    axisX->setTickCount(30);
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Time");
    qDebug() << "differnet Data Sensors " << sigData.count();

    for (QListWidgetItem* sensor : ui->listWidget->selectedItems()) {
        QString id = sensor->text().split(" ")[0];

        sigData[id]->setName(sensor->text());
        chart->addSeries(sigData[id]);

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

        sigData[id]->attachAxis(axisX);
        sigData[id]->attachAxis(axisY);
    }

    chart->setTitle("Graph of siglog");

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



