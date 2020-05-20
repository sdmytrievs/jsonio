//  This is JSONIO library+API (https://bitbucket.org/gems4/jsonio)
//
/// \file dbschema.cpp
/// Implementation of class TDBSchema - working with database
/// Used  SchemaNode class - API for direct code access to internal
/// DOM based on our JSON schemas.
//
// JSONIO is a C++ library and API aimed at implementing the interfaces
// for exchanging the structured data between NoSQL database backends,
// JSON/YAML/XML files, and client-server RPC (remote procedure calls).
//
// Copyright (c) 2018 Svetlana Dmytriieva (svd@ciklum.com) and
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

#include <iostream>
#include "jsonio/io_settings.h"
#include "jsonio/dbschemadoc.h"
#include "jsonio/jsondomfree.h"
using namespace std;

namespace jsonio {


TDBSchemaDocument* TDBSchemaDocument::newSchemaDocument( const TDataBase* dbconnect,
                           const string& schemaName, const string& collcName   )
{
    if( schemaName.empty() || collcName.empty() )
        return nullptr;

    TDBSchemaDocument* newDoc =  new TDBSchemaDocument( schemaName, dbconnect, collcName );
    return newDoc;
}

TDBSchemaDocument* TDBSchemaDocument::newSchemaDocumentQuery( const TDataBase* dbconnect, const std::string& schemaName,
                         const std::string& collcName, const DBQueryData& query  )
{
    if( schemaName.empty() || collcName.empty() )
        return nullptr;

    TDBSchemaDocument* newDoc =  new TDBSchemaDocument( schemaName, dbconnect, collcName );

    // init internal selection block
    newDoc->SetQuery(query);
    return newDoc;
}

//  Constructor
TDBSchemaDocument::TDBSchemaDocument( const string& aschemaName, TDBCollection* collection  ):
    TDBDocumentBase( collection ),
    _schema(ioSettings().Schema()), _schemaName(aschemaName), _currentData(nullptr)
{
    resetSchema( aschemaName, true );
}

TDBSchemaDocument::TDBSchemaDocument( const string& aschemaName, const TDataBase* dbconnect,
                                      const string& coltype, const string& colname ):
    TDBDocumentBase( dbconnect, coltype, colname ),
    _schema(ioSettings().Schema()), _schemaName(aschemaName), _currentData(nullptr)
{
    resetSchema( aschemaName, true );
}


/// Constructor from dom data
TDBSchemaDocument::TDBSchemaDocument( TDBCollection* collection, const JsonDom *object ):
    TDBDocumentBase( collection, object ), _schema(ioSettings().Schema()), _currentData(nullptr)
{
    fromJsonNode( object );
    resetSchema( _schemaName, true );
}

// Writes DOD data to ostream file
void TDBSchemaDocument::toJsonNode( JsonDom *object ) const
{
    TDBDocumentBase::toJsonNode( object );
    object->appendString( "SchemaName", _schemaName );
}

// Reads DOD data from istream file
void TDBSchemaDocument::fromJsonNode( const JsonDom *object )
{
    TDBDocumentBase::fromJsonNode( object );
    if(!object->findValue( "SchemaName", _schemaName ) )
        jsonioErr( "TGRDB0001: ", "Undefined SchemaName.");
}

void TDBSchemaDocument::resetSchema( const string& aschemaName, bool change_queries )
{
    /// !!! we use the same collection for different schemas
    /// if changed -> error
    if( _schemaName != aschemaName  || _currentData.get() == nullptr )
    {
        _schemaName = aschemaName;
        _currentData.reset( JsonDomSchema::newSchemaObject( _schemaName ) );
    }
    if( change_queries && _queryResult.get() != nullptr  )
    {
       // new one
       //auto query = _queryResult->getQuery();
       //SetQuery( query, makeDefaultQueryFields());
       // old one
       _queryResult->setFieldsCollect(makeDefaultQueryFields());
       _queryResult->setToSchema(_schemaName);
    }
}

JsonDomSchema* TDBSchemaDocument::recToSave(  time_t /*crtt*/, char* pars )
{
    // test oid
    JsonDomSchema* oidNode = _currentData.get()->field("_id");
    string oidval;
    if(oidNode!=nullptr)
        oidNode->getValue( oidval );

    if( oidNode==nullptr || oidval.empty() ) // no "_id" into schema definition
    {
        string _keyval;
        if( !getValue( "_key", _keyval ) || _keyval.empty()  ) // generate if no exist
            _keyval = makeTemplateKey(_currentData.get(), _currentData->getTemplateKeyList() );

        if( pars )
            _currentData->setOid_( string(pars) );
        else
            _currentData->setOid_( genOid(_keyval) );
    }
    else
    {
        setOid( oidval ); // set _key from _id (must be the same value)
        if( pars )
        {
            if( oidval != string(pars) )
                jsonioErr("TGRDB0003", "Changed record oid " + string(pars) );
        }
    }
    return _currentData.get();
}


// Load data from bson structure (return read record key)
string TDBSchemaDocument::recFromJson( const string& jsondata )
{
    SetJson( jsondata );
    // Get key of record
    return getKeyFromCurrent( );
}

// Return curent record in json format string
string TDBSchemaDocument::GetJson(bool dense ) const
{
    string jsonstr;
    printNodeToJson( jsonstr, _currentData.get(), dense );
    return  jsonstr;
}

/* Return curent record in YAML format string
string TDBSchemaDocument::GetYAML()
{
  string yamlstr;
  // record to YAML string
  ParserYAML::printNodeToYAML( yamlstr, _currentData.get() );
  return  yamlstr;
}*/


// Set json format string to curent record
void TDBSchemaDocument::SetJson( const string& sjson )
{
    _currentData.get()->clearField();

    //if( is_json )  // json
    parseJsonToNode( sjson, _currentData.get() );
    //else // yaml
    //    ParserYAML::parseYAMLToNode( sjson, _currentData.get() );
}


// Get current record key from internal data
string TDBSchemaDocument::getKeyFromCurrent() const
{
    string keyStr;
    if( !getValue( "_id", keyStr  ) )
        keyStr = "";

    strip( keyStr );
    return keyStr;
}

// Set&execute query for document
void TDBSchemaDocument::SetQuery( const DBQueryDef& querydef )
{
    // Set&execute query for document
    TDBDocumentBase::SetQuery( querydef );
    _queryResult->setToSchema(_schemaName);
}

// Set&execute query for document
void TDBSchemaDocument::SetQuery( DBQueryData query, std::vector<std::string>  fieldsList )
{
    if( query.empty()  )
        query = makeDefaultQueryTemplate();

    if( fieldsList.empty()  )
        fieldsList = makeDefaultQueryFields();

    SetQuery( DBQueryDef( fieldsList, query )  );
}
vector<string>  TDBSchemaDocument::makeDefaultQueryFields() const
{
   vector<string> keyFields;
   uint ii;
   ThriftFieldDef*  fldDef;

   ThriftStructDef* strDef = _schema->getStruct(_schemaName);
   if( strDef == nullptr )
       return keyFields;

   if( strDef->getSelectSize() > 0 )
   {
       string fldKey;
       for( ii=0; ii<strDef->getSelectSize(); ii++ )
       {
           fldKey = strDef->keyFldIdName(ii);
           auto fld = _currentData->field(fldKey);
           if(fld!= nullptr )
           {
               keyFields.push_back(fld->getFieldPath());
           }
       }
   }
     else // old type of keys
      {
        for( ii=0; ii<strDef->fields.size(); ii++ )
        {
           fldDef = &strDef->fields[ii];
           if( fldDef->fRequired == fld_required )
               keyFields.push_back(fldDef->fName);
        }
      }
   return keyFields;
}

FieldSetMap TDBSchemaDocument::extractFields( const vector<string> queryFields, const JsonDom* domobj ) const
{
    string valbuf;
    FieldSetMap res;
    for( auto& ent: queryFields )
    {
        if( domobj->findValue( ent, valbuf ) )
            res[ ent] = valbuf;
        else
            res[ ent] = "";
    }
    return res;
}


FieldSetMap TDBSchemaDocument::extractFields( const vector<string> queryFields, const string& jsondata ) const
{
    // temporaly fix
    //shared_ptr<JsonDomSchema> domnode(JsonDomSchema::newSchemaObject( _schemaName ));
    shared_ptr<JsonDomFree> domnode(JsonDomFree::newObject());
    parseJsonToNode( jsondata, domnode.get());
    FieldSetMap res = extractFields( queryFields, domnode.get() );
    return res;
}

// Working with collections
void TDBSchemaDocument::updateQuery()
{
    if( _queryResult.get() == nullptr )
        return;

    shared_ptr<JsonDomSchema> nodedata(JsonDomSchema::newSchemaObject( _schemaName ));
    _queryResult->clearResults();

    SetReadedFunction setfnc = [&]( const string& jsondata )
    {
        parseJsonToNode( jsondata, nodedata.get() );
        string keyStr = getKeyFromDom( nodedata.get() );
        _queryResult->addLine( keyStr,  nodedata.get(), false );
    };

    _collection->selectQuery( _queryResult->getQuery().getQueryCondition(), setfnc );
    // Create a thread using member function
    //std::thread th(&TDBCollection::selectQuery, _collection, _queryResult->getQuery().getQueryCondition(), setfnc );
    //th.detach();
}

} // namespace jsonio
