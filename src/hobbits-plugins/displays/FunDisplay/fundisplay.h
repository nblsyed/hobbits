#ifndef FUNDISPLAY_H
#define FUNDISPLAY_H

#include "displayinterface.h"
#include "fundisplaycontrols.h"
#include "fundisplaywidget.h"

class FunDisplay : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.FunDisplay")
    Q_INTERFACES(DisplayInterface)

public:
    FunDisplay();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    FunDisplayWidget* m_displayWidget;
    FunDisplayControls* m_controlsWidget;
};

#endif // FUNDISPLAY_H
