#ifndef PLAYFULPLUGIN_H
#define PLAYFULPLUGIN_H

#include "operatorinterface.h"

namespace Ui
{
class PlayfulPlugin;

}

class PlayfulPlugin : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.4.PlayfulPlugin")
    Q_INTERFACES(OperatorInterface)

public:
    PlayfulPlugin();

    OperatorInterface* createDefaultOperator() override;
    QString getName() override;

    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject& pluginState) override;
    bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer> > inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;
    void previewBits(QSharedPointer<BitContainerPreview> container) override;

private:
    Ui::PlayfulPlugin *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // PLAYFULPLUGIN_H
