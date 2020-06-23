#ifndef FUNDISPLAYWIDGET_H
#define FUNDISPLAYWIDGET_H

#include "displaybase.h"

class FunDisplayWidget : public DisplayBase
{
    Q_OBJECT

public:
    FunDisplayWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;
public slots:
    void setRatings(int ratings);
    private:
    int m_rating;
};

#endif // FUNDISPLAYWIDGET_H
