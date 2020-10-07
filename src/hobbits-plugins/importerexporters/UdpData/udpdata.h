#ifndef UDPDATA_H
#define UDPDATA_H

#include "importexportinterface.h"

class UdpData : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.UdpData")
    Q_INTERFACES(ImporterExporterInterface)

public:
    UdpData();
    ~UdpData() override;

    ImporterExporterInterface* createDefaultImporterExporter() override;

    QString name() override;

    bool canExport() override;
    bool canImport() override;

    QString getImportLabelForState(QJsonObject pluginState) override;
    QString getExportLabelForState(QJsonObject pluginState) override;

    QSharedPointer<ImportResult> importBits(QJsonObject pluginState) override;
    QSharedPointer<ExportResult> exportBits(
            QSharedPointer<const BitContainer> container,
            QJsonObject pluginState) override;

private:
};

#endif // UDPDATA_H
