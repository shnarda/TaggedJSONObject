#include "taggedjsonobject.h"
#include "taggedjsonarray.h"
#include "taggedjsonobjectmacros.h"

/*
Let's say, example.json file contains the following:

{
    "example_int": 42,
    "example_str": "Hello world!",
    "example_double": 35.5,
    "example_sub_class":{
        "example_sub_str": "Hello from an object!"
    },
    "example_arr": ["Hello", "World"],
    "example_mixed_arr": [42, "is", "the", "answer", "to", "everything"]
}
*/

TJO_DEFINE_JSON_TAGGED_OBJECT(InnerClass,
                         (TaggedJSONString, example_sub_str))

TJO_DEFINE_JSON_TAGGED_OBJECT(OuterClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedJSONStringArray, example_arr),
                          (TaggedJSONVariantArray, example_mixed_arr))



int main(int argc, char *argv[])
{
    QString jsonLocation("../../Examples/example.json");

    //Create an instance of the tagged object that has been defined by the macro
    OuterClass exampleObject(jsonLocation);

    //Alternatively exampleObject.example_int.get() can be used as well
    qDebug() << *exampleObject.example_int; //42

    //Mutating stored values is allowed as long as the class isn't const
    exampleObject.example_int = -1;
    qDebug() << exampleObject.example_int; //-1

    //Defining Tagged JSON objects inside another tagged JSON object allows chain access operations
    qDebug() << *exampleObject.example_sub_class.example_sub_str; // "Hello from an object!"

    //Arrays can be accessed by indexing
    qDebug() << exampleObject.example_arr[0]; //"Hello"
    exampleObject.example_arr.at(0) = "Another";
    qDebug() << exampleObject.example_arr; //"Another World"

    //Iterator access is available but it will be presented as QJsonValue
    [[maybe_unused]] const QJsonArray::const_iterator it = exampleObject.example_arr->begin();
    foreach(const QJsonValue& curText, *exampleObject.example_arr)
        qDebug() << curText.toString(); //"Another<newline>World";

    //JSON arrays with varied types can be integrated by TaggedJSONVariantArray
    int answer_to_everything = exampleObject.example_mixed_arr[0].toInt();
    QString text_everything = exampleObject.example_mixed_arr[5].toString();

    qDebug() << answer_to_everything;
    qDebug() << text_everything;

    return 0;
}
