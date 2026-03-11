#ifndef TOOLINGREPOSITORY_H
#define TOOLINGREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "toolingrecord.h"

class ToolingRepository
{
public:
    ToolingRepository();

    bool initialize(QString &errorMessage);

    QList<ToolingRecord> getAllTooling(QString &errorMessage) const;
    QString getNextToolingNo(QString &errorMessage) const;

    bool addTooling(const ToolingRecord &tooling, QString &errorMessage) const;
    bool updateTooling(const QString &no, const ToolingRecord &updatedTooling, QString &errorMessage) const;
    bool deleteTooling(const QString &no, QString &errorMessage) const;

private:
    std::optional<ToolingRecord> parseToolingLine(const QString &line) const;
    bool validateTooling(const ToolingRecord &tooling, QString &errorMessage) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // TOOLINGREPOSITORY_H
