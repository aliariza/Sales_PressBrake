#ifndef OPTIONREPOSITORY_H
#define OPTIONREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "optionrecord.h"

class OptionRepository
{
public:
    OptionRepository();

    bool initialize(QString &errorMessage);

    QList<OptionRecord> getAllOptions(QString &errorMessage) const;
    QString getNextOptionNo(QString &errorMessage) const;

    bool addOption(const OptionRecord &option, QString &errorMessage) const;
    bool updateOption(const QString &no, const OptionRecord &updatedOption, QString &errorMessage) const;
    bool deleteOption(const QString &no, QString &errorMessage) const;

private:
    std::optional<OptionRecord> parseOptionLine(const QString &line) const;
    bool validateOption(const OptionRecord &option, QString &errorMessage) const;
    bool codeExists(const QString &code, const QString &excludeNo, QString &errorMessage) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // OPTIONREPOSITORY_H
