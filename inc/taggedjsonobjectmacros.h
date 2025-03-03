#ifndef TAGGEDJSONOBJECTMACROS_H
#define TAGGEDJSONOBJECTMACROS_H

#include "map.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

#define TAGGEDOBJECTMACRO_PREPARE_JSON_OBJECT(type, name) ret[#name] = name.toJsonValue();
#define TAGGEDOBJECTMACRO_PREPARE_JSON_OBJECT_UNPACK(pair) TAGGEDOBJECTMACRO_PREPARE_JSON_OBJECT pair

#define TAGGEDOBJECTMACRO_DECLARE_MEMBER(type, name) type name;
#define TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_DECLARE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER(type, name) name(obj[#name], checkValues)
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE(type, name) name(val[#name], checkValues)
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE pair

#define TAGGEDOBJECTMACRO_LIST_MEMBERS(type, name) type name
#define TAGGEDOBJECTMACRO_LIST_MEMBERS_UNPACK(pair) TAGGEDOBJECTMACRO_LIST_MEMBERS pair

#define TAGGEDOBJECTMACRO_MOVE_PARAMETERS(type, name) name(std::move(name))
#define TAGGEDOBJECTMACRO_MOVE_PARAMETERS_UNPACK(pair) TAGGEDOBJECTMACRO_MOVE_PARAMETERS pair


namespace TaggedObject {
    inline QJsonObject getJSONObjectFromJSONText(const QByteArray& json)
    {
        return QJsonDocument::fromJson(json).object();
    }
    inline QJsonObject getJSONObjectFromFile(const QString& filePath)
    {
        QFile f{ filePath };
        f.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(f.readAll()).object();
    }
};

/*!
* @brief This macro builds a specialized class that is used for encapsulating JSON objects, which in this documentation as tagged object.
* 
* Name of the class to be defined is set by the argument \a CLASS_NAME and the members of the class are defined by adding:\n
(<any instance of TaggedJSONObject or TaggedJSONArray that matches the target JSON object's type>, <name of the member, which has to be the same as the name of the target JSON field>)\n
Example : TJO_DEFINE_JSON_TAGGED_OBJECT(MyObj, (TaggedJSONString, name), (TaggedJSONInt, age)), defines a class named MyObj, which can hold "name" and "age" fields.\n
Defined object can be constructed either by a QJSONObject, a QJsonValue, a QByteArray that represents the JSON data or a QString that holds the path of the target JSON file. Defined 
object also have a optional checkValues parameter for each constructor, which indicated if there can be a missing field on any of the members. If any of the members don't have a 
respective JSON data, a runtime error will be raised.
*/
#define TJO_DEFINE_JSON_TAGGED_OBJECT(CLASS_NAME, ...) \
class CLASS_NAME{\
public:\
    explicit CLASS_NAME() {}\
    explicit CLASS_NAME(const QJsonObject& obj, const bool checkValues=true) : MAP_LIST(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK, __VA_ARGS__) {}; \
    explicit CLASS_NAME(const QJsonValue& val, const bool checkValues=true) : MAP_LIST(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE_UNPACK, __VA_ARGS__) {}; \
    explicit CLASS_NAME(const QByteArray& json, const bool checkValues=true) : CLASS_NAME(TaggedObject::getJSONObjectFromJSONText(json), checkValues) {};\
    explicit CLASS_NAME(const QString& filePath, const bool checkValues=true) : CLASS_NAME(TaggedObject::getJSONObjectFromFile(filePath), checkValues) {};\
    explicit CLASS_NAME(MAP_LIST(TAGGEDOBJECTMACRO_LIST_MEMBERS_UNPACK, __VA_ARGS__), const bool checkValues=true) : MAP_LIST(TAGGEDOBJECTMACRO_MOVE_PARAMETERS_UNPACK, __VA_ARGS__) {};\
    QJsonObject toJsonObject() const\
    {\
        QJsonObject ret;\
        MAP(TAGGEDOBJECTMACRO_PREPARE_JSON_OBJECT_UNPACK, __VA_ARGS__)\
        return ret;\
    }\
    QJsonValue toJsonValue() const { return toJsonObject(); }\
    MAP(TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK, __VA_ARGS__)\
};

#endif // TAGGEDJSONOBJECTMACROS_H
