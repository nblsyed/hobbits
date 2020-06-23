#ifndef FUNDISPLAYCONTROLS_H
#define FUNDISPLAYCONTROLS_H

#include <QWidget>

namespace Ui
{
class FunDisplayControls;
}

class FunDisplayControls : public QWidget
{
    Q_OBJECT

public:
    FunDisplayControls();
signals:
    void ratingsChanged(int);

private:
    Ui::FunDisplayControls *ui;
};

#endif // FUNDISPLAYCONTROLS_H
