#include "fundisplay.h"

FunDisplay::FunDisplay() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* FunDisplay::createDefaultDisplay()
{
    return new FunDisplay();
}

QString FunDisplay::getName()
{
    return "FunDisplay";
}

QWidget* FunDisplay::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* FunDisplay::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void FunDisplay::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new FunDisplayWidget(displayHandle, this);
        m_controlsWidget = new FunDisplayControls();

        // make necessary connections here
        connect(m_controlsWidget, SIGNAL(ratingsChanged(int)), m_displayWidget, SLOT(setRatings(int)));
    }
}
