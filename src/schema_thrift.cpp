//  This is JSONIO library+API (https://bitbucket.org/gems4/jsonio)
//
/// \file thrift_schema.cpp
/// Implementation of classes and functions to work with Thrift Schemas
//
// JSONIO is a C++ library and API aimed at implementing the interfaces
// for exchanging the structured data between NoSQL database backends,
// JSON/YAML/XML files, and client-server RPC (remote procedure calls).
//
// Copyright (c) 2015-2016 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONIO depends on the following open-source software products:
// Apache Thrift (https://thrift.apache.org); Pugixml (http://pugixml.org);
// YAML-CPP (https://github.com/jbeder/yaml-cpp); EJDB (http://ejdb.org).
//

#include <fstream>
#include "jsonio/thrift_schema.h"
#include "jsonio/jsondomfree.h"
#include "jsonio/tf_json.h"
using namespace std;

namespace jsonio {

ThriftFieldDef ThriftStructDef::emptyfield = { -1, "", {}, 2, "", "", "", "", DOUBLE_EMPTY, DOUBLE_EMPTY };
int ThriftEnumDef::emptyenum = SHORT_EMPTY;

static const char * key_name = "name";
static const char * key_doc = "doc";
static const char * key_fields ="fields";
static const char * key_isException ="isException";
static const char * key_isUnion ="isUnion";
static const char * key_toselect ="to_select";
static const char * key_tokeytemplate ="to_key";
static const char * key_unique_fields ="to_unique";

static const char * key_key ="key";
static const char * key_typeId = "typeId";
static const char * key_type = "type";
static const char * key_elemTypeId = "elemTypeId";
static const char * key_elemType = "elemType";
static const char * key_keyTypeId ="keyTypeId";
static const char * key_keyType ="keyType";
static const char * key_valueTypeId ="valueTypeId";
static const char * key_valueType ="valueType";
static const char * key_default ="default";
static const char * key_minval ="minval";
static const char * key_maxval ="maxval";
static const char * key_class ="class";
static const char * key_required ="required";
static const char * key_req_out ="req_out";
static const char * key_optional ="optional";
static const char * key_undefined ="undefined";

static const char * key_members ="members";
static const char * key_value ="value";
static const char * key_structs ="structs";
static const char * key_enums ="enums";



void ThriftSchema::setTypeMap()
{
  name_to_thrift_types.insert(pair<string, int>("stop", Th_STOP));
  name_to_thrift_types.insert(pair<string, int>("void", Th_VOID));
  name_to_thrift_types.insert(pair<string, int>("bool", Th_BOOL));
  name_to_thrift_types.insert(pair<string, int>("byte", Th_BYTE));
  name_to_thrift_types.insert(pair<string, int>("i8", Th_I08));
  name_to_thrift_types.insert(pair<string, int>("i16", Th_I16));
  name_to_thrift_types.insert(pair<string, int>("i32", Th_I32));
  name_to_thrift_types.insert(pair<string, int>("u64", Th_U64));
  name_to_thrift_types.insert(pair<string, int>("i64", Th_I64 ));
  name_to_thrift_types.insert(pair<string, int>("double", Th_DOUBLE));
  name_to_thrift_types.insert(pair<string, int>("string", Th_STRING));
  name_to_thrift_types.insert(pair<string, int>("utf7", Th_UTF7));
  name_to_thrift_types.insert(pair<string, int>("struct", Th_STRUCT));
  name_to_thrift_types.insert(pair<string, int>("union", Th_STRUCT));
  name_to_thrift_types.insert(pair<string, int>("exception", Th_STRUCT));
  name_to_thrift_types.insert(pair<string, int>("map", Th_MAP));
  name_to_thrift_types.insert(pair<string, int>("set", Th_SET));
  name_to_thrift_types.insert(pair<string, int>("list", Th_LIST));
  name_to_thrift_types.insert(pair<string, int>("utf8", Th_UTF8));
  name_to_thrift_types.insert(pair<string, int>("utf16", Th_UTF16));
}


/// Read json schema for one thrift structure from bsondata
void ThriftStructDef::readSchema( const JsonDom* object )
{
    // read information about structure
    if(!object->findValue( key_name, name ) )
        name = key_undefined;  // May be exeption
    if(!object->findValue(  key_isException, isException ) )
        isException=false;
    if(!object->findValue(  key_isUnion, isUnion ) )
        isUnion=false;
    if(!object->findValue(  key_doc, sDoc ) )
        sDoc="";
   replaceall(sDoc, "\t\r\n", ' ' );
   // read to_select
   object->findArray(  key_toselect, keyIdSelect );
   // read to_key
   object->findArray(  key_tokeytemplate, keyTemplateSelect );
   // read unique_fields
   object->findArray(  key_unique_fields, uniqueList );

    // read arrays of fields
    auto fieldsarr  =object->field( key_fields );
    for( uint ii=0; ii<fieldsarr->getChildrenCount(); ii++ )
    {

       auto fieldsobject = fieldsarr->getChild( ii);
       ThriftFieldDef value;
       if(!fieldsobject->findValue( key_key, value.fId ) )
            value.fId=-1;
       if(!fieldsobject->findValue( key_name, value.fName ) )
            value.fName=key_undefined;

        string requred;
        if(!fieldsobject->findValue( key_required, requred ) )
            requred=key_req_out;
        if(requred == key_required )
             value.fRequired = fld_required;
            else if( requred == key_optional )
                   value.fRequired = fld_optional;
                 else
                    value.fRequired = fld_default;

        // vector<int> fTypeId; // "typeId"+"type"+"elemTypeId"+"elemType"  - all levels
        if(!fieldsobject->findValue( key_class, value.className ) )
           value.className = "";   // "class": enum name

        string  typeId;
        if(!fieldsobject->findValue( key_typeId, typeId ) )
            value.fTypeId.push_back(Th_VOID);
        else
            read_type_spec(fieldsobject, value, key_type, typeId);

        // default value we get to string for different types
        fieldsobject->findValue( key_default, value.fDefault );
        value.insertedDefault = "";

        if(!fieldsobject->findValue( key_doc, value.fDoc ) )
            value.fDoc="";
        replaceall(value.fDoc, "\t\r\n", ' ' );
        if(!fieldsobject->findValue( key_minval, value.minval ) )
            value.minval=DOUBLE_EMPTY;
        if(!fieldsobject->findValue( key_maxval, value.maxval ) )
            value.maxval=DOUBLE_EMPTY;

        fields.push_back( value );
        //id2index.insert( pair<int, uint>(value.fId, ii));
        //name2index.insert( pair<string, uint>(value.fName, ii) );
        id2index[value.fId] = ii;
        name2index[value.fName] = ii;
    }

}

/// Read json schema for thrift structure from bsondata
void ThriftStructDef::read_type_spec( const JsonDom* object,
        ThriftFieldDef& value, const char* keyspec, const string& typeID)
{
   auto it = ThriftSchema::name_to_thrift_types.find(typeID);
   if( it == ThriftSchema::name_to_thrift_types.end() )
    jsonioErr("Undefined type", typeID );

   ThType type = static_cast<ThType>(it->second);
   value.fTypeId.push_back(type);

  if( type == Th_STRUCT )
  {
    auto nextobj = object->field( keyspec );
    if(!nextobj->findValue( key_class, value.className ) )
        jsonioErr("Undefined class name", typeID );
    size_t pos = value.className.find_last_of('.');
    if( pos != std::string::npos)
       value.className.erase(0, pos+1);
  } else if ( type == Th_MAP)
   {
     auto nextobj = object->field( keyspec );
     string  typeId;
     if(!nextobj->findValue(  key_keyTypeId, typeId ) )
         value.fTypeId.push_back(Th_VOID);
     else
         read_type_spec(nextobj, value, key_keyType, typeId);
     if(!nextobj->findValue(  key_valueTypeId, typeId ) )
         value.fTypeId.push_back(Th_VOID);
     else
         read_type_spec(nextobj, value, key_valueType, typeId);
  }
    else
      if (type == Th_LIST || type == Th_SET)
      {
        auto nextobj = object->field( keyspec );
        string  typeId;
        if(!nextobj->findValue(  key_elemTypeId, typeId ) )
              value.fTypeId.push_back(Th_VOID);
        else
              read_type_spec(nextobj, value, key_elemType, typeId);
      }
}

//---------------------------------------------------------------------------

/// Read json schema for one thrift enum from bsondata
void ThriftEnumDef::readEnum( const JsonDom* object )
{
    // read information about enum
    if(!object->findValue( key_name, name ) )
        name = key_undefined;  // May be exeption
    if(!object->findValue( key_doc, sDoc ) )
        sDoc="";
    replaceall(sDoc, "\t\r\n", ' ' );

    // read arrays of fields
    string ename, edoc;
    int eId;
    auto fieldsarr  =  object->field( key_members );
    for( std::size_t ii=0; ii<fieldsarr->getChildrenCount(); ii++ )
    {
        auto fieldsobject = fieldsarr->getChild(ii);

        if(!fieldsobject->findValue(  key_name, ename ) )
           ename=key_undefined;
        if(!fieldsobject->findValue(  key_doc, edoc ) )
            edoc="";
        replaceall(edoc, "\t\r\n", ' ' );
        if(!fieldsobject->findValue(  key_value, eId ) )
            eId=emptyenum;

        name2index.insert( pair<string, int>(ename, eId) );
        name2doc.insert( pair<string, string>(ename, edoc) );
    }

}

//-----------------------------------------------------------

map<string, int> ThriftSchema::name_to_thrift_types;

// Read thrift schema from json file fileName
void ThriftSchema::addSchemaFile( const char *fileName )
{
    shared_ptr<JsonDomFree> node(JsonDomFree::newObject());

    // Reading work structure from json text file
    fstream fin(fileName, ios::in);
    jsonioErrIf( !fin.good() , fileName, "Fileread error...");

    if( !parseJsonToNode( fin, node.get() ))
        return;

    // copy schema to internal structures
    readSchema( node.get() );
}

/// Read bson data with thrift schema
void ThriftSchema::readSchema( const JsonDom* object )
{
    string fname, fdoc;
    // read information about enum
    if(!object->findValue( key_name, fname ) )
        fname = key_undefined;  // May be exeption
    if(!object->findValue( key_doc, fdoc ) )
        fdoc="";
    files.insert( pair<string, string>(fname, fdoc) );

    // read enum array
    auto enumsarr  = object->field( key_enums );
    for( std::size_t ii=0; ii<enumsarr->getChildrenCount(); ii++ )
    {
        auto enumobject = enumsarr->getChild(ii);
        ThriftEnumDef* enumdata = new ThriftEnumDef(enumobject);
        enums.insert( pair<string, unique_ptr<ThriftEnumDef>>
                    (enumdata->getName(), unique_ptr<ThriftEnumDef>(enumdata )) );
    }

    // read structs array
    auto structsarr  = object->field( key_structs );
    for( std::size_t ii=0; ii<structsarr->getChildrenCount(); ii++ )
    {
        auto structobject = structsarr->getChild(ii);
        ThriftStructDef* structdata = new ThriftStructDef(structobject);
        structs.insert( pair<string, unique_ptr<ThriftStructDef>>
                    (structdata->getName(), unique_ptr<ThriftStructDef>(structdata )) );

        const ThriftFieldDef* typeFld = structdata->getFieldDef( "_type" );
        const ThriftFieldDef* labelFld = structdata->getFieldDef( "_label" );
        string label, name;
        if( typeFld != nullptr && labelFld != nullptr )
        {
            if( typeFld->fDefault == "vertex")
            {
                label = labelFld->fDefault;
                name = structdata->getName();
                vertexes[label ] = name;
                vertexCollections[name ] = label+"s";
            }
            else if( typeFld->fDefault == "edge")
                 {
                    label = labelFld->fDefault;
                    auto name1 = structdata->getName();
                    edges[ label ] = name1;
                    edgeCollections[ name1 ] = label;
                    allEdgesTraverse.push_back(label);
                }
        }
    }

}

/// Get structs names list
vector<string> ThriftSchema::getStructsList( bool withDoc ) const
{
  vector<string> members;
  auto it = structs.begin();
  while( it != structs.end() )
  {
     string vasl = it->first;
     if( withDoc  )
     {
        vasl+= ',';
        vasl+= it->second->getComment();
     }
     members.push_back(vasl);
     it++;
  }
  return members;
}


void ThriftSchema::findPathTo(const string &schemaname, const string &fieldname, std::set<string> &paths, const string &fieldhead) const
{
    if( schemaname.empty() )
        return;

    ThriftStructDef* strDef2 = getStruct( schemaname );
    if( strDef2 == nullptr )
        return;

    for( const auto& field: strDef2->fields )
    {
        if( field.fName == fieldname )
        {
            paths.insert(fieldhead);
        }

        if( !field.fTypeId.empty() && field.fTypeId.back() == Th_STRUCT )
        {
            auto newfieldhead = fieldhead;
            if( !newfieldhead.empty() )
                newfieldhead += ".";
            newfieldhead += field.fName;
            findPathTo( field.className, fieldname, paths, newfieldhead);
        }
    }
}



} // namespace jsonio
