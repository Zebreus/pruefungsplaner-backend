#ifndef SEMESTER_H
#define SEMESTER_H

class Semester;

#include <QObject>
#include <QString>
#include "plan.h"

class Semester : public SerializableDataObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<Plan*> plans READ getPlans WRITE setPlans NOTIFY plansChanged)
public:
    using SerializableDataObject::SerializableDataObject;
    Q_INVOKABLE explicit Semester(QObject *parent = nullptr);
    QString name();
    void setName(const QString &name);

    QList<Plan*> getPlans() const;
    void setPlans(QList<Plan*> plans);

signals:
    void nameChanged();

    void plansChanged(QList<Plan*> plans);

private:
    QString semesterName;

    QList<Plan*> plans;

    // SerializableDataObject interface
public:
    void fromJsonObject(const QJsonObject &content);
    QJsonObject toJsonObject() const;
};

#endif // SEMESTER_H
