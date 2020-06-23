
#include "jsonio14/dbcollection.h"
#include "jsonio14/dbdriverarango.h"
#include "jsonio14/service.h"

namespace jsonio14 {


datamap_t DataBase::vertexes ={};
datamap_t DataBase::edges = {};
datamap_t DataBase::vertex_collections ={};
datamap_t DataBase::edge_collections= {};
std::vector<std::string> DataBase::all_edges_traverse ={};
//"inherits, takes, defines, master, product, prodreac, basis, pulls, involves, adds, yields";


//  Constructor used internal ArangoDB driver
void DataBase::update_from_schema( const schemas_t &schema_data )
{
    vertexes.clear();
    edges.clear();
    vertex_collections.clear();
    edge_collections.clear();
    all_edges_traverse.clear();

    for( const auto& structdata:  schema_data )
    {
        const  FieldDef* type_fld = structdata.second->getField( "_type" );
        const  FieldDef* label_fld = structdata.second->getField( "_label" );
        std::string label, name, type;
        if( type_fld != nullptr && label_fld != nullptr )
        {
            name = structdata.second->name();
            label = label_fld->defaultValue();
            trim(label, "\"");
            type = type_fld->defaultValue();
            trim(type, "\"");
            if( type == "vertex")
            {
                vertexes[label ] = name;
                vertex_collections[ name ] = label+"s";
            }
            else if( type == "edge")
            {
                edges[ label ] = name;
                edge_collections[ name ] = label;
                all_edges_traverse.push_back(label);
            }
        }
    }
}

DataBase::DataBase():
    current_driver(nullptr), collections_list(),
    driver_mutex(), collections_mutex()
{
    // Default Constructor - extract data from settings
    std::shared_ptr<AbstractDBDriver> db_driver( new ArangoDBClient() );
    updateDriver( db_driver );
}

DataBase::~DataBase()
{ }

void DataBase::updateDriver( std::shared_ptr<AbstractDBDriver> db_driver )
{
    {
        std::unique_lock lock(driver_mutex);
        current_driver = db_driver;
    }
    std::shared_lock lock(collections_mutex);
    for( auto coll:  collections_list )
        coll.second->reload();
}

std::shared_ptr<DBCollection> DataBase::add_collection( const std::string& colname, const std::string& type  ) const
{
    auto col_ptr = std::shared_ptr<DBCollection>( new DBCollection( *this, colname) );
    col_ptr->coll_type = type;
    col_ptr->load();
    {
        std::unique_lock lock(collections_mutex);
        collections_list[colname] = col_ptr;
    }
    return col_ptr;
}


} // namespace jsonio14
