

/* query all connected edges
FOR v, e, p IN 1..1 ANY 'reactions/H2_15_0'
 inherits, takes, defines, master, product, prodreac, basis, pulls, involves, adds, yields
RETURN e

*/

#include <iostream>
#include "jsonio14/dbedgedoc.h"
#include "jsonio14/dbconnect.h"
#include "jsonio14/io_settings.h"


namespace jsonio14 {


DBEdgeDocument* documentAllEdges( const DataBase& dbconnect )
{
    auto schemaName = DataBase::getEdgesList()[0];
    DBEdgeDocument* edge_document = new DBEdgeDocument( schemaName, dbconnect  );
    edge_document->setMode(true);
    return edge_document;
}


DBEdgeDocument *DBEdgeDocument::newEdgeDocumentQuery(const DataBase &dbconnect, const std::string &schema_name,
                                                     DBQueryBase&& query)
{
    if( schema_name.empty()  )
        return nullptr;

    auto new_document = new DBEdgeDocument( schema_name, dbconnect );
    // internal selection
    new_document->setQuery( std::move(query));
    return new_document;
}

DBEdgeDocument *DBEdgeDocument::newEdgeDocument(const DataBase &dbconnect, const std::string &aschema_name)
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
            JARANGO_THROW( "DBEdgeDocument", 10,
                           " illegal record schame: " + aschema_name + " current schema: " + schema_name );
    }
    current_schema_object.clear(); // set default values
    current_schema_object.set_value_via_path("_to", inV);
    current_schema_object.set_value_via_path("_from", outV);

    for(auto const &ent : fldvalues)
        current_schema_object.set_value_via_path( ent.first, ent.second  );
}

std::string DBEdgeDocument::extractSchemaFromId(const std::string &oid)
{
    auto names = split( oid, "/" );
    if( names.size()>1 )
    {
        auto  label = names.front();
        return DataBase::getEdgeName( label );
    }
    return "";
}


} // namespace jsonio14
