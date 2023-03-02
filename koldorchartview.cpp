#include <QApplication>

#include "koldorchartview.h"


#include <iostream>

koldorChartView::koldorChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

bool koldorChartView::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::TouchBegin){
        m_isTouching = true;

        chart()->setAnimationOptions(QChart::NoAnimation);
    }

    return QChartView::viewportEvent(event);
};

void koldorChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton){
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
        m_lastMousePos = event->pos();
        event->accept();
    }
    if (m_isTouching) {
        return;
    }
    QChartView::mousePressEvent(event);
};

void koldorChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MiddleButton){
        auto dPos = event->pos() - m_lastMousePos;
        chart()->scroll(-dPos.x(), dPos.y());

        m_lastMousePos = event->pos();

        QApplication::restoreOverrideCursor();
    }

    if (m_isTouching )
        return;
    QChartView::mouseMoveEvent(event);
};

void koldorChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_isTouching)
        m_isTouching = false;

    chart()->setAnimationOptions(QChart::SeriesAnimations);

    QChartView::mouseReleaseEvent(event);
};
