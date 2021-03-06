#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H

#include "mathparser.h"
#include "operatorinterface.h"
#include "parameterdelegateui.h"

class PythonRunner : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.PythonRunner")
    Q_INTERFACES(OperatorInterface)

public:
    PythonRunner();

    OperatorInterface* createDefaultOperator() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;
private:
    QSharedPointer<ParameterDelegateUi> m_delegate;
};

#endif // PYTHONRUNNER_H
