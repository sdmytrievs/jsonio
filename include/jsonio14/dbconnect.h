#pragma once

#include <memory>
#include "jsonio14/dbcollection.h"

namespace jsonio14 {

/// \class DataBase to managing database connection.
class DataBase
{

public:

    /// Constructor use specific database vendor.
    DataBase( std::shared_ptr<AbstractDBDriver> db_driver )
    {
        updateDriver( db_driver );
    }
    ///  Constructor used internal ArangoDBClient
    DataBase();

    /// Destructor
    virtual ~DataBase();

    ///  Switch to the specified database driver (provided it exists and the user can connect to it).
    ///  From this point on, any following action in the same shell or connection will use the specified database.
    void updateDriver( std::shared_ptr<AbstractDBDriver> db_driver );

    /// Get current database driver
    const AbstractDBDriver* theDriver() const
    {
        return current_driver.get();
    }

    /// Load the collection with the given colname or create new if no such collection exists.
    /// \param type - type of collection ( "undef", "schema", "vertex", "edge" )
    /// \param colname - name of collection
    DBCollection *getCollection( const std::string& type, const std::string& colname  ) const
    {
        auto itr = collections_list.find(colname);
        if( itr != collections_list.end() )
            return itr->second.get();

        return addCollection( type, colname );
    }

protected:

    /// Current Database Driver
    std::shared_ptr<AbstractDBDriver> current_driver;

    /// List of loaded collections
    mutable std::map< std::string, std::shared_ptr<DBCollection> > collections_list;

    /// Load the collection with the given colname or create new if no such collection exists.
    /// \param type - type of collection ( "undef", "schema", "vertex", "edge" )
    /// \param colname - name of collection
    DBCollection *addCollection( const std::string& type, const std::string& colname  ) const ;

};

} // namespace jsonio14


