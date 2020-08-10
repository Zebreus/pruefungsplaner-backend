#ifndef QTJSONTRAITS_H
#define QTJSONTRAITS_H

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "jwt-cpp/jwt.h"

struct QtJsonTraits {
    // Type Specifications
    using value_type = QJsonValue; // The generic "value type" implementation, most libraries have one
    using object_type = QJsonObject; // The "map type" string to value
    using array_type = QJsonArray; // The "list type" array of values
    using string_type = QString; // The "list of chars", must be a narrow char
    using number_type = double; // The "percision type"
    using integer_type = int; // The "integral type"
    using boolean_type = bool; // The "boolean type"

    // Translation between the implementation notion of type, to the jwt::json::type equivilant
    static jwt::json::type get_type(const value_type &val) {
        using jwt::json::type;
        switch(val.type()){
        case QJsonValue::Type::Bool:
            return type::boolean;
        case QJsonValue::Type::Double:
            return type::number;
        case QJsonValue::Type::String:
            return type::string;
        case QJsonValue::Type::Array:
            return type::array;
        case QJsonValue::Type::Object:
            return type::object;
        case QJsonValue::Type::Null:
        case QJsonValue::Type::Undefined:
            break;
//          return type::boolean;
        }

        throw std::logic_error("invalid type");
    }

    // Conversion from generic value to specific type
    static object_type as_object(const value_type &val){
        if(val.isObject()){
            return val.toObject();
        }else{
            throw std::logic_error("invalid type");
        }
    }
    static array_type as_array(const value_type &val){
        if(val.isArray()){
            return val.toArray();
        }else{
            throw std::logic_error("invalid type");
        }
    }
    static string_type as_string(const value_type &val){
        if(val.isString()){
            return val.toString();
        }else{
            throw std::logic_error("invalid type");
        }
    }
    static number_type as_number(const value_type &val){
        if(val.isDouble()){
            return val.toDouble();
        }else{
            throw std::logic_error("invalid type");
        }
    }
    static integer_type as_int(const value_type &val){
        if(val.isDouble()){
            return val.toInt();
        }else{
            throw std::logic_error("invalid type");
        }
    }
    static boolean_type as_bool(const value_type &val){
        if(val.isBool()){
            return val.toBool();
        }else{
            throw std::logic_error("invalid type");
        }
    }

    // serilization and parsing
    static bool parse(value_type &val, string_type str){
        QJsonValue result = QJsonValue(QJsonValue::Type::Undefined);;
        QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());

        if(document.isObject()){
            result = document.object();
        }else if(document.isArray()){
            result = document.array();
        }else if(document.isEmpty()){
            result = QJsonValue(QJsonValue::Type::Undefined);
        }else{
            QString arrayString = QString("[%1]").arg(str);
            document = QJsonDocument::fromJson(arrayString.toUtf8());
            if(document.isArray() && document.array().size() == 1){
                QJsonValue value = document.array()[0];
                if(value.isString() || value.isDouble() || value.isBool() ){
                    result = value;
                }
            }
        }

        val.swap(result);
        return !val.isUndefined();
    }

    static string_type serialize(const value_type &val){
        QString result = "";

        switch(val.type()){
        case QJsonValue::Type::Bool:
            if(val.toBool()){
                result = "true";
            }else{
                result = "false";
            }
            break;
        case QJsonValue::Type::Double:
            result = QString::number(val.toDouble());
            break;
        case QJsonValue::Type::String :
            result = val.toString();
            break;
        case QJsonValue::Type::Array :
            {
                QJsonDocument arrayDocument(val.toArray());
                result = arrayDocument.toJson(QJsonDocument::Compact);
            }
            break;
        case QJsonValue::Type::Object :
            {
                QJsonDocument objectDocument(val.toObject());
                result = objectDocument.toJson(QJsonDocument::Compact);
            }
            break;
        case QJsonValue::Type::Null:
        case QJsonValue::Type::Undefined:
            result = "";
            break;
        }

        return result;
    }

    //Functions for json objects
    static int object_count(const object_type& object, const string_type& key) {
        return (int)object.contains(key);
    }

    static void object_for_each(const object_type& object, std::function<void(const string_type&, const value_type&)> function) {
        for(QJsonObject::const_iterator value = object.begin(); value!=object.end(); value++){
            function(value.key(), value.value());
        }
    }

    //Functions for json strings
    static std::string string_to_std(const string_type& string) {
        return std::string(string.toLatin1().constData());
    }

    static string_type string_from_std(const std::string& string) {
        return QString::fromLatin1(string.data(), string.size());
    }

    template<typename Value>
    class ValueCreator{
    public:
        ValueCreator(const Value& value);
        QJsonValue get();
    private:
        QJsonValue jsonValue;
    };

    template<typename Iterator>
    static const array_type array_construct(Iterator begin, Iterator end){
        QJsonArray array;
        for(auto value = begin; value!=end; value++){
            auto realValue = *value;
            array.append(ValueCreator<decltype(realValue)>(realValue).get());
        }
        return array;
    }

    static void array_for_each(const array_type& array, std::function<void(const value_type&)> function) {
        for(const value_type& value : array){
            function(value);
        }
    }
};

#endif // QTJSONTRAITS_H
