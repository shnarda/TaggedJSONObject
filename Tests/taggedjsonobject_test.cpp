#include "gtest/gtest.h"
#include "taggedjsonobject.h"
#include "taggedjsonobjectmacros.h"

constexpr auto EXAMPLE_FILE_PATH = "../../Examples/example.json";
constexpr int EXPECTED_INT_RESULT = 42;
constexpr auto EXPECTED_STRING_RESULT = "Hello world!";
constexpr double EXPECTED_DOUBLE_RESULT = 35.5;
constexpr auto EXPECTED_SUBSTRING_RESULT = "Hello from an object!";
constexpr auto EXPECTED_ARRAY_FIRST_ELEMENT_RESULT = "Hello";
constexpr int EXPECTED_MIXEDARRAY_FIRST_ELEMENT_RESULT = 42;
constexpr auto EXPECTED_MIXEDARRAY_LAST_ELEMENT_RESULT = "everything";

DEFINE_JSON_TAGGED_OBJECT(InnerClass,
                          (TaggedJSONString, example_sub_str))

DEFINE_JSON_TAGGED_OBJECT(OuterClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedJSONStringArray, example_arr),
                          (TaggedJSONVariantArray, example_mixed_arr))

DEFINE_JSON_TAGGED_OBJECT(MissingValuedClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedJSONStringArray, example_arr),
                          (TaggedJSONVariantArray, example_mixed_arr),
                          (TaggedJSONInt, missing_member))


QJsonDocument jsonFromFile()
{
    QFile f{EXAMPLE_FILE_PATH};
    f.open(QIODevice::ReadOnly);
    return QJsonDocument::fromJson(f.readAll());
}




class TaggedObjectFixture : public testing::Test
{
public:
    TaggedObjectFixture() : testObj(OuterClass{jsonFromFile().toJson()}) {};
    OuterClass testObj;
};


TEST(ConstructorTests, ConstructFromDirectoryTest)
{
    QString jsonLocation(EXAMPLE_FILE_PATH);
    OuterClass obj{jsonLocation};
    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

TEST(ConstructorTests, ConstructFromJSONText)
{
    QByteArray jsonText = jsonFromFile().toJson();
    OuterClass obj{jsonText};
    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

TEST(ConstructorTests, ConstructFromQJsonObject)
{
    QJsonObject jsonObject = jsonFromFile().object();
    OuterClass obj{jsonObject};
    ASSERT_EQ(EXPECTED_INT_RESULT, *obj.example_int);
}

TEST(ConstructorTests, CheckValueTest)
{
    ASSERT_THROW(MissingValuedClass{jsonFromFile().toJson()}, std::runtime_error);
}

TEST(ConstructorTests, IgnoreMissingValueTest)
{
    ASSERT_NO_THROW(MissingValuedClass(jsonFromFile().toJson(), false));
}




TEST_F(TaggedObjectFixture, IntegerGetFromAsterisk)
{
    ASSERT_EQ(EXPECTED_INT_RESULT, *testObj.example_int);
}

TEST_F(TaggedObjectFixture, IntegerGetFromMethod)
{
    ASSERT_EQ(EXPECTED_INT_RESULT, testObj.example_int.get());
}

TEST_F(TaggedObjectFixture, IntegerSetFromAssignment)
{
    constexpr int MODIFIED_VALUE = 5;
    testObj.example_int = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_int);
}

TEST_F(TaggedObjectFixture, IntegerSetFromAsterisk)
{
    constexpr int MODIFIED_VALUE = 5;
    *testObj.example_int = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_int);
}

TEST_F(TaggedObjectFixture, IntegerSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_int << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_EQ("42\n", textOutput);
}

TEST_F(TaggedObjectFixture, IntegerQDebugTest)
{
    QString qDebugOutput(testObj.example_int);
    ASSERT_EQ("42", qDebugOutput);
}



TEST_F(TaggedObjectFixture, DoubleGetFromAsterisk)
{
    ASSERT_DOUBLE_EQ(EXPECTED_DOUBLE_RESULT, *testObj.example_double);
}

TEST_F(TaggedObjectFixture, DoubleGetFromMethod)
{
    ASSERT_DOUBLE_EQ(EXPECTED_DOUBLE_RESULT, testObj.example_double.get());
}

TEST_F(TaggedObjectFixture, DoubleSetFromAssignment)
{
    constexpr double MODIFIED_VALUE = 5.2;
    testObj.example_double = MODIFIED_VALUE;
    ASSERT_DOUBLE_EQ(MODIFIED_VALUE, *testObj.example_double);
}

TEST_F(TaggedObjectFixture, DoubleSetFromAsterisk)
{
    constexpr double MODIFIED_VALUE = 5.2;
    *testObj.example_double = MODIFIED_VALUE;
    ASSERT_DOUBLE_EQ(MODIFIED_VALUE, *testObj.example_double);
}

TEST_F(TaggedObjectFixture, DoubleSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_double << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_EQ("35.500000\n", textOutput);
}

TEST_F(TaggedObjectFixture, DoubleQDebugTest)
{
    QString qDebugOutput(testObj.example_double);
    ASSERT_EQ("35.5", qDebugOutput);
}



TEST_F(TaggedObjectFixture, StringGetFromAsterisk)
{
    ASSERT_EQ(EXPECTED_STRING_RESULT, *testObj.example_str);
}

TEST_F(TaggedObjectFixture, StringGetFromMethod)
{
    ASSERT_EQ(EXPECTED_STRING_RESULT, testObj.example_str.get());
}

TEST_F(TaggedObjectFixture, StringSetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "Another World";
    testObj.example_str = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_str);
}

TEST_F(TaggedObjectFixture, StringSetFromAsterisk)
{
    constexpr auto MODIFIED_VALUE = "Another World";
    *testObj.example_str = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_str);
}

TEST_F(TaggedObjectFixture, StringSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_str << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_STREQ("Hello world!\n", textOutput.data());
}

TEST_F(TaggedObjectFixture, StringQDebugTest)
{
    QString qDebugOutput(testObj.example_str);
    ASSERT_EQ(EXPECTED_STRING_RESULT, qDebugOutput);
}

TEST_F(TaggedObjectFixture, StringArrowOperatorTest)
{
    QByteArray byteArray = testObj.example_str->toUtf8();
    ASSERT_EQ(EXPECTED_STRING_RESULT, byteArray);
}



TEST_F(TaggedObjectFixture, SubclassGetFromAsterisk)
{
    ASSERT_EQ(EXPECTED_SUBSTRING_RESULT, *testObj.example_sub_class.example_sub_str);
}

TEST_F(TaggedObjectFixture, SubclassGetFromMethod)
{
    ASSERT_EQ(EXPECTED_SUBSTRING_RESULT, testObj.example_sub_class.example_sub_str.get());
}

TEST_F(TaggedObjectFixture, SubclassSetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "Hello from another object!";
    testObj.example_sub_class.example_sub_str = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_sub_class.example_sub_str);
}

TEST_F(TaggedObjectFixture, SubclassSetFromAsterisk)
{
    constexpr auto MODIFIED_VALUE = "Hello from another object!";
    *testObj.example_sub_class.example_sub_str = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, *testObj.example_sub_class.example_sub_str);
}

TEST_F(TaggedObjectFixture, SubclassSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_sub_class.example_sub_str << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_STREQ("Hello from an object!\n", textOutput.data());
}

TEST_F(TaggedObjectFixture, SubclassQDebugTest)
{
    QString qDebugOutput(testObj.example_sub_class.example_sub_str);
    ASSERT_EQ(EXPECTED_SUBSTRING_RESULT, qDebugOutput);
}


TEST(ConstructorTests, ArrayConstructFromDirectoryTest)
{
    QString jsonLocation(EXAMPLE_FILE_PATH);
    OuterClass obj{jsonLocation};
    ASSERT_EQ(false, obj.example_arr->isEmpty());
}

TEST(ConstructorTests, ArrayConstructFromJSONText)
{
    QByteArray jsonText = jsonFromFile().toJson();
    OuterClass obj{jsonText};
    ASSERT_EQ(false, obj.example_arr->isEmpty());
}

TEST(ConstructorTests, ArrayConstructFromQJsonObject)
{
    QJsonObject jsonObject = jsonFromFile().object();
    OuterClass obj{jsonObject};
    ASSERT_EQ(false, obj.example_arr->isEmpty());
}

TEST(ConstructorTests, ArrayCheckValueTest)
{
    ASSERT_THROW(MissingValuedClass{jsonFromFile().toJson()}, std::runtime_error);
}

TEST(ConstructorTests, ArrayIgnoreMissingValueTest)
{
    ASSERT_NO_THROW(MissingValuedClass(jsonFromFile().toJson(), false));
}




TEST_F(TaggedObjectFixture, ArrayGetFromBrackets)
{
    ASSERT_EQ(EXPECTED_ARRAY_FIRST_ELEMENT_RESULT, testObj.example_arr[0]);
}

TEST_F(TaggedObjectFixture, ArrayGetFromMethod)
{
    ASSERT_EQ(EXPECTED_ARRAY_FIRST_ELEMENT_RESULT, testObj.example_arr.at(0).toString());
}

TEST_F(TaggedObjectFixture, ArraySetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "Another World";
    testObj.example_arr.at(0) = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, testObj.example_arr[0]);
}

TEST_F(TaggedObjectFixture, ArrayWholeSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_arr << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_STREQ("Hello\nWorld\n\n", textOutput.data());
}

TEST_F(TaggedObjectFixture, ArrayWholeQDebugTest)
{
    QString qDebugOutput(testObj.example_arr);
    ASSERT_EQ(std::string("Hello\nWorld\n"), qDebugOutput.toStdString());
}



TEST_F(TaggedObjectFixture, MixedArrayGetFromBrackets)
{
    ASSERT_EQ(EXPECTED_MIXEDARRAY_FIRST_ELEMENT_RESULT, testObj.example_mixed_arr[0].toInt());
}

TEST_F(TaggedObjectFixture, MixedArrayGetFromMethod)
{
    ASSERT_EQ(EXPECTED_MIXEDARRAY_LAST_ELEMENT_RESULT, testObj.example_mixed_arr.at(5).toString());
}

TEST_F(TaggedObjectFixture, MixedArraySetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "nothing";
    testObj.example_mixed_arr.at(5) = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, testObj.example_mixed_arr[5].toString());
}



int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

