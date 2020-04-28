//  This is JSONIO library+API (https://bitbucket.org/gems4/jsonio)
//
/// \file dbcollection.cpp
/// Implementation of class class  TDBCollection - definition collections API
//
// JSONIO is a C++ library and API aimed at implementing the interfaces
// for exchanging the structured data between NoSQL database backends,
// JSON/YAML/XML files, and client-server RPC (remote procedure calls).
//
// Copyright (c) 2015-2018 Svetlana Dmytriieva (svd@ciklum.com) and
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
#include <cstring>
#include "jsonio14/jsonbase.h"
#include "jsonio/dbdocument.h"
#include "jsonio/dbconnect.h"
#include "jsonio/jsondomfree.h"
using namespace std;

namespace jsonio {

bool compareToTemplate(  const string& kelm, const string& kpart );
std::string fixTemplateKey( const string& genkey );

/*
 * The key must be a string value.
 * Numeric keys are not allowed, but any numeric value can be put into a string and
 *  can then be used as document key.
 * The key must be at least 1 byte and at most 254 bytes long.
 * Empty keys are disallowed when specified (though it may be valid to completely
 *  omit the _key attribute from a document)
 * It must consist of the letters a-z (lower or upper case), the digits 0-9 or
 *  any of the following punctuation characters: "_-:.@()+,=;$!*'%"
 * Any other characters, especially multi-byte UTF-8 sequences, whitespace or punctuation characters cannot be used inside key values
 */

const string  KeyPunctuationCharacters = "_-:.@()+,=;$!*'";

std::string fixTemplateKey( const string& genkey )
{
  string truekey;

  for(uint ii=0; ii<genkey.length(); ii++ )
  {
    auto c = genkey[ii];
    if( isalnum(c) || ( KeyPunctuationCharacters.find(c) != string::npos ) )
        truekey +=c;
  }

  return truekey;
}

std::string makeTemplateKey( const JsonDom *object, const std::vector<std::string>&  keyTemplateFields )
{
  std::string keyTemlpate = "";
  jsonioErrIf( !object->isTop(), "makeTemplateKey", "Illegal function on level");

  for(auto const &keyfld : keyTemplateFields)
  {
    auto fld = object->field(keyfld);
    if(fld!= nullptr )
    {
        if( !keyTemlpate.empty() )
          keyTemlpate +=";"; // delimiter for generated key
        auto flddata = fld->toString();
        keyTemlpate += fixTemplateKey(flddata);
    }
  }
  if( keyTemlpate.length() > 240 )
     keyTemlpate.resize( 240 );    // other symbols to number
  return keyTemlpate;
}

TAbstractDBDriver::~TAbstractDBDriver()
{ }

//-------------------------------------------------------------
// TDBCollection - This class contains the structure of Data Base Collection
//-------------------------------------------------------------


// Default configuration of the Data Base
TDBCollection::TDBCollection(  const TDataBase* adbconnect,
                               const std::string& name  ):
    _name( name ), _dbconnect(adbconnect), _dbdriver(adbconnect->theDriver())
{ }


// Configurator by readind from bson object
TDBCollection::TDBCollection( const TDataBase* adbconnect, const JsonDom *object ):
    _dbconnect(adbconnect), _dbdriver(adbconnect->theDriver())
{
    fromJsonNode(object);
}

// Open collection file and build linked record list
void TDBCollection::Load()
{
    _recList.clear();
    _itrL= _recList.end();
    loadCollection();
}

// Close file, clear key list
void TDBCollection::Close()
{
    closeCollection();
    _recList.clear();
    _itrL= _recList.end();
}

// Writes data to bson
void TDBCollection::toJsonNode( JsonDom *object ) const
{
    object->appendString( "CollectionType", _coltype );
    object->appendString( "CollectionName", _name );
}

// Reads data from bson
void TDBCollection::fromJsonNode( const JsonDom *object )
{
    if(!object->findValue( "CollectionName", _name ) )
        jsonioErr( "E009BSon: ", "Undefined collection name.");
    if(!object->findValue( "CollectionType", _coltype ) )
        jsonioErr( "E008BSon: ", "Undefined collection type.");
}

// Reconnect DataBase (collection)
void TDBCollection::changeDriver( TAbstractDBDriver* const adriver )
{
    Close();
    _dbdriver = adriver;
    Load();
    for( auto doc: _documents )
        doc->updateQuery(); // Run current query, rebuild internal table of values
}

// Reconnect DataBase (collection)
void TDBCollection::reloadCollection()
{
    Close();
    Load();
    for( auto doc: _documents )
        doc->updateQuery(); // Run current query, rebuild internal table of values
}


// Seach record index with key pkey.
bool TDBCollection::Find( const std::string& pkey )
{
    if(_recList.empty() )
        return false;

    _itrL = _recList.find( pkey );

    if( _itrL == _recList.end() )
        return  false;
    else
        return true;
}

// Retrive one record from the collection
void TDBCollection::Read( TDBDocumentBase* document, const std::string& pkey )
{
    if( isPattern( pkey ) )
        jsonioErr("TEJDB0010", "Cannot get under record key template" );

    _itrL = _recList.find( pkey );
    if( _itrL == _recList.end() )
    {
        string erstr = pkey;
        erstr += "\nrecord to retrive does not exist!";
        jsonioErr("TEJDB0001", erstr );
    }

    // Read record from DB to bson
    string jsondata;
    if( !_dbdriver->loadRecord( name(), _itrL, jsondata ))
        jsonioErr( "TEJDB0025",  string("Error Loading record ") + pkey );

    // Save json structure to internal arrays
    document->recFromJson( jsondata );
}

// Removes record from the collection
void TDBCollection::Delete( const std::string& pkey )
{
    if( isPattern( pkey ) )
        jsonioErr("TEJDB0010", "Cannot delete under record key template" );

    _itrL = _recList.find( pkey );
    if( _itrL == _recList.end() )
    {
        string erstr = pkey;
        erstr+= "\nrecord to delete does not exist!";
        jsonioErr("TEJDB0002", erstr );
    }

    // Remove BSON object from collection.
    if( !_dbdriver->removeRecord(name(), _itrL ) )
        jsonioErr( "TEJDB0024", string("Error deleting of record ") + pkey );
    // Set up internal data
    _recList.erase(_itrL);
    _itrL = _recList.end();
    for( auto itDoc:  _documents)
        itDoc->deleteLine( pkey );
}

// Save new record in the collection
string TDBCollection::Create( TDBDocumentBase* document )
{
    // Get bson structure from internal data
    auto domnode = document->recToSave( time(nullptr), nullptr );

    //if( !pkey ) protect different data into bsrec and key
    string __key = getKeyFromDom( domnode );
    if( __key.empty() )
        __key = "undef";
    if( isPattern( __key ) )
        jsonioErr("TEJDB0010", "Cannot save under record key template" );

    pair<KeysSet::iterator,bool> ret;
    ret = _recList.insert( pair<string,unique_ptr<char>>( __key, nullptr ) );
    _itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot insert record:\n";
        erstr += __key;
        erstr += ".\nTwo records with the same key!";
        jsonioErr("TEJDB0004", erstr );
    }

    try{
        // save record to data base
        string jsondata;
        printNodeToJson( jsondata, domnode, true );
        string retId = _dbdriver->saveRecord( name(), _itrL, jsondata );
        if( retId.empty() )
            jsonioErr( "TEJDB0021",  string("Error saving record ") );
        document->setOid( retId );
    }
    catch(...)
    {
        _recList.erase(_itrL);
        _itrL = _recList.end();
        throw;
    }

    // Set up internal data - only delete all, not add to selection
    string keyStr = getKeyFromDom( domnode );

    if(keyStr != __key )
    {
        // Swap value from oldKey to newKey, note that a default constructed value
        // is created by operator[] if 'm' does not contain newKey.
        std::swap(_recList[keyStr], _itrL->second);
        // Erase old key-value from map
        _recList.erase(_itrL);
        __key = keyStr;
    }
    // for( auto itDoc:  _documents)
    document->addLine( keyStr, domnode, false );
    return keyStr;
}

// Save/update record in the collection
string TDBCollection::Update( TDBDocumentBase* document, const std::string& pkey )
{
    // Try to insert new record to list
    if( isPattern( pkey ) )
        jsonioErr("TEJDB0011", "Cannot save under record key template" );

    pair<KeysSet::iterator,bool> ret;
    ret = _recList.insert( pair<string,unique_ptr<char>>( pkey, nullptr ) );
    _itrL = ret.first;

    // Get bson structure from internal data
    auto domnode = document->recToSave( time(nullptr), _itrL->second.get() );
    try{
        // save record to data base
        string jsondata;
        printNodeToJson( jsondata, domnode, true );
        string retId = _dbdriver->saveRecord( name(), _itrL, jsondata );
        if( retId.empty() )
            jsonioErr( "TEJDB0022",  string("Error saving record ") );
        document->setOid( retId );
    }
    catch(...)
    {
        if( ret.second == true )
        {
            _recList.erase(_itrL);
            _itrL = _recList.end();
        }
        throw;
    }
    // Set up internal data ( only for current document )
    string keyStr = getKeyFromDom( domnode );
    for( auto itDoc:  _documents)
        if( itDoc != document )
            itDoc->updateLine( keyStr, domnode );
    document->addLine( keyStr, domnode, true );
    return keyStr;
}

//-----------------------------------------------------------------

std::size_t TDBCollection::GetIdsList( const string& idpart, set<string>& aKeyList )
{
    aKeyList.clear();
    KeysSet::iterator it  = _recList.begin();
    while( it != _recList.end() )
    {
        if( it->second.get() != nullptr )
            if( compareToTemplate(  string(it->second.get()), idpart ) )
                aKeyList.insert( string(it->second.get()) );
        it++;
    }
    return aKeyList.size();
}

std::vector<std::string> TDBCollection::idsFromRecordKeys( const std::vector<std::string>& rkeys )
{
    std::vector<std::string> ids;

    for( auto rkey: rkeys )
    {
        if( isPattern( rkey ) )
            continue;
        auto  itrL = _recList.find( rkey );
        if( itrL != _recList.end() )
            ids.push_back( _dbdriver->getServerKey( itrL->second.get() ) );
    }
    return ids;
}

// Working with collections
void TDBCollection::loadCollectionFile(  const set<string>& queryFields )
{
    SetReadedFunctionKey setfnc = [=]( const string& jsondata, const string& keydata )
    {
        listKeyFromDom( jsondata, keydata );
    };

    _dbdriver->allQuery( name(), queryFields, setfnc );
}

// Load records key from bson structure
void TDBCollection::listKeyFromDom( const string& jsondata, const string& keydata )
{
    // Get key of record
    shared_ptr<JsonDomFree> domdata(JsonDomFree::newObject());
    parseJsonToNode( jsondata, domdata.get());
    string keyStr = getKeyFromDom( domdata.get() );
    if( isPattern( keyStr ) )
        jsonioErr("TGRDB0005", "Cannot save under record key template" );

    // Try add key to list
    char *oidhex = new char[keydata.length()+1];
    strcpy(oidhex, keydata.c_str() );
    pair<KeysSet::iterator,bool> ret;
    ret = _recList.insert( pair<string,unique_ptr<char> >( keyStr, unique_ptr<char>(oidhex) ) );
    _itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot add new record:\n";
        erstr += keyStr;
        erstr += ".\nTwo records with the same key!";
        jsonioErr("TGRDB0006", erstr );
    }
}

//-----------------------------------------------------------------


// Check if pattern/undefined in record key
bool TDBCollection::isPattern( const string& akey )
{
    if( akey.empty() || akey.find_first_of("*?") != std::string::npos )
        return true;
    return false;
}

set<string> TDBCollection::listKeyFields()
{
    set<string> queryFields;
    queryFields.insert( "_id" ); // always _id as internal
    return queryFields;
}

// Gen new oid from key template
string TDBCollection::genKeyFromTemplate( const string& _keytemplate )
{
    string idkey = name();
    idkey +="/"+_keytemplate+"*";
    set<string> aKeyList;
    auto iRec = GetIdsList( idkey, aKeyList );
    if(iRec==0)
        return _keytemplate;
    int ii=0;
    string keydata = string(name())+"/"+_keytemplate+"_";
    while( aKeyList.find( keydata+to_string(ii) ) != aKeyList.end() )
        ii++;
    return _keytemplate+"_"+to_string(ii);
}

// Get current record key from bson structure
string TDBCollection::getKeyFromDom( const JsonDom* domdata )
{
    //get key from object
    string keyStr = "";
    if( !domdata->findKey( "_id", keyStr ) )
        keyStr = "undef";
    return keyStr;
}

//-------------------------------------------------------------------------


bool compareToTemplate(  const string& kelm, const string& kpart )
{
    if( kpart == "*" )
        return true;

    auto rklen = kpart.length();
    if( kpart[rklen-1]=='*' )
        rklen--;
    if(  rklen > kelm.length() )
        return false;

    rklen = kpart.length();
    uint i;
    for(i=0; i<rklen; i++ )
    {
        if( kpart[i] == '*' ) // next field
            break;
        switch( kpart[i] )
        {
        case '?': // no ' '
            if( kelm[i] == ' ' )
                return false;
            break;
        default:
            if( kpart[i] != kelm[i] )
                return false;
        }
    }
    if( kpart[i] != '*' && i < kelm.length())
        return false;
    return true;
}


} // namespace jsonio

void jsonio14::DBCollection::selectQuery(const jsonio14::DBQueryBase &query, jsonio14::SetReadedFunction setfnc)
{
    _dbdriver->selectQuery( name(), query, setfnc );
}
