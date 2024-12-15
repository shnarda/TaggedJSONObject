
# TaggedJSONObject

Using the JSON support from the Qt Framework, this library allows user to define labeled JSON objects that understands the type it currently holds, inspired from the Typescript's JSON interfaces. This comes with the following benefits:
1) Introduces the type awareness for the target JSON field. This allows the IDE to autocomplete the fields and their potential methods. Type awareness also prevents potential runtime errors that comes from mistyping or type safety related mistakes.
2) The object checks for the missing fields from the json data (it can be disabled)
3) Class interface reduces the verbosity while accessing the desired field
Documentation can be seen with the following: ![Documentation](docs/html/index.html)





## Installation

Requirements for this library are as follows:
1) QT Framework (Qt5 or later)
2) C++17 or later
\
This library only uses the header files, so building the project isn't required. \
Building the project can still be done for the tests and code examples. 
    
## Usage/Examples

The following example is based on the "Examples/example.cpp" example file.\
Let's say, a file named "example.json" contains the following json data:
```json
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
```
To parse this data, first we need to define our tagged JSON object class with the help of the "TJO_DEFINE_JSON_TAGGED_OBJECT" preprocessor macro. Each field of the defined class should match with the field names of the json data

```c++
/* 
This is the definition for the inner object which will be used for defining the outer object.
Alternatively, the inner class can be defined as TaggedQJsonObject at the outer class but this
prevents inner class members to be accessed directly, which defeats the purpose of this library.
*/
TJO_DEFINE_JSON_TAGGED_OBJECT(InnerClass,
                         (TaggedJSONString, example_sub_str))

/* 
The first parameter of the macro is the name of the class, which is OuterClass for this instance.
Other parameters are the type-field pairs that defines the members of the defined object.
It should be noted that TaggedJSONObject class variants should be used for the field types instead of
the regular types like int, double etc.
*/
TJO_DEFINE_JSON_TAGGED_OBJECT(OuterClass,
                          (TaggedJSONInt, example_int),
                          (TaggedJSONString, example_str),
                          (TaggedJSONDouble, example_double),
                          (InnerClass, example_sub_class),
                          (TaggedJSONStringArray, example_arr),
                          (TaggedJSONVariantArray, example_mixed_arr))
```
The intention of the statements above has the similar intentions with the statements form the following Typescript interfaces

```typescript
interface InnerClass{
  example_sub_str: string
}

interface OuterClass{
  example_int: number;
  example_str: string;
  example_double: number;
  example_sub_class: InnerClass;
  example_arr: Array<string>;
  example_mixed_arr: Array<any>;
}
```
Later on, we can declare an instance by, either passing a QByteArray that stores the json data, by passing a QJsonObject or by passing the QString that holds the path of the json document.

```c++
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
    const QJsonArray::const_iterator it = exampleObject.example_arr->begin();
    foreach(const QJsonValue& curText, *exampleObject.example_arr)
        qDebug() << curText.toString(); //"Another<newline>World";

    //JSON arrays with varied types can be integrated by TaggedJSONVariantArray
    int answer_to_everything = exampleObject.example_mixed_arr[0].toInt();
    QString text_everything = exampleObject.example_mixed_arr[5].toString();

    qDebug() << answer_to_everything; //42
    qDebug() << text_everything;  //everything
```


## License

Like Qt library, this library is under the dual license LGPLv3 and GPLv2.

