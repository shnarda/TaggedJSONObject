#include "gtest/gtest.h"
#include "taggedjsonobject.h"
#include "taggedjsonarray.h"
#include "taggedjsonobjectmacros.h"

namespace {
    constexpr auto EXAMPLE_FILE_PATH = "../Tests/test_file.json";
    constexpr auto EXPECTED_ARRAY_FIRST_ELEMENT_RESULT = "Hello";
    constexpr int EXPECTED_MIXEDARRAY_FIRST_ELEMENT_RESULT = 42;
    constexpr auto EXPECTED_MIXEDARRAY_LAST_ELEMENT_RESULT = "everything";
    constexpr auto EXPECTED_REDUCED_RESULT = "HelloWorld";
    constexpr auto EXPECTED_NAME_RESULT = "Michael";
    constexpr auto EXPECTED_TOTAL_AGE = 75;

    QJsonDocument jsonFromFile()
    {
        QFile f{ EXAMPLE_FILE_PATH };
        f.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(f.readAll());
    }
}

TJO_DEFINE_JSON_TAGGED_OBJECT(Identity,
                            (TaggedJSONString, name),
                            (TaggedJSONInt, age))


TJO_DEFINE_JSON_TAGGED_OBJECT(TestClass,
                          (TaggedJSONStringArray, example_arr),
                          (TaggedJSONVariantArray, example_mixed_arr),
                          (TaggedJSONArray<Identity>, example_tagged_object_array))


class TaggedArrayFixture : public testing::Test
{
public:
    TaggedArrayFixture() : testObj(TestClass{jsonFromFile().toJson()}) {};
    TestClass testObj;
};

//Square bracket operator can be used to access the n'th element for TaggedJSONArray
TEST_F(TaggedArrayFixture, ArrayGetFromBrackets)
{
    ASSERT_EQ(EXPECTED_ARRAY_FIRST_ELEMENT_RESULT, testObj.example_arr[0].toString());
}

//At method can be used to access the n'th element for TaggedJSONArray
TEST_F(TaggedArrayFixture, ArrayGetFromMethod)
{
    ASSERT_EQ(EXPECTED_ARRAY_FIRST_ELEMENT_RESULT, testObj.example_arr.at(0).toString());
}

//At method can also be used to return QJSonValueRef, which can be mutated
TEST_F(TaggedArrayFixture, ArraySetFromAssignment)
{
    constexpr auto MODIFIED_VALUE = "Another World";
    testObj.example_arr.at(0) = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, testObj.example_arr[0].toString());
}

//Square bracket operator can also be used to return QJSonValueRef, which can be mutated
TEST_F(TaggedArrayFixture, ArraySetFromBrackets)
{
    constexpr auto MODIFIED_VALUE = "Another World";
    testObj.example_arr[0] = MODIFIED_VALUE;
    ASSERT_EQ(MODIFIED_VALUE, testObj.example_arr[0].toString());
}

//Arrow operator can be used to access some of the useful QJsonArray features like iterators
TEST_F(TaggedArrayFixture, ArrayArrowOperator)
{
    auto lambda_customSum = [](QString previous, QJsonValue current) -> QString {return previous + current.toString(); };
    const QString concatText = std::reduce(testObj.example_arr->begin(), testObj.example_arr->end(), QString(), lambda_customSum);
    ASSERT_EQ(std::string(EXPECTED_REDUCED_RESULT), concatText.toStdString());
}

//Using stout stream on the TaggedJSONArray prints all of its elements
TEST_F(TaggedArrayFixture, ArrayWholeSTDOutTest)
{
    testing::internal::CaptureStdout();
    std::cout << testObj.example_arr << std::endl;
    const std::string textOutput(testing::internal::GetCapturedStdout());
    ASSERT_STREQ("Hello\nWorld\n\n", textOutput.data());
}

//QDebug can also be used to print all of its elements
TEST_F(TaggedArrayFixture, ArrayWholeQDebugTest)
{
    QString qDebugOutput(testObj.example_arr);
    ASSERT_EQ(std::string("Hello\nWorld\n"), qDebugOutput.toStdString());
}

//Mixed arrays can be wrapped thanks to the QVariant containers
TEST_F(TaggedArrayFixture, MixedArrayGetFromBrackets)
{
    ASSERT_EQ(EXPECTED_MIXEDARRAY_FIRST_ELEMENT_RESULT, testObj.example_mixed_arr[0].toInt());
}

//Tagged objects can be placed on the TaggedJSONArrays and at method can be used to immutable access to the indexed object (just like the std::vector)
TEST_F(TaggedArrayFixture, TaggedObjectContainer)
{
    ASSERT_EQ(QString(EXPECTED_NAME_RESULT), *testObj.example_tagged_object_array.at(1).name);
}

//Internal vector of the TaggedJSONArray can be accessed by the asterisk operator
TEST_F(TaggedArrayFixture, TaggedObjectContainerVectorOperations)
{
    const auto lambda_accumulateAges = [](const int previous, const Identity& curIdentity) -> int {return previous + *curIdentity.age; };
    const int totalAge = std::reduce(testObj.example_tagged_object_array->cbegin(), testObj.example_tagged_object_array->cend(), 0, lambda_accumulateAges);

    ASSERT_EQ(EXPECTED_TOTAL_AGE, totalAge);
}

//Square brackets can be used for the mutable access of the indexed object (just like the std::vector)
TEST_F(TaggedArrayFixture, TaggedObjectContainerMutation)
{
    constexpr auto MODIFIED_NAME = "Charles";
    constexpr auto MODIFIED_AGE = 16;

    QJsonObject newObj;
    newObj["name"] = MODIFIED_NAME;
    newObj["age"] = MODIFIED_AGE;

    testObj.example_tagged_object_array[0] = Identity{ newObj };
    ASSERT_EQ(MODIFIED_AGE, *testObj.example_tagged_object_array.at(0).age);
}