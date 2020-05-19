
#include "jsonio14/dbconnect.h"
#include "jsonio14/dbdriverarango.h"

namespace jsonio14 {

//  Constructor used internal ArangoDB driver
DataBase::DataBase():
    current_driver(nullptr), collections_list()
{
    // Default Constructor - extract data from settings
    std::shared_ptr<AbstractDBDriver> db_driver( new ArangoDBClient() );
    updateDriver( db_driver );
}

DataBase::~DataBase()
{ }

void DataBase::updateDriver( std::shared_ptr<AbstractDBDriver> db_driver )
{
    current_driver = db_driver;

    for( auto coll:  collections_list )
        coll.second->change_driver( current_driver.get() );
}

DBCollection *DataBase::addCollection( const std::string& type, const std::string& colname  ) const
{
    auto col_ptr = std::make_shared<DBCollection>( *this, colname );
    col_ptr->coll_type = type;
    col_ptr->load();
    collections_list[colname] = col_ptr;
    return col_ptr.get();
}


} // namespace jsonio14
