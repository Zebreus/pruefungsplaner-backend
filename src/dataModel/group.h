#ifndef CONSTRAINT_H
#define CONSTRAINT_H

class Group;

#include <QObject>
#include <QString>
#include "serializabledataobject.h"

class Group : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool selected READ getSelected WRITE setSelected NOTIFY selectedChanged)
public:
    using SerializableDataObject::SerializableDataObject;
    explicit Group(QObject *parent = nullptr);
    QString name();
    void setName(const QString &name);
    bool getSelected();
    void setSelected(const bool selected);

signals:
    void nameChanged();
    void selectedChanged();

private:
    QString constraintName;
    bool selected;


    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // CONSTRAINT_H
