#pragma once

#include <gtest/gtest.h>


#include "jsonio/jsondump.h"
#include "jsonio/schema_thrift.h"
#include "example_schema.h"


using namespace testing;
using namespace jsonio;


TEST( JsonioSchema, Struct )
{
    SchemasData all_schemas;

    all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
    all_schemas.addSchemaFormat(schema_thrift, schema_str);

    auto all_structs =  all_schemas.getStructs(false);
    EXPECT_EQ( all_structs.size(), static_cast<size_t>(5) );
    EXPECT_EQ( json::dump(all_structs) , "[ \"ComplexSchemaTest\", \"Describe\", \"FormatData\", \"SimpleSchemaTest\", \"SpecifiersData\" ]" );

    auto schem_simpl = all_schemas.getStruct("SimpleSchemaTest");
    EXPECT_NE( schem_simpl, nullptr );
    EXPECT_EQ( all_schemas.getStruct("NotExist"), nullptr );

    EXPECT_EQ( schem_simpl->name(), "SimpleSchemaTest" );
    EXPECT_EQ( schem_simpl->description(), "Simple schema class " );
    EXPECT_EQ( schem_simpl->isUnion(), false );

    EXPECT_EQ( schem_simpl->getSelectedList(), list_names_t{} );
    EXPECT_EQ( schem_simpl->getUniqueList(), list_names_t{} );
    EXPECT_EQ( schem_simpl->getKeyTemplateList(), list_names_t{} );
}

TEST( JsonioSchema, Field )
{
    SchemasData all_schemas;

    all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
    all_schemas.addSchemaFormat(schema_thrift, schema_str);

    auto schem_simpl = all_schemas.getStruct("SimpleSchemaTest");
    EXPECT_NE( schem_simpl, nullptr );

    auto fields = schem_simpl->getFields(false);
    EXPECT_EQ( fields.size(), static_cast<size_t>(6) );
    EXPECT_EQ( json::dump(fields) , "[ \"vbool\", \"vint\", \"vdouble\", \"vstring\", \"vlist\", \"vmap\" ]" );

    auto dfield = schem_simpl->getField("vdouble");
    EXPECT_EQ( dfield->id(), 3 );
    EXPECT_EQ( dfield->name(), "vdouble" );
    EXPECT_EQ( dfield->type(), FieldDef::T_DOUBLE );
    EXPECT_EQ( dfield->required(), FieldDef::fld_default );
    EXPECT_EQ( dfield->description(), "A 64-bit floating point number " );
    EXPECT_EQ( dfield->className(), "" );
    EXPECT_EQ( dfield->defaultValue(), "" );
    EXPECT_EQ( dfield->minValue(), std::numeric_limits<double>::lowest() );
    EXPECT_EQ( dfield->maxValue(), std::numeric_limits<double>::max() );
}

TEST( JsonioSchema, DefaultField )
{
    SchemasData all_schemas;

    all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
    all_schemas.addSchemaFormat(schema_thrift, schema_str);

    auto schem_spec = all_schemas.getStruct("SpecifiersData");
    EXPECT_NE( schem_spec, nullptr );

    auto fields = schem_spec->getFields(false);
    EXPECT_EQ( fields.size(), static_cast<size_t>(2) );
    EXPECT_EQ( json::dump(fields) , "[ \"group\", \"value\" ]" );

    auto dfield = schem_spec->getField("value");
    EXPECT_EQ( dfield->id(), 2 );
    EXPECT_EQ( dfield->name(), "value" );
    EXPECT_EQ( dfield->type(), FieldDef::T_DOUBLE );
    EXPECT_EQ( dfield->required(), FieldDef::fld_default );
    EXPECT_EQ( dfield->description(), "Value " );
    EXPECT_EQ( dfield->className(), "" );
    EXPECT_EQ( dfield->defaultValue(), "100000" );
    EXPECT_EQ( dfield->minValue(), 1 );
    EXPECT_EQ( dfield->maxValue(), 1e10 );
}

TEST( JsonioSchema, ComplexField )
{
    SchemasData all_schemas;

    all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
    all_schemas.addSchemaFormat(schema_thrift, schema_str);

    auto schem_spec = all_schemas.getStruct("ComplexSchemaTest");
    EXPECT_NE( schem_spec, nullptr );

    auto fields = schem_spec->getFields(false);
    EXPECT_EQ( fields.size(), static_cast<size_t>(4) );
    EXPECT_EQ( json::dump(fields) , "[ \"about\", \"formats\", \"data\", \"values\" ]" );

    auto dfield = schem_spec->getField("about");
    EXPECT_EQ( dfield->id(), 1 );
    EXPECT_EQ( dfield->name(), "about" );
    EXPECT_EQ( dfield->type(), FieldDef::T_STRUCT );
    EXPECT_EQ( dfield->required(), FieldDef::fld_default );
    EXPECT_EQ( dfield->description(), "Description of task " );
    EXPECT_EQ( dfield->className(), "Describe" );
    EXPECT_EQ( dfield->defaultValue(), "{\"version\":1}" );
    EXPECT_THROW( dfield->type(1), jsonio_exception );

    dfield = schem_spec->getField("values");
    EXPECT_EQ( dfield->id(), 4 );
    EXPECT_EQ( dfield->name(), "values" );
    EXPECT_EQ( dfield->type(), FieldDef::T_LIST );
    EXPECT_EQ( dfield->type(1), FieldDef::T_LIST );
    EXPECT_EQ( dfield->type(2), FieldDef::T_DOUBLE );
    EXPECT_EQ( dfield->required(), FieldDef::fld_default );
    EXPECT_EQ( dfield->description(), "2D list " );
    EXPECT_EQ( dfield->className(), "" );
    EXPECT_EQ( dfield->defaultValue(), "[[1,2],[3,4]]" );

    EXPECT_EQ( schem_spec->getField("formats")->className(), "FormatData" );
    EXPECT_EQ( schem_spec->getField("data")->className(), "SpecifiersData" );
}


TEST( JsonioSchema, StructList )
{
    const char* const schema_lst_str =
            R"({
            "name": "schema_test",
            "namespaces": {
              "*": "schema_test"
            },
            "structs": [
              {
                "name": "SimpleSchemaTest",
                "doc": "Simple schema class\n",
                "isException": false,
                "isUnion": false,
                "to_select": [ "1", "3",  "6", "4", "5" ],
                "to_key": [ "vint", "vstring" ],
                "to_unique": [ "vbool", "vint", "vstring" ],
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
              }
            ]
            }
     )";

SchemasData all_schemas;
all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
all_schemas.addSchemaFormat(schema_thrift, schema_lst_str);

auto schem_simpl = all_schemas.getStruct("SimpleSchemaTest");
EXPECT_NE( schem_simpl, nullptr );

EXPECT_EQ( schem_simpl->getSelectedList(), list_names_t({ "1", "3", "6", "4", "5" }) );
EXPECT_EQ( schem_simpl->getUniqueList(), list_names_t({ "vbool", "vint", "vstring" }) );
EXPECT_EQ( schem_simpl->getKeyTemplateList(), list_names_t({ "vint", "vstring" }) );
}


TEST( JsonioSchema, Enum )
{
    const char* const schema_enum_str =
            R"( { "name": "element",
            "doc": "Apache Thrift IDL definition for the element service interface\n",
            "namespaces": {
              "*": "element"
            },
            "includes": [
          "prop"
            ],
            "enums": [
              {
                "name": "ElementClass",
                "doc": "Classes (types) of elements (independent components)\n",
                "members": [
                  {
                    "name": "ELEMENT",
                    "value": 0,
                    "doc": "Chemical element\n"
                  },
                  {
                    "name": "ISOTOPE",
                    "value": 1,
                    "doc": "Isotope of chemical element\n"
                  },
                  {
                    "name": "LIGAND",
                    "value": 2,
                    "doc": "Ligand such as Oxa (oxalate)\n"
                  },
                  {
                    "name": "SURFSITE",
                    "value": 3,
                    "doc": "Surface site in adsorption models\n"
                  },
                  {
                    "name": "CHARGE",
                    "value": 4,
                    "doc": "Electrical charge\n"
                  },
                  {
                    "name": "OTHER_EC",
                    "value": 5,
                    "doc": "Other type of independent component\n"
                  }
                ]
              },
              {
                "name": "DecayType",
                "doc": "Types of radioactive decay\n",
                "members": [
                  {
                    "name": "STABLE",
                    "value": 0,
                    "doc": "Stable isotope (no decay)\n"
                  },
                  {
                    "name": "ALPHA",
                    "value": 1,
                    "doc": "Alpha decay\n"
                  },
                  {
                    "name": "BETA",
                    "value": 2,
                    "doc": "Beta decay\n"
                  },
                  {
                    "name": "GAMMA",
                    "value": 3,
                    "doc": "Gamma decay\n"
                  },
                  {
                    "name": "POSITRON",
                    "value": 4,
                    "doc": "Positron decay\n"
                  },
                  {
                    "name": "NEUTRON",
                    "value": 5,
                    "doc": "Neutron decay\n"
                  },
                  {
                    "name": "PROTON",
                    "value": 6,
                    "doc": "Proton decay\n"
                  },
                  {
                    "name": "OTHER_DT",
                    "value": 7,
                    "doc": "Other type of decay\n"
                  },
                  {
                    "name": "FISSION",
                    "value": 8,
                    "doc": "Fission\n"
                  },
                  {
                    "name": "TRANSMUTATION",
                    "value": 9,
                    "doc": "Transmutation\n"
                  }
                ]
              },
              {
                "name": "SourceTDB",
                "doc": "Codes of source thermodynamic data bases for substances and reactions\n",
                "members": [
                  {
                    "name": "CODATA",
                    "value": 0,
                    "doc": "CODATA 1989\n"
                  },
                  {
                    "name": "NIST",
                    "value": 1,
                    "doc": "NIST Wagman et al. 1989\n"
                  },
                  {
                    "name": "SUPCRT",
                    "value": 2,
                    "doc": "SUPCRT slop98.dat\n"
                  },
                  {
                    "name": "HOLLAND_POWELL",
                    "value": 3,
                    "doc": "Holland and Powell 2001\n"
                  },
                  {
                    "name": "BERMAN_BROWN",
                    "value": 4,
                    "doc": "Berman and Brown 1998\n"
                  },
                  {
                    "name": "ROBIE_HEMINGWAY",
                    "value": 5,
                    "doc": "Robie and Hemingway 1995\n"
                  },
                  {
                    "name": "GOTTSCHALK",
                    "value": 6,
                    "doc": "Gottschalk 1999\n"
                  },
                  {
                    "name": "BABUSHKIN",
                    "value": 7,
                    "doc": "Babushkin et al. 1986\n"
                  },
                  {
                    "name": "KUBASCHEWSKI",
                    "value": 8,
                    "doc": "Kubaschewski er al. 1993\n"
                  },
                  {
                    "name": "KONINGS",
                    "value": 9,
                    "doc": "Konings et al. 1998\n"
                  },
                  {
                    "name": "REID_GL",
                    "value": 10,
                    "doc": "Reid et al. 1987 (gases and liquids)\n"
                  },
                  {
                    "name": "NEA",
                    "value": 11,
                    "doc": "NEA TDB Project\n"
                  },
                  {
                    "name": "THEREDA",
                    "value": 12,
                    "doc": "THEREDA Project\n"
                  },
                  {
                    "name": "THERMODDEM",
                    "value": 13,
                    "doc": "THERMODDEM Project\n"
                  },
                  {
                    "name": "SMITH_MARTELL",
                    "value": 14,
                    "doc": "Smith and Martell 1978\n"
                  },
                  {
                    "name": "PSI_NAGRA",
                    "value": 15,
                    "doc": "PSI-Nagra TDB 12\/07\n"
                  },
                  {
                    "name": "MOIETY",
                    "value": 16,
                    "doc": "Moiety in multi-site phase models (no TDB source)\n"
                  },
                  {
                    "name": "HKF_DEW",
                    "value": 17,
                    "doc": "Sverjensky et al. 2014\n"
                  },
                  {
                    "name": "MINES",
                    "value": 18,
                    "doc": "Mines16 TDB Gysi, 2016\n"
                  },
                  {
                    "name": "AQ17",
                    "value": 19,
                    "doc": "Aq17 TDB Miron et al., 2017\n"
                  },
                  {
                    "name": "CEMDATA18",
                    "value": 20,
                    "doc": "CEMDATA 18 Lothenbach et al., 2018\n"
                  },
                  {
                    "name": "SUPCRT07",
                    "value": 21,
                    "doc": "SUPCRT 2007 slop07.dat\n"
                  },
                  {
                    "name": "HERACLES",
                    "value": 22,
                    "doc": "Heracles\n"
                  },
                  {
                    "name": "SLOP16",
                    "value": 23,
                    "doc": "SLOP 2016 slop16.dat\n"
                  },
                  {
                    "name": "WORK1",
                    "value": 1001,
                    "doc": "SourceTDB code for data under work\/development\n"
                  },
                  {
                    "name": "WORK2",
                    "value": 1002,
                    "doc": "SourceTDB code for data under work\/development\n"
                  },
                  {
                    "name": "WORK3",
                    "value": 1003,
                    "doc": "SourceTDB code for data under work\/development\n"
                  },
                  {
                    "name": "WORK4",
                    "value": 1004,
                    "doc": "SourceTDB code for data under work\/development\n"
                  },
                  {
                    "name": "OTHER",
                    "value": 1111,
                    "doc": "Other data sources\n"
                  }
                ]
              }
            ]
          })";

    SchemasData all_schemas;

    all_schemas.addSchemaMethod( schema_thrift, ThriftSchemaRead );
    all_schemas.addSchemaFormat(schema_thrift, schema_enum_str);

    auto all_enums =  all_schemas.getEnums();
    EXPECT_EQ( all_enums.size(), static_cast<size_t>(3) );
    EXPECT_EQ( json::dump(all_enums) ,"[ \"DecayType\", \"ElementClass\", \"SourceTDB\" ]" );

    auto aenum = all_schemas.getEnum("ElementClass");
    EXPECT_NE( aenum, nullptr );
    EXPECT_EQ( all_schemas.getEnum("NotExist"), nullptr );

    EXPECT_EQ( aenum->name(), "ElementClass" );
    EXPECT_EQ( aenum->description(), "Classes (types) of elements (independent components) " );

    EXPECT_EQ( aenum->value2name(4), "CHARGE" );
    EXPECT_EQ( aenum->name2value("CHARGE"), 4 );
    EXPECT_EQ( aenum->name2description("CHARGE"), "Electrical charge " );

    EXPECT_EQ( aenum->all_names(), list_names_t({ "CHARGE", "ELEMENT", "ISOTOPE", "LIGAND", "OTHER_EC", "SURFSITE" }) );

    EXPECT_EQ( aenum->value2name(100), "" );
    EXPECT_EQ( aenum->name2value("NOT_EXIST"), EnumDef::empty_enum );
}
