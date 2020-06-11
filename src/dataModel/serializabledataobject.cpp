#include "serializabledataobject.h"
#include "group.h"
#include "day.h"

int SerializableDataObject::idc = 0;

void SerializableDataObject::simpleValuesFromJsonObject(const QJsonObject &content)
{
    auto metaObject = this->metaObject();
    for(QString key: content.keys()){
        const char* keyChars = key.toStdString().c_str();
        //Find index of property
        int propIndex = metaObject->indexOfProperty(keyChars);
        if ( propIndex < 0 ){
            continue;
        }

        QJsonValue jsonValue = content.value(key);
        if (jsonValue.isBool() || jsonValue.isDouble() || jsonValue.isString()) {
            QVariant variant = this->property(keyChars);
            this->setProperty(key.toLatin1().data(), jsonValue.toVariant());
        }

    }
}

QJsonObject SerializableDataObject::recursiveToJsonObject() const
{
    QJsonObject jsonObject;
    auto metaObject = this->metaObject();
    //TODO metaobject->superclass auch für weitere vererbung
    for(int i = metaObject->superClass()->propertyOffset()-1; i < metaObject->propertyCount(); ++i) {
        QMetaProperty prop = metaObject->property(i);
        QVariant variant = property(prop.name());

        if ( variant.canConvert<SerializableDataObject *>() ) {
            SerializableDataObject* sub_obj = variant.value<SerializableDataObject*>();
            jsonObject.insert( prop.name(), sub_obj->toJsonObject() );

        }else if ( variant.canConvert<QList<QVariant>>() ) {
            jsonObject.insert( prop.name(), toJsonArray(variant.value<QList<QVariant>>()) );

        } else{
            jsonObject.insert(prop.name(), QJsonValue::fromVariant(variant));
        }
    }

    return jsonObject;
}

QJsonArray SerializableDataObject::toJsonArray(const QList<SerializableDataObject *> &list) const
{
    if(list.size() > 0){
        if(list.front()->parent() == this){
            return toObjectJsonArray(list);
        }else{
            return toIdJsonArray(list);
        }
    }else{
        return QJsonArray();
    }
}

QJsonArray SerializableDataObject::toJsonArray(const QList<QVariant> &list) const
{
    if(list.size() > 0){
        QVariant firstEntry = list.front();

        //If QList contains SerializableDataObjects
        if(firstEntry.canConvert<SerializableDataObject*>()){
            //TODO Continue looking for a nicer way to convert QList<QVariant> to QList<SerializableDataObject>
            QList<SerializableDataObject*> sdoPointerList;
            for(QVariant variant: list){
                sdoPointerList.push_back(variant.value<SerializableDataObject*>());
            }
            return toJsonArray(sdoPointerList);
        }

        //Use default JsonValue fromVariant if no SerializableDataObject
        //TODO check other types, that cannot be converted directly
        QJsonArray returnArray;
        for(QVariant variant: list){
            returnArray.append(QJsonValue::fromVariant(variant));
        }
        return returnArray;
    }
    return QJsonArray();
}

QJsonArray SerializableDataObject::toObjectJsonArray(const QList<SerializableDataObject *> &list) const
{
    QJsonArray activeGroupArray;
    for(SerializableDataObject* sdo: list){
        activeGroupArray.push_back( sdo->toJsonObject() );
    }
    return activeGroupArray;
}

QJsonArray SerializableDataObject::toIdJsonArray(const QList<SerializableDataObject *> &list) const
{
    QJsonArray activeGroupArray;
    for(SerializableDataObject* sdo: list){
        activeGroupArray.push_back(sdo->id);
    }
    return activeGroupArray;
}

bool SerializableDataObject::setPropertyValue(const QJsonValue &value, const QString &propertyName)
{
    auto metaObject = this->metaObject();
    const char* propertyNameChars = propertyName.toStdString().c_str();
    int propertyIndex = metaObject->indexOfProperty(propertyNameChars);

    //Return false if property does not exist
    if ( propertyIndex < 0 ){
        return false;
    }

    QMetaProperty property = metaObject->property(propertyIndex);
    //TODO QVariant::Type not completly equal to QMetaType::Type. Check the differences and account for them later
    QMetaType::Type propertyType = (QMetaType::Type) property.type();
    QJsonValue::Type jsonType = value.type();

    switch(jsonType){
    case QJsonValue::Null:
    case QJsonValue::Bool:
    case QJsonValue::Double:
    case QJsonValue::String:
        return this->setProperty(propertyNameChars, value.toVariant());
        break;
    case QJsonValue::Object:

        break;
    case QJsonValue::Array:
        break;
    case QJsonValue::Undefined:
        return false;
    }

    return false;
}

QMetaType::Type SerializableDataObject::getTypeFromList(const QMetaType::Type &listType) const
{
    const char* listTypeName = QMetaType::typeName(listType);
    const char* contentStart = strchr(listTypeName, '<') + 1;
    const char* contentEnd = strrchr(listTypeName, '>');
    size_t contentLength = contentEnd-contentStart;
    if(contentStart==nullptr || contentEnd==nullptr || contentLength<=0){
        return QMetaType::UnknownType;
    }

    char *contentTypeName = new char[contentLength+1]();
    memcpy(contentTypeName, contentStart, contentLength);

    return (QMetaType::Type) QMetaType::type(contentTypeName);
}

QVariant SerializableDataObject::createListFromValueAndListType(const QJsonArray &value, const QMetaType::Type &listType)
{
    QMetaType::Type contentType = getTypeFromList(listType);
    return createListFromValueAndContentType(value, contentType);
}

QVariant SerializableDataObject::createListFromValueAndContentType(const QJsonArray &value, const QMetaType::Type &listType)
{
    QMetaType::Type contentType = getTypeFromList(listType);
    if(contentType == QMetaType::UnknownType){
        return QList<QVariant>();
    }

    const char* a = QMetaType::typeName(listType);
    const char* b = QMetaType::typeName(contentType);

    QMetaType::TypeFlags flags = QMetaType::typeFlags(contentType);
    if((flags & QMetaType::PointerToQObject) != 0){
        qDebug() << "Is pointer to QObject";
        QList<QObject*> resultList;
        const QMetaObject* metaObject = QMetaType::metaObjectForType(contentType);
        for(QJsonValue arrayEntry: value){
            QObject* qObject = metaObject->newInstance(Q_ARG(QObject*, this));
            resultList.push_back(qObject);

            //Test if object can be cast to a SerializableDataObject* and initialize if possible
            SerializableDataObject* sdo = dynamic_cast<SerializableDataObject*>(qObject);
            if(sdo != nullptr){
                sdo->fromJsonObject(arrayEntry.toObject());
            }
        }

        QVariant var((QVariant::Type)listType);
        QVariant convar;
        convar.setValue(new Day(this));
        var.toList().push_back(convar);
        //var.setValue(resultList);
        bool d = var.canConvert(listType);
        bool e = var.canConvert(QMetaType::type("QList<QObject*>"));
        bool f = var.canConvert(QMetaType::type("QList<SerializableDataObject*>"));
        bool g = var.canConvert(QMetaType::type("QList<QVariant>"));
        bool h = var.canConvert(QMetaType::type("QList<Day*>"));
        var.convert(listType);
        return var;
    }else{
        qDebug() << "Is no pointer to QObject";
    }



    return QList<QVariant>();
}

int SerializableDataObject::getId()
{
    return id;
}

SerializableDataObject::SerializableDataObject(QObject *parent, int id) : QObject(parent), id(id)
{
}

/*
QList<SerializableDataObject *> SerializableDataObject::fromObjectJsonArray(const QJsonArray& content, const QString& name)
{
    auto metaObject = this->metaObject();
    const char* keyChars = name.toStdString().c_str();
    //Find index of property
    int propIndex = metaObject->indexOfProperty(keyChars);
    QMetaProperty mp = metaObject->property(propIndex);
    QString typen = mp.name();
    const char* typeChars = typen.toStdString().c_str();
    int metaTypeId1 = QMetaType::type(typeChars);
    int metaTypeId2 = QMetaType::type("Semester");
    int metaTypeId3 = QMetaType::type("Semester*");
    const QMetaObject* mo3 = QMetaType::metaObjectForType(metaTypeId3);
    const SerializableDataObject* sdo3 = (SerializableDataObject*)mo3->newInstance(Q_ARG(QObject*, this));
    const SerializableDataObject* sdo4 = (SerializableDataObject*)mo3->newInstance();
    //  const SerializableDataObject* sdo3 = mo3->newInstance(Q_ARG(QMetaType::QObjectStar, this),Q_ARG(QMetaType::Int,45));
    auto myClassPtr = (SerializableDataObject*)QMetaType::create(metaTypeId3);

    QList<SerializableDataObject *> resultList;
    return resultList;
}
*/