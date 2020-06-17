#ifndef TIMESLOT_H
#define TIMESLOT_H

class Timeslot;

#include <QObject>
#include <QString>
#include "serializabledataobject.h"
#include "module.h"
#include "group.h"
#include "plan.h"

class Timeslot : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<Module*> modules READ getModules WRITE setModules NOTIFY modulesChanged)
    Q_PROPERTY(QList<Group*> activeGroups READ getActiveGroups WRITE setActiveGroups NOTIFY activeGroupsChanged)
public:
    using SerializableDataObject::SerializableDataObject;
    explicit Timeslot(QObject *parent = nullptr);
    QString name();
    void setName(const QString &name);
    QList<Module*> getModules() const;
    QList<Group*> getActiveGroups() const;
    void setModules(QList<Module*> modules);
    void setActiveGroups(QList<Group*> activeGroups);
    Q_INVOKABLE bool containsActiveGroup(Group* gp);
    Q_INVOKABLE void addActiveGroup(Group* gp);
    Q_INVOKABLE void removeActiveGroup(Group* gp);
    Q_INVOKABLE bool containsModule(Module* gp);
    Q_INVOKABLE void addModule(Module* gp);
    Q_INVOKABLE void removeModule(Module* gp);

signals:
    void nameChanged();
    void modulesChanged(QList<Module*> modules);
    void activeGroupsChanged(QList<Group*> activeGroups);

private:
    QString timeslotName;

    QList<Module*> modules;
    QList<Group*> activeGroups;

    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // TIMESLOT_H
