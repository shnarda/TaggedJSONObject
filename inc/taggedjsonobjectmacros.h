#ifndef TAGGEDJSONOBJECTMACROS_H
#define TAGGEDJSONOBJECTMACROS_H

#include "map.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

#define TAGGEDOBJECTMACRO_DECLARE_MEMBER(type, name) type name;
#define TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_DECLARE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER(type, name) name(val.take(#name), checkValues),
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE(type, name) name(val[#name], checkValues),
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE pair


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
    explicit CLASS_NAME(QJsonObject val, const bool checkValues=true) : MAP(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK, __VA_ARGS__) dummyVar(false) {}; \
    explicit CLASS_NAME(QJsonValue val, const bool checkValues=true) : MAP(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_VALUE_UNPACK, __VA_ARGS__) dummyVar(false) {}; \
    explicit CLASS_NAME(const QByteArray& json, const bool checkValues=true) : CLASS_NAME(TaggedObject::getJSONObjectFromJSONText(json), checkValues) {};\
    explicit CLASS_NAME(const QString& filePath, const bool checkValues=true) : CLASS_NAME(TaggedObject::getJSONObjectFromFile(filePath), checkValues) {};\
    MAP(TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK, __VA_ARGS__)\
private:\
    bool dummyVar;\
};

#endif // TAGGEDJSONOBJECTMACROS_H
