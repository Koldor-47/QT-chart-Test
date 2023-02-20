#ifndef KOLDORCHARTVIEW_H
#define KOLDORCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>


QT_USE_NAMESPACE

class koldorChartView : public QChartView
{
public:
    koldorChartView(QChart *chart, QWidget *parent = 0);

protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //void keyPressEvent(QKeyEvent *event); <- This is for key broad support

private:
        bool m_isTouching;
signals:

};

#endif // KOLDORCHARTVIEW_H
