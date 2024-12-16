#include "gtest/gtest.h"
#include "taggedjsonobject.h"
#include "taggedjsonobjectmacros.h"


namespace {
    constexpr auto EXAMPLE_FILE_PATH = "../Tests/test_file.json";
    constexpr int EXPECTED_INT_RESULT = 42;
    constexpr auto EXPECTED_STRING_RESULT = "Hello world!";
    constexpr double EXPECTED_DOUBLE_RESULT = 35.5;
    constexpr auto EXPECTED_SUBSTRING_RESULT = "Hello from an object!";
    constexpr auto EXPECTED_JSON_VALUE_RESULT = 12;
    constexpr auto EXPECTED_ARRAY_ACCESS_RESULT = 2;

    QJsonDocument jsonFromFile()
    {
        QFile f{ EXAMPLE_FILE_PATH };
        f.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(f.readAll());
    }
}

TJO_DEFINE_JSON_TAGGED_OBJECT(InnerClass,
                          (TaggedJSONString, example_sub_str))

TJO_DEFINE_JSON_TAGGED_OBJECT(OuterClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedQJsonObject, example_json_object),
                          (TaggedJSONValue, example_json_value1),
                          (TaggedJSONValue, example_json_value2))

TJO_DEFINE_JSON_TAGGED_OBJECT(MissingValuedClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedQJsonObject, example_json_object),
                          (TaggedJSONValue, example_json_value1),
                          (TaggedJSONValue, example_json_value2),
                          (TaggedJSONInt, missing_member))


class TaggedObjectFixture : public testing::Test
{
public:
    TaggedObjectFixture() : testObj(OuterClass{jsonFromFile().toJson()}) {};
    OuterClass testObj;
};


// Tagged object can be constructed from a file path that json file holds
TEST(ConstructorTests, ConstructFromDirectoryTest)
{
    const QString jsonLocation(EXAMPLE_FILE_PATH);
    const OuterClass obj{jsonLocation, true};

    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

// Tagged object path constructor throws an exception if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromDirectoryStrictTestFail)
{
    const QString jsonLocation(EXAMPLE_FILE_PATH);
    ASSERT_THROW(MissingValuedClass( jsonLocation, true ), std::runtime_error);
}

// Tagged object path constructor won't throw an exception even if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromDirectoryNonStrictTestPass)
{
    const QString jsonLocation(EXAMPLE_FILE_PATH);
    ASSERT_NO_THROW(MissingValuedClass( jsonLocation, false ));
}

// Tagged object can be constructed from a json data that is stored on a bytearray.
TEST(ConstructorTests, ConstructFromJSONText)
{
    QByteArray jsonText = jsonFromFile().toJson();
    OuterClass obj{jsonText};
    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

// Tagged object json text constructor throws an exception if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromTextStrictTestFail)
{
    QByteArray jsonText = jsonFromFile().toJson();
    ASSERT_THROW(MissingValuedClass( jsonText, true ), std::runtime_error);
}


// Tagged object path constructor won't throw an exception even if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromTextNonStrictTestPass)
{
    QByteArray jsonText = jsonFromFile().toJson();
    ASSERT_NO_THROW(MissingValuedClass( jsonText, false ));
}


// Tagged objects can be constructed by a QJsonObject as well
TEST(ConstructorTests, ConstructFromQJsonObject)
{
    QJsonObject jsonObject = jsonFromFile().object();
    OuterClass obj{jsonObject};
    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

// QJsonObject constructor throws an exception if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromQJsonObjectStrictTestFail)
{
    QJsonObject jsonObject = jsonFromFile().object();
    ASSERT_THROW(MissingValuedClass(jsonObject, true), std::runtime_error);
}

// QJsonObject constructor won't throw an exception even if at least one of the keys doesn't exist on the target file 
TEST(ConstructorTests, ConstructorFromQJsonObjectNonStrictTestPass)
{
    QJsonObject jsonObject = jsonFromFile().object();
    ASSERT_NO_THROW(MissingValuedClass(jsonObject, false));
}

// Tagged object can be constructed with values of its members
TEST(ConstructorTests, ConstructorFromMemberInitialization)
{
    OuterClass testObj{ EXPECTED_INT_RESULT, EXPECTED_STRING_RESULT, EXPECTED_DOUBLE_RESULT, InnerClass{QJsonObject{}, false}, QJsonObject{}, QJsonValue{}, QJsonValue{}};

    ASSERT_EQ(EXPECTED_INT_RESULT, *testObj.example_int);
}


// Asterisk operator can be used for obtaining the target data
TEST_F(TaggedObjectFixture, IntegerGetFromAsterisk)
{
    ASSERT_EQ(EXPECTED_INT_RESULT, *testObj.example_int);
}

// Get method can be used for obtaining the target data as well
TEST_F(TaggedObjectFixture, IntegerGetFromMethod)
{
    ASSERT_EQ(EXPECTED_INT_RESULT, testObj.example_int.get());
}

// Initial value from the tagged object can be modified with the assignment operator
TEST_F(TaggedObjectFixture, IntegerSetFromAssignment)
{
    constexpr int MODIFIED_VALUE = 5;
    testObj.example_int = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_int);
}

// Initial value from the tagged object can also be modified by the set method
TEST_F(TaggedObjectFixture, IntegerSetFromMethod)
{
    constexpr int MODIFIED_VALUE = 5;
    testObj.example_int.set(MODIFIED_VALUE);
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_int);
}

// Asterisk operator has an l-value reference overload that can be modified from
TEST_F(TaggedObjectFixture, IntegerSetFromAsterisk)
{
    constexpr int MODIFIED_VALUE = 5;
    *testObj.example_int = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_int);
}

// Equality operator can be used for comparing the stored values
TEST_F(TaggedObjectFixture, EqualityOperator)
{
    OuterClass other{ QByteArray(), false};
    other.example_int = EXPECTED_INT_RESULT;

    ASSERT_TRUE(testObj.example_int == other.example_int);
}

// Inequality operator can be used for comparing the stored values
TEST_F(TaggedObjectFixture, InequalityOperator)
{
    OuterClass other{ QByteArray(), false};
    other.example_int = EXPECTED_INT_RESULT + 1;
    
    ASSERT_TRUE(testObj.example_int != other.example_int);
}


// Contained value can be printed by an output stream
TEST_F(TaggedObjectFixture, IntegerSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_int << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_EQ("42\n", textOutput);
}

// Contained value can also be printed by qDebug and has a valid QString construction
TEST_F(TaggedObjectFixture, IntegerQDebugTest)
{
    QString qDebugOutput(testObj.example_int);
    ASSERT_EQ("42", qDebugOutput);
}

// TaggedJSONObject can be used for JSON doubles
TEST_F(TaggedObjectFixture, DoubleStorage)
{
    ASSERT_DOUBLE_EQ(EXPECTED_DOUBLE_RESULT, *testObj.example_double);
}

// STD stream can be used to print the double value
TEST_F(TaggedObjectFixture, DoubleSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_double << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_EQ("35.500000\n", textOutput);
}

// qDebug can be used for double wrapped object
TEST_F(TaggedObjectFixture, DoubleQDebugTest)
{
    QString qDebugOutput(testObj.example_double);
    ASSERT_EQ("35.5", qDebugOutput);
}

// TaggedJSONObject can be used to wrap JSON strings
TEST_F(TaggedObjectFixture, StringStorage)
{
    ASSERT_EQ(EXPECTED_STRING_RESULT, *testObj.example_str);
}

// STD stream can be used for printing the stored value
TEST_F(TaggedObjectFixture, StringSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_str << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_STREQ("Hello world!\n", textOutput.data());
}

// qDebug can be used for the string wrapped object
TEST_F(TaggedObjectFixture, StringQDebugTest)
{
    QString qDebugOutput(testObj.example_str);
    
    ASSERT_EQ(EXPECTED_STRING_RESULT, qDebugOutput);
}

// QString methods can be accessed with the arrow operator
TEST_F(TaggedObjectFixture, StringArrowOperatorTest)
{
    QByteArray byteArray = testObj.example_str->toUtf8();
    ASSERT_EQ(EXPECTED_STRING_RESULT, byteArray);
}

// Storage value that being stored in TaggedJSONValue can be accessed without dereferencing
TEST_F(TaggedObjectFixture, QJsonValueAccess)
{
    ASSERT_EQ(testObj.example_json_value1["test_value"].toInt(), EXPECTED_JSON_VALUE_RESULT);
}

// QJsonValues can store JSON arrays as well, so their access operations can be shortcutted as well
TEST_F(TaggedObjectFixture, QJsonArrayAccess)
{
    ASSERT_EQ(testObj.example_json_value2[1].toInt(), EXPECTED_ARRAY_ACCESS_RESULT);
}

// Storage value that being stored in TaggedQJsonObject can be accessed without dereferencing
TEST_F(TaggedObjectFixture, QJsonObjectAccess)
{
    ASSERT_EQ(testObj.example_json_object["test_value"].toInt(), EXPECTED_JSON_VALUE_RESULT);
}

// Storage value that being stored in TaggedQJsonObject can be modified without dereferencing
TEST_F(TaggedObjectFixture, QJsonObjectMutation)
{
    constexpr auto MODIFIED_VALUE = 25;
    testObj.example_json_object["test_value"] = MODIFIED_VALUE;
    ASSERT_EQ(testObj.example_json_object["test_value"].toInt(), MODIFIED_VALUE);
}

// Tagged Objects can be used inside each other to obtain nested JSON values
TEST_F(TaggedObjectFixture, SubclassGetFromAsterisk)
{
    ASSERT_EQ(EXPECTED_SUBSTRING_RESULT, *testObj.example_sub_class.example_sub_str);
}

// Chained access can be used to modified the wrapped object as well
TEST_F(TaggedObjectFixture, SubclassSetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "Hello from another object!";
    testObj.example_sub_class.example_sub_str = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, testObj.example_sub_class.example_sub_str);
}
