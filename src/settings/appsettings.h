#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <string>
#include <QJsonObject>


class AppSettings
{
public:
    AppSettings();

    QJsonObject generateJsonObject();
    bool updateFromJson();

    bool m_loadDefaultProfilesAtStartup;
    bool m_caseSensitiveScriptParsing;
    std::string m_customSpawnCmd;
};

#endif // APPSETTINGS_H
