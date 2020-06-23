#include "fundisplaywidget.h"
#include <QColor>
#include <QPainter>
#include <QImage>
#include "displayhelper.h"

FunDisplayWidget::FunDisplayWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent)
{

}

void FunDisplayWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    QImage image(":/images/odnsm.jpg");
    QImage img = image.scaled(300, 300, Qt::KeepAspectRatio);
    QPainterPath path;
    path.addText(0, 215, QFont("Sans-Serif", 8), "Play around with the rings to change the image");
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if(m_rating == 1){
        img = QImage(":/images/hobbits.jpeg");

        for(int x(1); x < img.width(); x++){
            for(int y (1); y < img.height(); y++){
                QRgb pixcolor = img.pixel(x, y);
                img.setPixel(x, y, qRgba(qRed(pixcolor),qGreen(pixcolor),255, 180));
            }
        }

    }else if(m_rating == 2){
        img = QImage(":/images/hobbits.jpeg");

        for(int x(1); x < img.width(); x++){
            for(int y (1); y < img.height(); y++){
                QRgb pixcolor = img.pixel(x, y);
                img.setPixel(x, y, qRgba(qRed(pixcolor),255,qBlue(pixcolor), 180));
            }
        }
    }else if(m_rating == 0){
        img = QImage(":/images/hobbits.jpeg");

        for(int x(1); x < img.width(); x++){
            for(int y (1); y < img.height(); y++){
                QRgb pixcolor = img.pixel(x, y);
                img.setPixel(x, y, qRgba(255, qGreen(pixcolor),qBlue(pixcolor), 180));
            }
        }
    }
    painter.drawImage(0, 0, img);





}

void FunDisplayWidget::mouseMoveEvent(QMouseEvent *event) {
    sendHoverUpdate(event, 1, 1, 1, 1, QPoint(0, 0));
}
void FunDisplayWidget::setRatings(int ratings) {
    m_rating = ratings;
    this->repaint();
}

