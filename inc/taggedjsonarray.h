#ifndef TAGGEDJSONARRAY_H
#define TAGGEDJSONARRAY_H
#include <type_traits>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

//! All types that can be encapsulated in QJsonObject
#define TJO_JSON_COMPATIBLE std::is_arithmetic_v<T> || std::is_same_v<T, QJsonValue> || std::is_same_v<T, QJsonObject>|| std::is_same_v<T, QString> || std::is_same_v<T, QVariant>

//! Only objects that have been defined with the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro
#define TJO_IS_TAGGED_OBJECT std::is_constructible_v<T, QJsonValue, const bool> &&\
                             std::is_constructible_v<T, QJsonObject, const bool> &&\
                             std::is_constructible_v<T, const QString&, const bool> &&\
                             std::is_constructible_v<T, const QByteArray&, const bool>

namespace {
    template<typename T>
    std::vector<T> extractFromQJSONArray(QJsonArray arr, const bool checkValue)
    {
        std::vector<T> out;
        out.reserve(arr.size());
        foreach(const QJsonValue & val, arr) {
            out.emplace_back(val, checkValue);
        }

        return out;
    }
}


//! Invalid TaggedJSONArray template for SFINAE
template<typename T, typename = void>
class TaggedJSONArray;

/*!
 * \brief The TaggedJSONArray class variation that accepts JSON compatible data as its container.
 *
 * This class is used for storing JSON arrays with a preknown type (QVariantArrays can be used for mixed types)\n
 * Main usage for this class is to treat JSON data as class member for the tagged objects that has been generated from the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro.\n
 * TaggedJSONArray can store all types of data JSON format accepts but TaggedJSONArray<T, typename std::enable_if_t< std::is_constructible_v<T, QJsonValue, bool>&& std::is_constructible_v<T, QString, bool>&& std::is_constructible_v<T, QByteArray, bool>&& std::is_constructible_v <T, QJsonObject, bool>>>
 * is more suitable for chaining JSON objects within JSON arrays.
 */
template<typename T>
class TaggedJSONArray <T, typename std::enable_if_t<TJO_JSON_COMPATIBLE>>
{
public:
    /*!
    * \brief TaggedJSONArray constructor variant that takes QJsonArray input directly
    * \param val target JSON array data
    */
    explicit TaggedJSONArray(QJsonArray val) : m_arr(std::move(val)) {};

    /*!
     * \brief TaggedJSONArray constructor variant that takes QJsonValue input
     *
     * This constructor is intended for the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro
     * \param ref target JSON array data
     * \param checkValue If set to true, invalid conversions (missing value, wrong type etc.) will throw a runtime error.
     */
    explicit TaggedJSONArray(const QJsonValue& ref, const bool checkValue = true) : m_arr(ref.toArray()) {
        //Check if there is a valid data if it's intended
        if (checkValue && ref.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONArray"));
    }

    //! Assignment operator setter
    template<typename V, typename = std::enable_if_t<std::is_convertible_v<V, QJsonArray>>>
    void operator=(V&& val) { m_arr = std::forward<V>(val); };

    //Operator overloads
    bool operator!=(const QJsonArray& other) const { return m_arr != other; };
    bool operator==(const QJsonArray& other) const { return m_arr == other; };

    /*!
     * \brief operator * Operator overload for the whole JSON array
     *
     * If the contents of the array desired to be taken as std::vector or QVector, see toStdVector() and toQvector()
     * \return Stored JSON array
     */
    QJsonArray& operator*() { return m_arr; };

    /*!
     * \brief operator * Operator overload for the whole JSON array
     *
     * If the contents of the array desired to be taken as std::vector or QVector, see toStdVector() and toQvector()
     * \return Stored JSON array
     */
    const QJsonArray& operator*() const { return m_arr; };

    /*!
     * \brief operator -> This operator is a shortcut for QJSONArray methods
     * \return Stored JSON array
     */
    const QJsonArray* operator->() const { return &m_arr; };

    /*!
     * \brief operator [] Shortcut for the mutable access operator for the storage.
     * \param i index to be accessed
     * \return i'th element of the JSON array
     */
    QJsonValueRef operator[](const qsizetype i) { return m_arr[i]; };

    /*!
     * \brief operator [] Shortcut for the immutable access operator for the storage.
     * \param i index to be accessed
     * \return i'th element of the JSON array
     */
    T operator[](const qsizetype i) const { return dispatchValue(m_arr[i]); };

    /*!
     * \brief at Shortcut for the mutable access for the storage.
     * \param i index to be accessed
     * \return i'th element of the JSON array
     */
    QJsonValueRef at(const qsizetype i) { return m_arr[i]; };

    /*!
     * \brief at Shortcut for the immutable access for the storage.
     * \param i index to be accessed
     * \return i'th element of the JSON array
     */
    T at(const qsizetype i) const { return dispatchValue(m_arr[i]); };

    //!\brief operator QString QString constructor variant for qDebug stream access.
    operator QString() const {
        QString ret;

        for (const auto& curVal : m_arr)
            ret.append(QString(dispatchValue(curVal)) + "\n");
        return ret;
    };

    /*!
     * \brief toStdVector Converts stored JSON array to a std::vector with the class templated type
     * \return std::vector of the stored data with the class templated type
     */
    std::vector<T> toStdVector() const {
        std::vector<T> ret;
        ret.reserve(m_arr.size());

        for (const auto& curVal : m_arr)
            ret.push_back(dispatchValue(curVal));
        return ret;
    };

    /*!
     * \brief toQVector Converts stored JSON array to a QVector with the class templated type
     * \return QVector of the stored data with the class templated type
     */
    QVector<T> toQVector() const {
        QVector<T> ret;
        ret.reserve(m_arr.size());

        for (const auto& curVal : m_arr)
            ret.append(dispatchValue(curVal));
        return ret;
    }

private:
    QJsonArray m_arr;

    template<class TO>
    friend std::ostream& operator<< (std::ostream& stream, const TaggedJSONArray<TO>& obj);

    T dispatchValue(const QJsonValue& ref) const {
        if constexpr (std::is_same_v<T, bool>)
            return ref.toBool();
        else if constexpr (std::is_integral_v<T>)
            return ref.toInt();
        else if constexpr (std::is_floating_point_v<T>)
            return ref.toDouble();
        else if constexpr (std::is_same_v<T, QJsonValue>)
            return ref;
        else if constexpr (std::is_same_v<T, QJsonObject>)
            return ref.toObject();
        else if constexpr (std::is_same_v<T, QString>)
            return ref.toString();
        else if constexpr (std::is_same_v<T, QVariant>)
            return ref.toVariant();
        else
            throw(std::invalid_argument("Template argument for the TaggedJSONArray is not valid"));
    };
};

/*!
 * \brief operator << stdout implementation for the contained object.
 * \param stream The std stream
 * \param obj The class instance
 * \return The output stream
 */
template<typename T>
std::ostream& operator<< (std::ostream& stream, const TaggedJSONArray<T>& obj)
{
    std::string text;
    for (const auto& curVal : obj.m_arr) {
        if constexpr (std::is_arithmetic_v<T>)
            text = std::to_string(curVal) + "\n";
        else
            text = QString(obj.dispatchValue(curVal)).toStdString() + "\n";
        stream << text;
    }
    return stream;
};


/*!
 * @brief Tagged object vector specialization of the TaggedJSONArray
 * 
 * This object can be used for encapsulating JSON array of previously defined tagged object, which makes accessing them quite convenient to access.\n
 * Intended usage of this object is to use this class as member of the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro.\n
 * This template specialization can only take the objects that has been defined by the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro. Passing any other object may cause unintended side effects.
 * @tparam T 
*/
template<typename T>
class TaggedJSONArray<T, typename std::enable_if_t<TJO_IS_TAGGED_OBJECT>>
{
public:
    /*!
     * @brief Main constructor that is intended to be used with the #TJO_DEFINE_JSON_TAGGED_OBJECT() macro.
     * @param ref QJsonValue that holds the array of predefined JSON objects.
     * @param checkValue If set to true, invalid conversions (missing value, wrong type etc.) will throw a runtime error.
    */
    explicit TaggedJSONArray(const QJsonValue& ref, const bool checkValue = true) : m_arr(extractFromQJSONArray<T>(ref.toArray(), checkValue))
    {
        //Check if there is a valid data if it's intended
        if (checkValue && ref.isUndefined())
            throw(std::runtime_error("Invalid data has been encountered while parsing the json data for TaggedJSONArray"));
    }

    //!Assignment operator to get the whole container data 
    template<typename V, typename = std::enable_if_t<std::is_convertible_v<V, std::vector<T>>>>
    void operator=(V&& val) { m_arr = std::forward<V>(val); };

    bool operator!=(const std::vector<T>& other) const { return m_arr != other; };
    bool operator==(const std::vector<T>& other) const { return m_arr == other; };

    //!Mutable reference of the stored object
    std::vector<T>& operator*() { return m_arr; };

    //!Immutable reference of the stored object
    const std::vector<T>& operator*() const { return m_arr; };

    //!Can be used for accessing the std::vector operations on the encapsulated data
    const std::vector<T>* operator->() const { return &m_arr; };

    //!Mutable access operator
    T& operator[](const qsizetype i) { return m_arr[i]; };

    //!Immutable access operator
    const T& at(const qsizetype i) const { return m_arr.at(i); };

    //! Mutable access operator
    T& at(const qsizetype i) { return m_arr.at(i); };

    //!QDebug enabler 
    operator QString() const {
        QString ret;

        for (const auto& curVal : m_arr)
            ret.append(QString(curVal) + "\n");
        return ret;
    };

    //!QVector access
    QVector<T> toQVector() const { return QVector::fromStdVector(m_arr); }

private:
    std::vector<T> m_arr;
};

using TaggedJSONBoolArray = TaggedJSONArray<bool>;
using TaggedJSONIntArray = TaggedJSONArray<int>;
using TaggedJSONDoubleArray = TaggedJSONArray<double>;
using TaggedJSONValueArray = TaggedJSONArray<QJsonValue>;
using TaggedQJsonObjectArray = TaggedJSONArray<QJsonObject>;
using TaggedJSONStringArray = TaggedJSONArray<QString>;
using TaggedJSONVariantArray = TaggedJSONArray<QVariant>;

#endif // TAGGEDJSONARRAY_H
