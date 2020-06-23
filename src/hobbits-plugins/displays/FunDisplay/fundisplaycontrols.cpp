#include "fundisplaycontrols.h"
#include "ui_fundisplaycontrols.h"


FunDisplayControls::FunDisplayControls() :
    ui(new Ui::FunDisplayControls())
{
    ui->setupUi(this);
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(ratingsChanged(int)));
}
