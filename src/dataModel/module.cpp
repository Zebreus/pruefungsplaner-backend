#include "module.h"

Module::Module(QObject *parent) : SerializableDataObject(parent)
{

}
QString Module::getName()
{
    return name;
}

void Module::setName(const QString &name)
{
    if (name == this->name)
        return;

    this->name = name;
    emit nameChanged();
}

QString Module::getOrigin()
{
    return origin;
}

void Module::setOrigin(const QString &origin)
{
    if (origin == this->origin)
        return;

    this->origin = origin;
    emit originChanged();
}

QString Module::getNumber()
{
    return number;
}

void Module::setNumber(const QString &number)
{
    if (number == this->number)
        return;

    this->number = number;
    emit numberChanged();
}

bool Module::getActive()
{
    return active;
}

void Module::setActive(const bool active)
{
    if (active == this->active)
        return;

    this->active = active;
    emit activeChanged();
}

QList<Group*> Module::getConstraints(){
    return constraints;
}

void Module::setConstraints(QList<Group*> constraints)
{
    if (this->constraints == constraints)
        return;

    this->constraints = constraints;
    emit constraintsChanged(this->constraints);
}

QList<Group*> Module::getGroups() const
{
    return groups;
}

void Module::setGroups(QList<Group*> groups)
{
    if (this->groups == groups)
        return;

    this->groups = groups;
    emit groupsChanged(this->groups);
}

void Module::removeGroup(Group* group){

    int removed = groups.removeAll(group);
    //int removed = 0;
    if (removed > 0){
        emit groupsChanged(this->groups);
    }
}

void Module::removeConstraint(Group* constraint){

    int removed = constraints.removeAll(constraint);
    //int removed = 0;
    if (removed > 0){
        emit constraintsChanged(this->constraints);
    }
}


void Module::fromJsonObject(const QJsonObject &content)
{
    simpleValuesFromJsonObject(content);
    Plan* activePlan = (Plan*) this->parent();

    groups = fromIdJsonArray<Group>(content.value("groups"), activePlan->getGroups());
    constraints = fromIdJsonArray<Group>(content.value("constraints"), activePlan->getConstraints());
    //fromMetaObjectJsonArray(content.value("groups").toArray(), "List<Group*>");

    //qDebug() << (QMetaType::Type)var.userType() << " " << getTypeFromList((QMetaType::Type)var.userType()) << "=" << QMetaType::typeName(getTypeFromList((QMetaType::Type)var.userType()));

}

QJsonObject Module::toJsonObject() const
{
    return recursiveToJsonObject();
}
