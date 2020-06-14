#include "playfulplugin.h"
#include "ui_playfulplugin.h"
#include <QObject>

#include <QTime>
PlayfulPlugin::PlayfulPlugin() :
    ui(new Ui::PlayfulPlugin())
{

}

OperatorInterface* PlayfulPlugin::createDefaultOperator()
{
    return new PlayfulPlugin();
}

//Return name of operator
QString PlayfulPlugin::getName()
{
    return "PlayfulPlugin";
}

void PlayfulPlugin::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void PlayfulPlugin::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
}

bool PlayfulPlugin::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
        return false;
    }
    if(!pluginState.contains("message") || !pluginState.value("message").isString()){
        return false;
    }
    return true;
}

bool PlayfulPlugin::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    // Set the UI fields based on the plugin state
    ui->lineEdit->setText(pluginState.value("message").toString());
    return true;
}

QJsonObject PlayfulPlugin::getStateFromUi()
{
    QJsonObject pluginState;

    //Pull data from the input fields and input them into pluginState
    pluginState.insert("message", ui->lineEdit->text());
    return pluginState;
}

int PlayfulPlugin::getMinInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

int PlayfulPlugin::getMaxInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

QSharedPointer<const OperatorResult> PlayfulPlugin::operateOnContainers(
        QList<QSharedPointer<const BitContainer> > inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QSharedPointer<OperatorResult> result(new OperatorResult());
    //Perform bit operations here
    if(!canRecallPluginState(recallablePluginState)){
        return result;
    }
    QString message = recallablePluginState.value("message").toString();



    //A little program that besmirches the insides of words

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    if(message.isEmpty() || message.length()<3){
        message = "Word here word there word everywhere word nowhere";
    }

    bool p = true;
    for(int i = 0; i < message.length()-1; i++){
        if(message[i+1] == " " || p==true){
            p = false;
        }else if(message[i] == " "){
            p = true;
        }else{

            int r = qrand() % ((5 + 1) - 1) + 1;
            if(r==1){message.replace(i, 1, 'a');}else if(r==2){message.replace(i, 1, 'e');}else if(r==3){message.replace(i, 1, 'i');}else if(r==4){message.replace(i, 1, 'o');}else if(r==5){message.replace(i, 1, 'u');}


        }
    }






    QSharedPointer<BitContainer> outputContainer = QSharedPointer<BitContainer>(new BitContainer());
    outputContainer->setBits(message.toLatin1());

    result->setOutputContainers({outputContainer});
    result->setPluginState({recallablePluginState});

    return result;
}

void PlayfulPlugin::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
    // optionally use the current container to prepare the UI or something
}
