#ifndef TAGGEDJSONOBJECTMACROS_H
#define TAGGEDJSONOBJECTMACROS_H

#include "map.h"
#include <QFile>

#define TAGGEDOBJECTMACRO_DECLARE_MEMBER(type, name) type name;
#define TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_DECLARE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER(type, name) name(val[#name], checkValues),
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER pair

#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_MOVE(type, name) name(val.take(#name), checkValues),
#define TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_MOVE_UNPACK(pair) TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_MOVE pair

#define TAGGEDOBJECTMACRO_SET_MEMBER(type, name) name = type(val[#name], checkValues);
#define TAGGEDOBJECTMACRO_SET_MEMBER_UNPACK(pair) TAGGEDOBJECTMACRO_SET_MEMBER pair

#define TAGGEDOBJECTMACRO_SET_MEMBER_MOVE(type, name) name = type(val.take(#name), checkValues);
#define TAGGEDOBJECTMACRO_SET_MEMBER_MOVE_UNPACK(pair) TAGGEDOBJECTMACRO_SET_MEMBER_MOVE pair

#define DEFINE_JSON_TAGGED_OBJECT(CLASS_NAME, ...) \
class CLASS_NAME{\
public:\
    CLASS_NAME(){};\
    explicit CLASS_NAME(QJsonObject&& val, const bool checkValues=true) : MAP(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_MOVE_UNPACK, __VA_ARGS__) dummyVar(false) {}; \
    explicit CLASS_NAME(const QJsonObject& val, const bool checkValues=true) : MAP(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK, __VA_ARGS__) dummyVar(false) {}; \
    explicit CLASS_NAME(const QJsonValue& val, const bool checkValues=true) : MAP(TAGGEDOBJECTMACRO_INITIALIZE_MEMBER_UNPACK, __VA_ARGS__) dummyVar(false) {}; \
    explicit CLASS_NAME(const QByteArray& json, const bool checkValues=true) {\
        QJsonObject val = QJsonDocument::fromJson(json).object();\
        MAP(TAGGEDOBJECTMACRO_SET_MEMBER_MOVE_UNPACK, __VA_ARGS__)\
    };\
    explicit CLASS_NAME(const QString& filePath, const bool checkValues=true) {\
        QFile f{filePath};\
        f.open(QIODevice::ReadOnly);\
        QJsonObject val = QJsonDocument::fromJson(f.readAll()).object();\
        MAP(TAGGEDOBJECTMACRO_SET_MEMBER_MOVE_UNPACK, __VA_ARGS__)\
    };\
    MAP(TAGGEDOBJECTMACRO_DECLARE_MEMBER_UNPACK, __VA_ARGS__)\
private:\
    bool dummyVar;\
};

#endif // TAGGEDJSONOBJECTMACROS_H
