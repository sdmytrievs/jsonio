
#include "jsonio14/dbconnect.h"
#include "jsonio14/dbdriverarango.h"

namespace jsonio14 {

//  Constructor used internal ArangoDB driver
DataBase::DataBase()
{
    // Default Constructor - extract data from settings
    std::shared_ptr<AbstractDBDriver> dbDriver( new ArangoDBClient() );
    updateDriver( dbDriver );
}

DataBase::~DataBase()
{ }

void DataBase::updateDriver( std::shared_ptr<AbstractDBDriver> dbDriver )
{
    current_driver = dbDriver;

    for( auto coll:  collections_list )
        coll.second->changeDriver( current_driver.get() );
}

DBCollection *DataBase::addCollection( const std::string& type, const std::string& colname  ) const
{
    DBCollection *col = new DBCollection( this, colname );
    col->_coltype = type;
    col->Load();
    collections_list[colname] = std::shared_ptr<DBCollection>(col);
    return col;
}


} // namespace jsonio14
