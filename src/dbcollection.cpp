#include "jsonio14/jsonbase.h"
#include "jsonio14/dbdocument.h"
#include "jsonio14/dbconnect.h"
#include "jsonio14/jsonfree.h"

namespace jsonio14 {

bool compare_to_template(  const std::string& kelm, const std::string& kpart );
std::string fix_template_key( const std::string& genkey );

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

const std::string  KeyPunctuationCharacters = "_-:.@()+,=;$!*'";

std::string fix_template_key( const std::string& genkey )
{
    std::string legal_key;
    for( const auto c:genkey )
    {
        if( isalnum(c) || ( KeyPunctuationCharacters.find(c) != std::string::npos ) )
            legal_key +=c;
    }
    return legal_key;
}

//std::string makeTemplateKey( const JsonDom *object, const std::vector<std::string>&  keyTemplateFields )
//{
//  std::string keyTemlpate = "";
//  jsonioErrIf( !object->isTop(), "makeTemplateKey", "Illegal function on level");

//  for(auto const &keyfld : keyTemplateFields)
//  {
//    auto fld = object->field(keyfld);
//    if(fld!= nullptr )
//    {
//        if( !keyTemlpate.empty() )
//          keyTemlpate +=";"; // delimiter for generated key
//        auto flddata = fld->toString();
//        keyTemlpate += fixTemplateKey(flddata);
//    }
//  }
//  if( keyTemlpate.length() > 240 )
//     keyTemlpate.resize( 240 );    // other symbols to number
//  return keyTemlpate;
//}


//-------------------------------------------------------------
// TDBCollection - This class contains the structure of Data Base Collection
//-------------------------------------------------------------


// Default configuration of the Data Base
DBCollection::DBCollection(  const DataBase* adbconnect,
                               const std::string& name  ):
    coll_name( name ), db_connect(adbconnect), db_driver( adbconnect->theDriver() )
{ }


// Open collection file and build linked record list
void DBCollection::load()
{
    key_record_map.clear();
    key_record_itr= key_record_map.end();
    loadCollection();
}

// Close file, clear key list
void DBCollection::close()
{
    closeCollection();
    key_record_map.clear();
    key_record_itr= key_record_map.end();
}


// Reconnect DataBase (collection)
void DBCollection::reload()
{
    close();
    load();
    for( auto doc: documents_list )
        doc->updateQuery(); // Run current query, rebuild internal table of values
}

std::string DBCollection::extract_key_from(const JsonBase& object)
{
    std::string key_str, kbuf;
    for( const auto& keyfld: keyFields())
    {
        object.get_key_via_path( keyfld, kbuf, "undef" );
        key_str += kbuf;
    }
    return key_str;
}

std::string DBCollection::id_from_template( const std::string& key_template ) const
{
    //temporaly
    if( key_template.empty())
        return "";
    return coll_name+"/"+key_from_template(key_template);
    //return db_driver->genOid( name(), genKeyFromTemplate(_keytemplate) );
}

bool DBCollection::existsDocument(const std::string &key) const
{
    return  key_record_map.find(key)  != key_record_map.end();
}

std::string DBCollection::createDocument( const JsonBase &data_object )
{
    auto new_key = extract_key_from( data_object );

    if( is_pattern( new_key ) )
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

}

bool DBCollection::readDocument(JsonBase &data_object, const std::string &key)
{

}



std::string DBCollection::create_new_record_in_db(DBDocumentBase *document)
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

void DBCollection::read_record_from_db(DBDocumentBase *document, const std::string &key)
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

std::string DBCollection::save_record_to_db(DBDocumentBase *document, const std::string &key)
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

void DBCollection::delete_record_from_db(const std::string &key)
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


//-----------------------------------------------------------------

// Reconnect DataBase (collection)
void DBCollection::change_driver( AbstractDBDriver* const adriver )
{
    close();
    db_driver = adriver;
    load();
    for( auto doc: documents_list )
        doc->updateQuery(); // Run current query, rebuild internal table of values
}

void DBCollection::loadCollectionFile(  const std::set<std::string>& query_fields )
{
    SetReadedKey_f setfnc = [=]( const std::string& jsondata, const std::string& keydata )
    {
        add_record_to_map( jsondata, keydata );
    };
    db_driver->all_query( name(), query_fields, setfnc );
}


// Gen new oid from key template
std::string DBCollection::key_from_template( const std::string& key_template ) const
{
    std::string idkey = name();
    idkey +="/"+key_template+"*";

    auto ids_list = get_ids_as_template( idkey );
    if( ids_list.empty() )
        return key_template;

    int ii=0;
    auto keydata = name()+"/"+key_template+"_";
    while( ids_list.find( keydata+std::to_string(ii) ) != ids_list.end() )
        ii++;
    return key_template+"_"+std::to_string(ii);
}


void DBCollection::add_record_to_map( const std::string& jsondata, const std::string& keydata )
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


// Check if pattern/undefined in record key
bool DBCollection::is_pattern( const std::string& akey ) const
{
    if( akey.empty() || akey.find_first_of("*?") != std::string::npos )
        return true;
    return false;
}

std::vector<std::string> DBCollection::ids_from_keys(const std::vector<std::string> &rkeys) const
{
    std::vector<std::string> ids;

    for( auto rkey: rkeys )
    {
        if( is_pattern( rkey ) )
            continue;
        auto  itrL = key_record_map.find( rkey );
        if( itrL != key_record_map.end() )
            ids.push_back( db_driver->get_server_key( itrL->second.get() ) );
    }
    return ids;
}


std::set<std::string> DBCollection::get_ids_as_template( const std::string& idpart ) const
{
    std::set<std::string> ids_list;
    for( const auto& it: key_record_map )
    {
        if( it.second.get() != nullptr )
            if( compare_to_template(  it.second.get(), idpart ) )
                ids_list.insert( it.second.get() );
    }
    return ids_list;
}

//-------------------------------------------------------------------------

bool compare_to_template(  const std::string& kelm, const std::string& kpart )
{
    if( kpart == "*" )
        return true;

    auto rklen = kpart.length();
    if( kpart[rklen-1]=='*' )
        rklen--;
    if(  rklen > kelm.length() )
        return false;

    rklen = kpart.length();
    size_t i;
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


} // namespace jsonio14

