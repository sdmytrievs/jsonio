

/* query all connected edges
FOR v, e, p IN 1..1 ANY 'reactions/H2_15_0'
 inherits, takes, defines, master, product, prodreac, basis, pulls, involves, adds, yields
RETURN e

*/

#include "jsonio/dbedgedoc.h"
#include "jsonio/dbconnect.h"
#include "jsonio/io_settings.h"


namespace jsonio {


DBEdgeDocument* documentAllEdges( const DataBase& dbconnect )
{
    auto schemaName = DataBase::getEdgesList()[0];
    DBEdgeDocument* edge_document = new DBEdgeDocument( schemaName, dbconnect  );
    edge_document->setMode(true);
    return edge_document;
}


DBEdgeDocument *DBEdgeDocument::newEdgeDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name,
                                                      const DBQueryBase& query)
{
    if( aschema_name.empty()  )
        return nullptr;

    auto new_document = new DBEdgeDocument( aschema_name, dbconnect );
    // internal selection
    new_document->setQuery( query );
    return new_document;
}

DBEdgeDocument *DBEdgeDocument::newEdgeDocument( const DataBase& dbconnect, const std::string& aschema_name)
{
    if( aschema_name.empty()  )
        return nullptr;

    return new DBEdgeDocument( aschema_name, dbconnect );
}


void DBEdgeDocument::setEdgeObject( const std::string &aschema_name, const std::string &outV,
                                    const std::string &inV, const field_value_map_t &fldvalues)
{
    if( schema_name != aschema_name )
    {
        if( change_schema_mode )
            resetSchema( aschema_name, false );
        else
            JSONIO_THROW( "DBEdgeDocument", 10,
                           " illegal record schame: " + aschema_name + " current schema: " + schema_name );
    }
    current_schema_object.clear(); // set default values
    current_schema_object.set_value_via_path("_to", inV);
    current_schema_object.set_value_via_path("_from", outV);

    for(auto const &ent : fldvalues)
        current_schema_object.load_value_via_path( ent.first, ent.second  );
}

std::string DBEdgeDocument::extractSchemaFromId(const std::string &oid) const
{
    auto names = split( oid, "/" );
    if( names.size()>1 )
    {
        auto  label = names.front();
        return DataBase::getEdgeName( label );
    }
    return "";
}

DBQueryBase DBEdgeDocument::make_default_query_template() const
{
    std::string AQLquery = "FOR u IN " + collection_from->name();
    AQLquery += "\nFILTER u._label == '" + object_label + "' ";
    //AQLquery += "\nRETURN u ";
//    auto flds_query = make_default_query_fields();
//    auto key_flds = collection_from->keyFields();
//    flds_query.insert(flds_query.end(), key_flds.begin(), key_flds.end() );

    DBQueryBase def_query( AQLquery, DBQueryBase::qAQL );
//    if( !flds_query.empty() )
//        def_query.setQueryFields(flds_query);
    return def_query;
}

DBQueryBase DBEdgeDocument::make_default_query_old() const
{
    return DBQueryBase(std::string("{ \"_label\" : \"")+ object_label + "\" }", DBQueryBase::qTemplate);
}


} // namespace jsonio
