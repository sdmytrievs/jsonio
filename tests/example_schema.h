#pragma once

const char* const schema_str = R"({
                               "name": "schema_test",
                               "namespaces": {
                                 "*": "schema_test"
                               },
                               "includes": [
                               ],
                               "enums": [
                               ],
                               "typedefs": [
                               ],
                               "structs": [
                                 {
                                   "name": "SimpleSchemaTest",
                                   "doc": "Simple schema class\n",
                                   "isException": false,
                                   "isUnion": false,
                                   "fields": [
                                     {
                                       "key": 1,
                                       "name": "vbool",
                                       "typeId": "bool",
                                       "doc": "A boolean value (true or false)\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 2,
                                       "name": "vint",
                                       "typeId": "i32",
                                       "doc": "A 32-bit signed integer\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 3,
                                       "name": "vdouble",
                                       "typeId": "double",
                                       "doc": "A 64-bit floating point number\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 4,
                                       "name": "vstring",
                                       "typeId": "string",
                                       "doc": "A text string encoded using UTF-8 encoding\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 5,
                                       "name": "vlist",
                                       "typeId": "list",
                                       "type": {
                                         "typeId": "list",
                                         "elemTypeId": "double"
                                       },
                                       "doc": "An ordered list of elements. Translates to an STL vector.\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 6,
                                       "name": "vmap",
                                       "typeId": "map",
                                       "type": {
                                         "typeId": "map",
                                         "keyTypeId": "string",
                                         "valueTypeId": "string"
                                       },
                                       "doc": "A map of strictly unique keys to values. Translates to an STL map.\n",
                                       "required": "req_out"
                                     }
                                   ]
                                 },
                                 {
                                   "name": "Describe",
                                   "doc": "Describe schema class\n",
                                   "isException": false,
                                   "isUnion": false,
                                   "fields": [
                                     {
                                       "key": 1,
                                       "name": "version",
                                       "typeId": "i32",
                                       "doc": "A Version\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 4,
                                       "name": "description",
                                       "typeId": "string",
                                       "doc": "A text string encoded using UTF-8 encoding\n",
                                       "required": "req_out"
                                     }
                                   ]
                                 },
                                 {
                                   "name": "SpecifiersData",
                                   "doc": "Array data class\n",
                                   "isException": false,
                                   "isUnion": false,
                                   "fields": [
                                     {
                                       "key": 1,
                                       "name": "group",
                                       "typeId": "string",
                                       "doc": "Group of specifiers\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 2,
                                       "name": "value",
                                       "typeId": "double",
                                       "doc": "Value\n",
                                       "required": "req_out",
                                       "minval": 1,
                                       "maxval": 1e10,
                                       "default": 100000
                                     }
                                   ]
                                 },
                                 {
                                   "name": "FormatData",
                                   "doc": "Object data class\n",
                                   "isException": false,
                                   "isUnion": false,
                                   "fields": [
                                     {
                                       "key": 1,
                                       "name": "width",
                                       "typeId": "i32",
                                       "doc": "Minimum number of characters to be printed\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 2,
                                       "name": "precision",
                                       "typeId": "i32",
                                       "doc": "The number of digits to be printed after the decimal point\n",
                                       "required": "req_out"
                                     }
                                   ]
                                 },
                                 {
                                   "name": "ComplexSchemaTest",
                                   "doc": "Complex schema class\n",
                                   "isException": false,
                                   "isUnion": false,
                                   "fields": [
                                     {
                                       "key": 1,
                                       "name": "about",
                                       "typeId": "struct",
                                       "type": {
                                         "typeId": "struct",
                                         "class": "Describe"
                                       },
                                       "doc": "Description of task\n",
                                       "required": "req_out",
                                       "default": {
                                         "version": 1
                                       }
                                     },
                                     {
                                       "key": 2,
                                       "name": "formats",
                                       "typeId": "map",
                                       "type": {
                                         "typeId": "map",
                                         "keyTypeId": "string",
                                         "valueTypeId": "struct",
                                         "valueType": {
                                           "typeId": "struct",
                                           "class": "FormatData"
                                         }
                                       },
                                       "doc": "Format data list\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 3,
                                       "name": "data",
                                       "typeId": "list",
                                       "type": {
                                         "typeId": "list",
                                         "elemTypeId": "struct",
                                         "elemType": {
                                           "typeId": "struct",
                                           "class": "SpecifiersData"
                                         }
                                       },
                                       "doc": "A Format described data\n",
                                       "required": "req_out"
                                     },
                                     {
                                       "key": 4,
                                       "name": "values",
                                       "typeId": "list",
                                       "type": {
                                         "typeId": "list",
                                         "elemTypeId": "list",
                                         "elemType": {
                                           "typeId": "list",
                                           "elemTypeId": "double"
                                         }
                                       },
                                       "doc": "2D list\n",
                                       "required": "req_out",
                                       "default": [
                                         [
                                           1,
                                           2
                                         ],
                                         [
                                           3,
                                           4
                                         ]
                                       ]
                                     }
                                   ]
                                 }
                               ],
                               "constants": [
                               ],
                               "services": [
                               ]
                             }


)" ;

const char* const simple_schema_value = R"({
                                        "vbool" :   true,
                                        "vint" :   -100,
                                        "vdouble" :   5.2,
                                        "vstring" :   "Test string",
                                        "vlist" :   [
                                             1.7,
                                             2.7,
                                             3.7,
                                             5.7
                                        ],
                                        "vmap" :   {
                                             "key1" :   "val1",
                                             "key2" :   "val2"
                                        }
                                   })";

const char* const complex_schema_value = R"({
                                         "about" :   {
                                              "version" :   1,
                                              "description" :   "About"
                                         },
                                         "formats" :   {
                                              "int" :   {
                                                   "width" :   5,
                                                   "precision" :   0
                                              },
                                              "float" :   {
                                                   "width" :   10,
                                                   "precision" :   4
                                              },
                                              "double" :   {
                                                   "width" :   15,
                                                   "precision" :   6
                                              }
                                         },
                                         "data" :   [
                                              {
                                                   "group" :   "float",
                                                   "value" :   1.4
                                              },
                                              {
                                                   "group" :   "int",
                                                   "value" :   100
                                              },
                                              {
                                                   "group" :   "double",
                                                   "value" :   1e-10
                                              },
                                              {
                                                   "group" :   "double",
                                                   "value" :   10000000000
                                              }
                                         ],
                                         "values" :   [
                                              [
                                                   1,
                                                   2,
                                                   3
                                              ],
                                              [
                                                   11,
                                                   12,
                                                   13
                                              ]
                                         ]
                                    })";
