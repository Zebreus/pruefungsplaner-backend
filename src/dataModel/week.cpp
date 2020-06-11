#include "week.h"

Week::Week(QObject *parent) : SerializableDataObject(parent)
{

}

QString Week::name()
{
    return weekName;
}

void Week::setName(const QString &name)
{
    if (name == weekName)
        return;

    weekName = name;
    emit nameChanged();
}

QList<Day*> Week::getDays() const
{
    return this->days;
}

void Week::setDays(QList<Day*> days)
{
    if (this->days == days)
        return;

    this->days = days;
    emit daysChanged(this->days);
}


void Week::fromJsonObject(const QJsonObject &content)
{
    simpleValuesFromJsonObject(content);

    QJsonArray daysJsonArray = content.value("days").toArray();
    days = fromObjectJsonArray<Day>(daysJsonArray);
    QVariant var;
    var.setValue(days);
    createListFromValueAndContentType(daysJsonArray, (QMetaType::Type)var.userType());
}

QJsonObject Week::toJsonObject() const
{
    return recursiveToJsonObject();
}
