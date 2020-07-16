
#include <chrono>
#include <jsonio17/dbedgedoc.h>
#include "jsonio17/dbconnect.h"
#include "jsonio17/io_settings.h"
#include "jsonio17/service.h"
#include "jsonio17/jsondump.h"

namespace jsonio17 {


std::string collectionNameFromSchema( const std::string& aschema_name )
{
    std::string labelVertex = DataBase::getVertexCollection( aschema_name );
    if(!labelVertex.empty())
        return labelVertex;

    labelVertex = DataBase::getEdgeCollection( aschema_name );
    if(!labelVertex.empty())
        return labelVertex;
    return "collection";
}


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


void DBVertexDocument::resetSchema( const std::string &aschema_name, bool change_queries )
{
    if( schema_name != aschema_name || change_queries  )
    {
        schema_name = aschema_name;
        current_schema_object = JsonSchema::object(schema_name);
        current_schema_object.get_value_via_path("_label", object_label, std::string("") );
        current_schema_object.get_value_via_path("_type", object_type, std::string("") );

        JSONIO_THROW_IF( object_type != type(), "DBVertexDocument", 11,
                          " illegal record type:" + object_type );
        // update collection
        update_collection(  aschema_name );
        load_unique_fields();
    }
    if( change_queries && query_result.get() != nullptr  )
    {
       // set query and reload  query_result
       setQuery(  make_default_query_template(), make_default_query_fields() );
    }
}

void DBVertexDocument::setVertexObject(const std::string &aschema_name, const field_value_map_t &fldvalues)
{
    if( schema_name != aschema_name )
    {
        if( change_schema_mode )
            resetSchema( aschema_name, false );
        else
            JSONIO_THROW( "DBVertexDocument", 12,
                           " illegal record schame: " + aschema_name + " current schema: " + schema_name );
    }
    current_schema_object.clear(); // set default values

    for(auto const &ent : fldvalues)
        current_schema_object.set_value_via_path( ent.first, ent.second  );
}

void DBVertexDocument::updateVertexObject(const std::string &aschema_name, const field_value_map_t &fldvalues)
{
    // check schema
    if( schema_name != aschema_name )
        JSONIO_THROW( "DBVertexDocument", 13,
                       " illegal record schame when update: " + aschema_name + " current schema: " + schema_name );
    for(auto const &ent : fldvalues)
        current_schema_object.set_value_via_path( ent.first, ent.second  );
}

std::string DBVertexDocument::extractSchemaFromId( const std::string& oid )
{
    auto names = split( oid, "/" );
    if( names.size()>1 )
    {
       auto  label = names.front();
       label.pop_back();   // delete last "s"
       return DataBase::getVertexName( label );
    }
    return "";
}


void DBVertexDocument::before_load(const std::string & key)
{
    auto schemaName = extractSchemaFromId( key  );
    if( !schemaName.empty() && schemaName != getSchemaName() )
        resetSchema(schemaName, false );
}

void DBVertexDocument::before_remove(const std::string & vertex_id )
{
    /*string bsonquery = "{  \"_type\": \"edge\",  \"$or\": [ { \"_to\":  \"";
           bsonquery += _vertexid + "\" }, { \"_from\": \"";
           bsonquery += _vertexid + "\" } ], \"$dropall\": true }";
    DBQueryData  query( bsonquery, DBQueryData::qEJDB );*/

    std::shared_ptr<DBEdgeDocument> edges( documentAllEdges( collection_from->db_connect ));
    auto edgekeys =  getKeysByQuery( allEdgesQuery( vertex_id ) );
    for( auto idedge: edgekeys )
    {
        //cout << idedge << endl;
        edges->resetSchema( edges->extractSchemaFromId(idedge), false );
        edges->deleteDocument(idedge);
    }

    // very slow
    // collection_from->deleteEdges( vertex_id );
}

unique_fields_map_t::iterator DBVertexDocument::unique_line_by_id( const std::string& idschem )
{
    unique_fields_map_t::iterator itrow = unique_fields_values.begin();
    while( itrow != unique_fields_values.end() )
    {
        if( itrow->second == idschem )
            break;
        itrow++;
    }
    return itrow;
}

void DBVertexDocument::after_remove(const std::string & vertex_id)
{
    // delete from unique map
    if( !unique_fields_names.empty() )
    {
        auto itr = unique_line_by_id( vertex_id );
        if( itr != unique_fields_values.end() )
            unique_fields_values.erase(itr);
    }
}

void DBVertexDocument::before_save_update(std::string & key)
{
    // generate key if empty
    DBSchemaDocument::before_save_update(key);

    if( !unique_fields_names.empty() )
    {
        auto uniq_fields = extract_fields( unique_fields_names, current_schema_object );

        values_t uniq_values;
        for( size_t ii=0; ii<unique_fields_names.size()-1; ii++ )
            uniq_values.push_back( uniq_fields[unique_fields_names[ii]] );

        auto itfind = unique_fields_values.find( uniq_values );
        if( itfind != unique_fields_values.end() )
        {
            if( itfind->second != uniq_fields["_id"] )
                JSONIO_THROW( "DBVertexDocument", 14, " not unique values: " + json::dump( uniq_values ) );
        }
    }
}

void DBVertexDocument::after_save_update(const std::string &)
{
    if( !unique_fields_names.empty() )
    {
        auto uniq_fields = extract_fields( unique_fields_names, current_schema_object );

        // delete old
        auto itrow = unique_line_by_id( uniq_fields["_id"] );
        if( itrow != unique_fields_values.end() )
            unique_fields_values.erase(itrow);

        values_t uniq_values;
        for( size_t ii=0; ii<unique_fields_names.size()-1; ii++ )
            uniq_values.push_back( uniq_fields[unique_fields_names[ii]] );

        if( !unique_fields_values.insert( std::pair< values_t, std::string>(uniq_values, uniq_fields["_id"])).second )
        {
            std::cout << "Not unique values: " << json::dump( uniq_values ) << std::endl;
        }
    }
}


void DBVertexDocument::load_unique_fields()
{
    unique_fields_names.clear();
    unique_fields_values.clear();

    auto schema_struct = ioSettings().Schema().getStruct( schema_name );
    if( schema_struct == nullptr )
        return;
    /// temporaly block unique test
    //unique_fields_names = schema_struct->getUniqueList();
    if( unique_fields_names.empty() )
        return;

    unique_fields_names.push_back("_id");
    auto uniqueVal = downloadDocuments( make_default_query_template(), unique_fields_names);

    for( auto row: uniqueVal)
    {
        auto idkey = row.back();
        row.pop_back();

        if( !unique_fields_values.insert( std::pair< values_t, std::string>(row, idkey)).second )
        {
            std::cout << "Not unique values: " << json::dump( row ) << std::endl;
        }
    }
}


// Change base collections
void DBVertexDocument::update_collection( const std::string& aschema_name )
{
    auto oldName = collection_from->name();
    auto newName = collectionNameFromSchema( aschema_name );
    if( newName != oldName )
    {
        collection_from->eraseDocument(this);
        collection_from = collection_from->db_connect.collection( newName, type() );
        collection_from->addDocument(this);
    }
}

// Test true type and label for schema
void DBVertexDocument::test_schema( const std::string &jsondata )
{
    auto newtype = extract_string_json( "_type", jsondata );
    auto newlabel = extract_string_json( "_label", jsondata );
    if( newtype.empty() || newlabel.empty() )
        return;

    if( change_schema_mode )
    {
        std::string newschema;
        if( newtype == "edge")
            newschema = DataBase::getEdgeName(newlabel);
        else if( newtype == "vertex")
            newschema = DataBase::getVertexName(newlabel);

        JSONIO_THROW_IF( newschema.empty(), "DBVertexDocument", 15,
                          " undefined record type: " + newtype + " or label: " + newlabel );
        resetSchema( newschema, false );
    }
    else
    {
        if( newtype != object_type || newlabel != object_label )
            JSONIO_THROW( "DBVertexDocument", 16, " illegal record type: " + newtype + " or label: " + newlabel );
    }
}

} // namespace jsonio17
