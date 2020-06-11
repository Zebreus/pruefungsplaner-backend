#ifndef PLAN_H
#define PLAN_H

class Plan;

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <iostream>
#include "module.h"
#include "group.h"
#include "week.h"

using namespace std;

class Plan : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<Group*> constraints READ getConstraints WRITE setConstraints NOTIFY constraintsChanged)
    Q_PROPERTY(QList<Group*> groups READ getGroups WRITE setGroups NOTIFY groupsChanged)
    Q_PROPERTY(QList<Week*> weeks READ getWeeks WRITE setWeeks NOTIFY weeksChanged)
    Q_PROPERTY(QList<Module*> modules READ getModules NOTIFY modulesChanged)

public:
    using SerializableDataObject::SerializableDataObject;
    explicit Plan(QObject *parent = nullptr);
    QString name();
    QList<Group*> getConstraints();
    QList<Group*> getGroups() const;
    void setConstraints(QList<Group*> constraints);
    void setGroups(QList<Group*> groups);
    QList<Module*> getModules();
    void setName(const QString &name);
    QList<Week*> getWeeks() const;
    void setWeeks(QList<Week*> weeks);
    Q_INVOKABLE void addNewGroup(QString name);
    Q_INVOKABLE void removeGroup(Group* gp);
    Q_INVOKABLE void addNewConstraint(QString name);
    Q_INVOKABLE void removeConstraint(Group* gp);

signals:
    void nameChanged();
    void modulesChanged();
    void constraintsChanged(QList<Group*> groups);
    void groupsChanged(QList<Group*> groups);
    void weeksChanged(QList<Week*> weeks);

public:
    QString planName;
    QList<Group*> constraints;
    QList<Group*> groups;
    QList<Module*> modules;
    QList<Week*> weeks;


    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // PLAN_H
