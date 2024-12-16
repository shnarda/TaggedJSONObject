#ifndef TAGGEDJSONOBJECT_H
#define TAGGEDJSONOBJECT_H
#include <QJsonObject>
#include <QJsonArray>

//! All types that can be encapsulated in QJsonObject
#define TJO_JSON_COMPATIBLE std::is_arithmetic_v<T> || std::is_same_v<T, QJsonValue> || std::is_same_v<T, QJsonObject>|| std::is_same_v<T, QString> || std::is_same_v<T, QVariant>

/*!
 * \class TaggedJSONObject
 * \brief The TaggedJSONObject class stores the JSON data given by the tagged object.
 *
 * If the type of the value the key holds is known at compile time, (QVariant can still be used as a template parameter if it isn't known)
 * QJSONObject can be used to introduce type knowledge to target data.\n
 * Main usage of this object is to treat JSON data as class member for the tagged objects that has been generated from the #TJO_DEFINE_JSON_TAGGED_OBJECT(CLASS_NAME, ...) macro.\n
 * TaggedJSONObject can store all types of data JSON format accepts.
 */
template <typename T, typename=std::enable_if_t<TJO_JSON_COMPATIBLE>>
class TaggedJSONObject
{
public:
    /*!
     * \brief TaggedJSONObject Constructor that takes the JSON data and stores it according to the template argument.
     * \param val Target JSON value to be stored.
     * \param checkValue If set to true, invalid conversions (missing value, wrong type etc.) will throw a runtime error.
     */
    explicit TaggedJSONObject(QJsonValue val, const bool checkValue=true)
    {
        //Check if there is a valid data if it's intended
        if(checkValue && val.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONObject"));

        dispatchValue(std::move(val));
    };

    //! Implicit value constructor for the tagged object constructor
    template<typename V, typename = std::enable_if_t<std::is_convertible_v<V, T>>>
    TaggedJSONObject(V&& val) { m_value = std::forward<V>(val); };

    /*!
     * \brief l-value reference getter for the contained object
     * \return The l-value reference of the contained object
     */
    T& get() {return m_value;};

    /*!
     * \brief const reference getter for the contained object
     * \return The const reference of the contained object
     */
    const T& get() const {return m_value;};

    /*!
     * \brief operator = Setter for the contained object
     * \param val Value to set
     * \return Modified instance of this class
     */
    template<typename V, typename=std::enable_if_t<std::is_convertible_v<V, T>>>
    TaggedJSONObject& operator=(V&& val) {m_value = std::forward<V>(val); return *this;};

    /*!
     * \brief set Setter for the contained object
     * \param val Value to set
     */
    template<typename V, typename= std::enable_if_t<std::is_convertible_v<V, T>>>
    void set(V&& val) {m_value = std::forward<V>(val);};

    //Assignment shortcuts
    bool operator!=(const T& other) const {return m_value != other;};
    bool operator==(const T& other) const {return m_value == other;};
    bool operator!=(const TaggedJSONObject<T>& other) const { return m_value != other.m_value; };
    bool operator==(const TaggedJSONObject<T>& other) const { return m_value == other.m_value; };

    /*!
     * \brief operator * l-value getter for the contained object
     * \return The l-value reference of the contained object
     */
    T& operator*() {return m_value;};

    /*!
     * \brief operator * const reference getter for the contained object
     * \return const reference of the contained object
     */
    const T& operator*() const {return m_value;};

    /*!
     * \brief operator -> Shortcut for object values (QString, QVariant etc.) methods.
     * \return The contained object
     */
    const T* const operator->() const {return &m_value;};

    //! Shortcut for value operations (for QJsonValue and QJsonObjects only)
    template<typename S=T, typename=std::enable_if_t<std::is_same_v<S, QJsonValue> || std::is_same_v<S, QJsonObject>>>
    QJsonValue operator[](const QString& key) const { return m_value[key]; };

    template<typename S = T, typename = std::enable_if_t<std::is_same_v<S, QJsonObject>>>
    QJsonValueRef operator[](const QString& key) { return m_value[key]; };

    template<typename S = T, typename = std::enable_if_t<std::is_same_v<S, QJsonValue>>>
    QJsonValue operator[](const qsizetype i) const { return m_value[i]; };

    //!\brief operator QString QString constructor variant for qDebug stream access.
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
    void dispatchValue(QJsonValue&& val){
        if constexpr(std::is_same_v<T, bool>)
            m_value = val.toBool();
        else if constexpr(std::is_integral_v<T>)
            m_value = val.toInt();
        else if constexpr(std::is_floating_point_v<T>)
            m_value = val.toDouble();
        else if constexpr(std::is_same_v<T, QJsonValue>)
            m_value = std::move(val);
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

/*!
 * \brief operator << stdout implementation for the contained object.
 * \param stream The std stream
 * \param obj The class instance
 * \return The output stream
 */
template<typename T>
std::ostream& operator<< (std::ostream& stream, const TaggedJSONObject<T>& obj)
{
    std::string text;
    if constexpr (std::is_arithmetic_v<T>)
        text = std::to_string(obj.m_value);
    else
        text = QString(obj.m_value).toStdString();

    stream << text;
    return stream;
};

//Type aliases
using TaggedJSONBool = TaggedJSONObject<bool>;
using TaggedJSONInt = TaggedJSONObject<int>;
using TaggedJSONDouble = TaggedJSONObject<double>;
using TaggedJSONValue = TaggedJSONObject<QJsonValue>;
using TaggedQJsonObject = TaggedJSONObject<QJsonObject>;
using TaggedJSONString = TaggedJSONObject<QString>;
using TaggedJSONVariant = TaggedJSONObject<QVariant>;

#endif // TAGGEDJSONOBJECT_H
