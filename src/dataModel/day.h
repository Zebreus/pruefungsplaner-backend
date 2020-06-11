#ifndef DAY_H
#define DAY_H

class Day;

#include <QObject>
#include <QString>
#include "timeslot.h"
#include "serializabledataobject.h"

class Day : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<Timeslot*> timeslots READ getTimeslots WRITE setTimeslots NOTIFY timeslotsChanged)
public:
    Q_INVOKABLE explicit Day(QObject *parent = nullptr);
    using SerializableDataObject::SerializableDataObject;

    QString name();
    void setName(const QString &name);
    QList<Timeslot*> getTimeslots() const;
    void setTimeslots(QList<Timeslot*> timeslots);

signals:
    void nameChanged();

    void timeslotsChanged(QList<Timeslot*> timeslots);

private:
    QString dayName;

    QList<Timeslot*> timeslots;

    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // DAY_H
