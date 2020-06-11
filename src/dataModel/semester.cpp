#include "semester.h"

Semester::Semester(QObject *parent) : SerializableDataObject(parent)
{

}
QString Semester::name()
{
    return semesterName;
}

void Semester::setName(const QString &name)
{
    if (name == semesterName)
        return;

    semesterName = name;
    emit nameChanged();
}

QList<Plan*> Semester::getPlans() const
{
    return this->plans;
}

void Semester::setPlans(QList<Plan*> plans)
{
    if (this->plans == plans)
        return;

    this->plans = plans;
    emit plansChanged(this->plans);
}


void Semester::fromJsonObject(const QJsonObject &content)
{
    simpleValuesFromJsonObject(content);

    QJsonArray plansJsonArray = content.value("plans").toArray();
    plans = fromObjectJsonArray<Plan>(plansJsonArray);
}

QJsonObject Semester::toJsonObject() const
{
    return recursiveToJsonObject();
}
