#ifndef TAGGEDJSONOBJECT_H
#define TAGGEDJSONOBJECT_H
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtGlobal>


template <typename T>
class TaggedJSONObject
{
public:
    TaggedJSONObject(){}; //Only exists for preventing initialization errors.
    TaggedJSONObject(const TaggedJSONObject& other) = default;
    TaggedJSONObject(TaggedJSONObject&& other) = default;
    ~TaggedJSONObject() = default;

    explicit TaggedJSONObject(const QJsonValue& val, const bool checkValue=true)
    {
        //Check if there is a valid data if it's intended
        if(checkValue && val.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONObject"));

        dispatchValue(val);
    };
    explicit TaggedJSONObject(QJsonValue&& val, const bool checkValue=true){
        //Check if there is a valid data if it's intended
        if(checkValue && val.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONObject"));

        else if constexpr(std::is_same_v<T, QJsonValue>)
            m_value = std::move(val);
        else
            dispatchValue(val);
    };
    explicit TaggedJSONObject(QJsonValueRef val, const bool checkValue=true){
        //Check if there is a valid data if it's intended
        if(checkValue && val.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONObject"));

        else if constexpr(std::is_same_v<T, QJsonValue>)
            m_value = std::move(val);
        else
            dispatchValue(val);
    };

    //Assignment operators
    TaggedJSONObject& operator=(const TaggedJSONObject& other) = default;
    TaggedJSONObject& operator=(TaggedJSONObject&& other) = default;
    TaggedJSONObject& operator=(const T& val) {this->m_value = val; return *this;};
    TaggedJSONObject& operator=(T&& val) {this->m_value = std::move(val); return *this;};

    //Getter/Setter
    T get() const {return m_value;};
    void set(const T& val) {m_value = val;};
    void set(T&& val) {m_value = std::move(val);};

    //Operator overloads
    bool operator!=(const T& other) const {return m_value != other;};
    bool operator==(const T& other) const {return m_value == other;};
    T& operator*() {return m_value;};
    const T* const operator->() const {return &m_value;};
    const T& operator*() const {return m_value;};

    //qDebug() stream overload
    operator QString() const {
        if constexpr(std::is_arithmetic_v<T>)
            return QString::number(m_value);
        else
            return QString(m_value);
    };

private:
    T m_value;

    template<class TO>
    friend std::ostream& operator<< (std::ostream& stream, const TaggedJSONObject<TO>& obj);

    //Determine the function to be called at compile time based on the current template parameter
    void dispatchValue(const QJsonValue& val){
        if constexpr(std::is_same_v<T, bool>)
            m_value = val.toBool();
        else if constexpr(std::is_same_v<T, int64_t>)
            m_value = val.toInteger();
        else if constexpr(std::is_integral_v<T>)
            m_value = val.toInt();
        else if constexpr(std::is_floating_point_v<T>)
            m_value = val.toDouble();
        else if constexpr(std::is_same_v<T, QJsonValue>)
            m_value = val;
        else if constexpr(std::is_same_v<T, QJsonObject>)
            m_value = val.toObject();
        else if constexpr(std::is_same_v<T, QString>)
            m_value = val.toString();
        else if constexpr(std::is_same_v<T, QVariant>)
            m_value = val.toVariant();
        else
            throw(std::invalid_argument("Template argument for the TaggedJSONObject is not valid"));
    }

};

template<typename T>
std::ostream& operator<< (std::ostream& stream, const TaggedJSONObject<T>& obj)
{
    std::string text;
    if constexpr(std::is_arithmetic_v<T>)
        text = std::to_string(obj.m_value);
    else
        text = QString(obj.m_value).toStdString();

    stream << text;
    return stream;
};




template<typename T>
class TaggedJSONArray
{
public:
    TaggedJSONArray(){}; //Only exists for preventing initialization errors.
    TaggedJSONArray(const TaggedJSONArray& other) = default;

    ~TaggedJSONArray() = default;

    explicit TaggedJSONArray(const QJsonArray& val) : m_arr(val){};
    explicit TaggedJSONArray(const QJsonValue& ref, const bool checkValue=true) : m_arr(ref.toArray()){
        //Check if there is a valid data if it's intended
        if(checkValue && ref.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONArray"));
    }

    //Move constructors (only supported after Qt5.10 for QJsonArray)
    #if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    TaggedJSONArray(TaggedJSONArray&& other) = default;
    explicit TaggedJSONArray(QJsonArray&& val) : m_arr(std::move(val)){};
    TaggedJSONArray& operator=(TaggedJSONArray&& other) = default;
    TaggedJSONArray& operator=(QJsonArray&& val) {m_arr = std::move(val);};
    explicit TaggedJSONArray(QJsonValue&& ref, const bool checkValue=true) : m_arr(std::move(ref.toArray())){
        //Check if there is a valid data if it's intended
        if(checkValue && ref.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONArray"));
    }
    #endif

    //Operator overloads
    bool operator!=(const QJsonArray& other) const {return m_arr != other;};
    bool operator==(const QJsonArray& other) const {return m_arr == other;};
    QJsonArray& operator*() {return m_arr;};
    const QJsonArray* operator->() const {return &m_arr;};
    const QJsonArray& operator*() const {return m_arr;};

    T operator[](const qsizetype i) const { return dispatchValue(m_arr[i]); };

    //Returned value has to be QJsonValueRef in order to be mutable
    QJsonValueRef at(const qsizetype i) { return m_arr[i]; };

    operator QString() const {
        QString ret;

        for(const auto& curVal : m_arr)
            ret.append(QString(dispatchValue(curVal)) + "\n");
        return ret;
    };

    std::vector<T> toStdVector() const{
        std::vector<T> ret;
        ret.reserve(m_arr.size());

        for(const auto& curVal : m_arr)
            ret.push_back(dispatchValue(curVal));
        return ret;
    };

    QVector<T> toQVector() const{
        QVector<T> ret;
        ret.reserve(m_arr.size());

        for(const auto& curVal : m_arr)
            ret.append(dispatchValue(curVal));
        return ret;
    }

private:
    QJsonArray m_arr;

    template<class TO>
    friend std::ostream& operator<< (std::ostream& stream, const TaggedJSONArray<TO>& obj);

    T dispatchValue(const QJsonValue& ref) const{
        if constexpr(std::is_same_v<T, bool>)
            return ref.toBool();
        else if constexpr(std::is_same_v<T, int64_t>)
            return ref.toInteger();
        else if constexpr(std::is_integral_v<T>)
            return ref.toInt();
        else if constexpr(std::is_floating_point_v<T>)
            return ref.toDouble();
        else if constexpr(std::is_same_v<T, QJsonValue>)
            return ref;
        else if constexpr(std::is_same_v<T, QJsonObject>)
            return ref.toObject();
        else if constexpr(std::is_same_v<T, QString>)
            return ref.toString();
        else if constexpr(std::is_same_v<T, QVariant>)
            return ref.toVariant();
        else
            throw(std::invalid_argument("Template argument for the TaggedJSONArray is not valid"));
    };
};


template<typename T>
std::ostream& operator<< (std::ostream& stream, const TaggedJSONArray<T>& obj)
{
    std::string text;
    for(const auto& curVal : obj.m_arr){
        if constexpr(std::is_arithmetic_v<T>)
            text = std::to_string(curVal) + "\n";
        else
            text = QString(obj.dispatchValue(curVal)).toStdString() + "\n";
        stream << text;
    }
    return stream;
};



//Type aliases
using TaggedJSONBool = TaggedJSONObject<bool>;
using TaggedJSONInt = TaggedJSONObject<int>;
using TaggedJSONInteger = TaggedJSONObject<qint64>;
using TaggedJSONDouble = TaggedJSONObject<double>;
using TaggedJSONValue = TaggedJSONObject<QJsonValue>;
using TaggedQJsonObject = TaggedJSONObject<QJsonObject>;
using TaggedJSONString = TaggedJSONObject<QString>;
using TaggedJSONVariant = TaggedJSONObject<QVariant>;
using TaggedJSONBoolArray = TaggedJSONArray<bool>;
using TaggedJSONIntArray = TaggedJSONArray<int>;
using TaggedJSONIntegerArray = TaggedJSONArray<qint64>;
using TaggedJSONDoubleArray = TaggedJSONArray<double>;
using TaggedJSONValueArray = TaggedJSONArray<QJsonValue>;
using TaggedQJsonObjectArray = TaggedJSONArray<QJsonObject>;
using TaggedJSONStringArray = TaggedJSONArray<QString>;
using TaggedJSONVariantArray = TaggedJSONArray<QVariant>;

#endif // TAGGEDJSONOBJECT_H
