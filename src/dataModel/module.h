#ifndef MODULE_H
#define MODULE_H

class Module;

#include <QObject>
#include <QString>
#include <QList>
#include "group.h"
#include "serializabledataobject.h"
#include "plan.h"
#include <vector>
#include <iostream>

using namespace std;

class Module : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString origin READ getOrigin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(QString number READ getNumber WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(bool active READ getActive WRITE setActive NOTIFY activeChanged STORED false)

    Q_PROPERTY(QList<Group*> constraints READ getConstraints WRITE setConstraints NOTIFY constraintsChanged)
    Q_PROPERTY(QList<Group*> groups READ getGroups WRITE setGroups NOTIFY groupsChanged)

public:
    using SerializableDataObject::SerializableDataObject;
    explicit Module(QObject *parent = nullptr);
    QString getName();
    void setName(const QString &name);
    QString getOrigin();
    void setOrigin(const QString &origin);
    QString getNumber();
    void setNumber(const QString &number);
    bool getActive();
    void setActive(const bool active);
    QList<Group*> getConstraints();
    QList<Group*> getGroups() const;
    void setConstraints(QList<Group*> constraints);
    void setGroups(QList<Group*> groups);


signals:
    void nameChanged();
    void originChanged();
    void numberChanged();
    void activeChanged();
    void constraintsChanged(QList<Group*> groups);
    void groupsChanged(QList<Group*> groups);

public slots:
    void removeGroup(Group* group);
    void removeConstraint(Group* constraint);
public:
    QString name;
    QString origin;
    QString number;
    bool active;
    QList<Group*> constraints;
    QList<Group*> groups;

    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // MODULE_H
