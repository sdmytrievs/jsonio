/* thrift -r -v --gen json schema_test.thrift */

namespace * schema_test

/** Simple schema class */
struct SimpleSchemaTest {
    /** A boolean value (true or false) */
    1: bool vbool
    /** A 32-bit signed integer */
    2: i32 vint
    /** A 64-bit floating point number */
    3: double vdouble
    /** A text string encoded using UTF-8 encoding */
    4: string vstring
    /** An ordered list of elements. Translates to an STL vector. */
    5: list<double> vlist
    /** A map of strictly unique keys to values. Translates to an STL map. */
    6: map<string,string> vmap
}

/* Complex part */

/** Describe schema class */
struct Describe {
    /** A Version */
    1: i32 version
    /** A text string encoded using UTF-8 encoding */
    4: string description
}

/** Array data class */
struct SpecifiersData {
    /** Group of specifiers */
    1: string group
    /** Value */
    2: double value = 1.0e5
}


/** Object data class */
struct FormatData {
    /** Minimum number of characters to be printed */
    1: i32 width
    /** The number of digits to be printed after the decimal point */
    2: i32 precision
}

/** Complex schema class */
struct ComplexSchemaTest {
    /** Description of task */
    1: Describe about = { "version": 1 }
    /** Format data list */
    2: map<string,FormatData> formats
    /** A Format described data */
    3: list<SpecifiersData>  data
    /** 2D list */
    4: list<list<double>>   values = [[ 1 ,2 ],[ 3, 4 ]]
}
