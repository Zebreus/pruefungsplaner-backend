#ifndef WEEK_H
#define WEEK_H

class Week;

#include <QObject>
#include <QString>
#include "day.h"

class Week : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<Day*> days READ getDays WRITE setDays NOTIFY daysChanged)
public:
    using SerializableDataObject::SerializableDataObject;
    explicit Week(QObject *parent = nullptr);
    QString name();
    void setName(const QString &name);
    QList<Day*> getDays() const;
    void setDays(QList<Day*> days);

signals:
    void nameChanged();
    void daysChanged(QList<Day*> days);

private:
    QString weekName;
    QList<Day*> days;

    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // WEEK_H
