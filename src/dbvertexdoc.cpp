
#include <chrono>
#include "jsonio14/dbvertexdoc.h"
#include "jsonio14/io_settings.h"
#include "jsonio14/service.h"

namespace jsonio14 {

// TGraphAbstract ---------------------------------------------


//string collectionNameFromSchema( const string& schemaName )
//{
//    string labelVertex = ioSettings().Schema()->getVertexCollection( schemaName );
//    if(!labelVertex.empty())
//        return labelVertex;

//    labelVertex = ioSettings().Schema()->getEdgeCollection( schemaName );
//    if(!labelVertex.empty())
//        return labelVertex;
//    return ioSettings().collection();
//}

DBQueryBase DBVertexDocument::edgesQuery(const std::string& id_document, DBQueryBase::QType query_type, const std::string& edge_collections)
{
    std::string queryJson = "{ \"startVertex\": \""
            + id_document  + "\", \"edgeCollections\": \""
            + edge_collections + "\" }";
    return DBQueryBase( queryJson, query_type);
}


DBVertexDocument *DBVertexDocument::newVertexDocumentQuery(const DataBase &dbconnect, const std::string &aschema_name, DBQueryBase &&query)
{
    if( aschema_name.empty()  )
        return nullptr;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    DBVertexDocument* new_document = new DBVertexDocument( aschema_name, dbconnect );
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    // init internal selection block
    new_document->setQuery( std::move(query));
    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();

    std::cout << aschema_name <<  " Loding collection " << duration1 <<  " Loding query " << duration3 << std::endl;

    return new_document;
}

DBVertexDocument *DBVertexDocument::newVertexDocument(const DataBase &dbconnect, const std::string &aschema_name)
{
    if( aschema_name.empty()  )
        return nullptr;

    return new DBVertexDocument( aschema_name, dbconnect );
}









//--------------------------------------------------------------------------------



// Change base collections
void DBVertexDocument::update_collection( const std::string& aschemaName )
{
    auto oldName = collection_from->name();
    auto newName = collectionNameFromSchema( aschemaName );
    if( newName != oldName )
    {
        collection_from->eraseDocument(this);
        collection_from = database()->getCollection( type(), newName  );
        collection_from->addDocument(this);
    }
}

// Test true type and label for schema
void DBVertexDocument::test_schema(const std::string &jsondata)
{
    string newtype = extractStringField( "_type", jsondata );
    string newlabel = extractStringField( "_label", jsondata );

    if( newtype.empty() || newlabel.empty() )
        return;

    if( _changeSchemaMode )
    {
        string newSchema;
        if( newtype == "edge")
            newSchema = _schema->getEdgeName(newlabel);
        else
            if( newtype == "vertex")
                newSchema = _schema->getVertexName(newlabel);
        jsonioErrIf(newSchema.empty(),  "TGRDB0007",
                    "Undefined record: " + newtype + " type " + newlabel + " label" );
        resetSchema( newSchema, false );
    } else
    {
        if( newtype != _type || newlabel != _label )
            jsonioErr("TGRDB0004", "Illegal record type: " + newtype + " or label: " + newlabel );
    }
}



void TDBVertexDocument::resetSchema( const string& aschemaName, bool change_queries )
{
    if( _schemaName != aschemaName || change_queries  )
    {
        _schemaName = aschemaName;

        _currentData.reset( JsonDomSchema::newSchemaObject( _schemaName ) );
        auto type_node = _currentData->field("_type");
        _type = "";
        if( type_node != nullptr )
            type_node->getValue(_type);
        jsonioErrIf( _type != type(), "TGRDB0004", "Illegal record type: " + _type );

        type_node = _currentData->field("_label");
        _label = "";
        if( type_node != nullptr )
            type_node->getValue(_label);

        // update collection
        updateCollection(  aschemaName );
    }
    if( change_queries && _queryResult.get() != nullptr  )
    {
       // new one
       //SetQuery( makeDefaultQueryTemplate(), makeDefaultQueryFields());
       // old one
       _queryResult->setFieldsCollect(makeDefaultQueryFields());
       _queryResult->setQueryCondition(makeDefaultQueryTemplate());
       _queryResult->setToSchema(_schemaName);
    }

    // load unique map
    loadUniqueFields();
}


void TDBVertexDocument::testUpdateSchema( const std::string&  pkey )
{
    string _id = pkey;
    // _id.pop_back(); // delete ":"
    string schemaName = extractSchemaFromId( _id  );
    if( !schemaName.empty() && schemaName != getSchemaName() )
        resetSchema(schemaName, false );
}

void TDBVertexDocument::loadUniqueFields()
{
    uniqueFieldsNames.clear();
    uniqueFieldsValues.clear();

    ThriftStructDef* strDef = _schema->getStruct(_schemaName);
    if( strDef == nullptr )
        return;

    /// temporaly block unique test
    //uniqueFieldsNames = strDef->getUniqueList();
    if( uniqueFieldsNames.empty() )
        return;

    uniqueFieldsNames.push_back("_id");
    ValuesTable uniqueVal = downloadDocuments( makeDefaultQueryTemplate(), uniqueFieldsNames);

    for( auto row: uniqueVal)
    {
        auto idkey = row.back();
        row.pop_back();

        if( !uniqueFieldsValues.insert( std::pair< vector<string>, string>(row, idkey)).second )
        {
            cout << "Not unique values: " << string_from_vector( row ) << endl;
        }
    }
}



/// Delete all edges by vertex id
void TDBVertexDocument::beforeRm( const std::string& key )
{
    string  _vertexid = key;

    /*string bsonquery = "{  \"_type\": \"edge\",  \"$or\": [ { \"_to\":  \"";
           bsonquery += _vertexid + "\" }, { \"_from\": \"";
           bsonquery += _vertexid + "\" } ], \"$dropall\": true }";
    DBQueryData  query( bsonquery, DBQueryData::qEJDB );*/

    shared_ptr<TDBEdgeDocument> edges( documentAllEdges( this->database() ));
    auto query = edges->allEdgesQuery( _vertexid );
    auto edgekeys =  edges->getKeysByQuery( query );

    for( auto idedge: edgekeys )
    {
      //cout << idedge << endl;
      edges->testUpdateSchema( idedge );
      edges->Delete(idedge);
    }

   // very slow
   // _collection->deleteEdges( _vertexid );
}

void TDBVertexDocument::afterRm( const std::string& key )
{

    // delete from unique map
    if( !uniqueFieldsNames.empty() )
    {
        string  _vertexid = key;
        //strip_all( _vertexid, ":" ); // get id from key
        //_vertexid =_vertexid.substr(0, _vertexid.size()-1); // delete ":"
        auto itrow = uniqueLinebyId( _vertexid );
        if( itrow != uniqueFieldsValues.end() )
            uniqueFieldsValues.erase(itrow);
    }
}

UniqueFieldsMap::iterator TDBVertexDocument::uniqueLinebyId( const string& idschem )
{
    UniqueFieldsMap::iterator itrow = uniqueFieldsValues.begin();
    while( itrow != uniqueFieldsValues.end() )
    {
        if( itrow->second == idschem )
            break;
        itrow++;
    }
    return itrow;
}

void TDBVertexDocument::beforeSaveUpdate( const std::string&  )
{
    if( !uniqueFieldsNames.empty() )
    {
        FieldSetMap uniqfields = extractFields( uniqueFieldsNames, _currentData.get() );

        vector<string> uniqValues;
        for( uint ii=0; ii<uniqueFieldsNames.size()-1; ii++ )
            uniqValues.push_back( uniqfields[uniqueFieldsNames[ii]] );

        auto itfind = uniqueFieldsValues.find(uniqValues);
        if( itfind != uniqueFieldsValues.end() )
        {
            if( itfind->second != uniqfields["_id"] )
                jsonioErr("TGRDB0009", "Not unique values: " + string_from_vector( uniqValues ) );
        }
    }
}

void TDBVertexDocument::afterSaveUpdate( const std::string&  )
{
    if( !uniqueFieldsNames.empty() )
    {
        FieldSetMap uniqfields = extractFields( uniqueFieldsNames, _currentData.get() );

        // delete old
        auto itrow = uniqueLinebyId( uniqfields["_id"] );
        if( itrow != uniqueFieldsValues.end() )
            uniqueFieldsValues.erase(itrow);

        // insert new
        vector<string> uniqValues;
        for( uint ii=0; ii<uniqueFieldsNames.size()-1; ii++ )
            uniqValues.push_back( uniqfields[uniqueFieldsNames[ii]] );
        if( !uniqueFieldsValues.insert( std::pair< vector<string>, string>(uniqValues, uniqfields["_id"])).second )
        {
            cout << "Not unique values: " << string_from_vector( uniqValues ) << endl;
        }
    }
}


// Extract label by id (old function )
string TDBVertexDocument::extractLabelById( const string& id )
{
    string token;

    auto query =  idQuery( id );
    vector<string> queryFields = { "_label"};
    query.setQueryFields( queryFields );
    vector<string> resultData = runQuery( query );
    if( resultData.size()>0  )
        token = extractStringField( "_label", resultData[0] );

    return token;
}

// Extract label from id
string TDBVertexDocument::extractLabelFromId( const string& id )
{
    string token;
    queue<string> names = split(id, "/");
    if(names.size()>1)
        token = names.front();
    return token;
}


// build functions ------------------------------------------------------------

// Define new Vertex
void TDBVertexDocument::setVertexObject( const string& aschemaName, const FieldSetMap& fldvalues )
{
    // check schema
    if( _schemaName != aschemaName )
    {
        if( _changeSchemaMode )
            resetSchema( aschemaName, false );
        else
            jsonioErr("TGRDB0007", "Illegal record schame: " + aschemaName + " current schema: " + _schemaName );
    }
    _currentData->clearField(); // set default values

    for(auto const &ent : fldvalues)
        setValue( ent.first, ent.second  );
}

// Update current schema data
void TDBVertexDocument::updateVertexObject( const string& aschemaName, const FieldSetMap& fldvalues )
{
    // check schema
    if( _schemaName != aschemaName )
        jsonioErr("TGRDB0008", "Illegal record schame: " + aschemaName + " current schema: " + _schemaName );
    for(auto const &ent : fldvalues)
        setValue( ent.first, ent.second  );
}



FieldSetMap TDBVertexDocument::loadRecordFields( const string& id, const vector<string>& queryFields )
{
    Read( id/*+":"*/ );
    FieldSetMap fldsValues = extractFields( queryFields, _currentData.get() );
    return fldsValues;
}




void DBVertexDocument::testUpdateSchema(const std::string &pkey)
{

}

void DBVertexDocument::resetSchema(const std::string &aschemaName, bool change_queries)
{

}

//// Extract the string value by key from query
std::string extractStringField( const std::string& key, const std::string& jsondata )
{
    std::string token = "";
    std::string query2 =  replace_all( jsondata, "\'", '\"');
    std::string regstr =  string(".*\"")+key+"\"\\s*:\\s*\"([^\"]*)\".*";
    std::regex re( regstr );
    std::smatch match;

    if( std::regex_search( query2, match, re ))
    {
        if (match.ready())
            token = match[1];
    }
    //cout << key << "  token " << token  << endl;
    return token;
}

} // namespace jsonio14
