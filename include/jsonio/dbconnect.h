#pragma once

#include <memory>
#include <thread>
#include <shared_mutex>
#include "jsonio/dbdriverbase.h"
#include "jsonio/schema.h"

// https://en.cppreference.com/w/cpp/thread/shared_mutex
// https://ncona.com/2019/03/read-write-mutex-with-shared_mutex/


namespace jsonio {

/// Table of correspondence data.
using datamap_t = std::map<std::string, std::string>;

class DBCollection;

/// \class DataBase to managing database connection.
class DataBase
{

    /// Map of readed vertex schemas ( "_label" -> schema name )
    static datamap_t vertexes;
    /// Map of readed edge schemas ( "_label" -> schema name )
    static datamap_t edges;
    /// Map of used vertex collections ( schema name -> collection name )
    static datamap_t vertex_collections;
    /// Map of used edge collections ( schema name -> collection name )
    static datamap_t edge_collections;
    /// String with all edge collections (Anonymous graphs traverse)
    /// Instead of GRAPH graphName you may specify a list of edge collections.
    static std::vector<std::string> all_edges_traverse;

public:

    /// Get vertex names list.
    static std::vector<std::string> getVertexesList()
    {
        std::vector<std::string> members;
        auto it = vertexes.begin();
        while( it != vertexes.end() )
            members.push_back(it++->second);
        return members;
    }

    /// Get vertex schema name from label data
    static std::string getVertexName(const std::string& _label)
    {
        auto it = vertexes.find( _label );
        if( it == vertexes.end() )
            return "";
        else
            return it->second;
    }

    /// Get vertex collection from Schema name
    static std::string getVertexCollection(const std::string& schema_name)
    {
        auto it = vertex_collections.find( schema_name );
        if( it == vertex_collections.end() )
            return "";
        else
            return it->second;
    }

    /// Link Map of used vertex collections ( schema name -> collection name )
    static const std::map<std::string, std::string>& usedVertexCollections()
    {
        return vertex_collections;
    }

    /// Get edge names list
    static std::vector<std::string> getEdgesList()
    {
        std::vector<std::string> members;
        auto it = edges.begin();
        while( it != edges.end() )
            members.push_back(it++->second);
        return members;
    }

    /// Get edge schema name from label data
    static std::string getEdgeName(const std::string& _label)
    {
        auto it = edges.find( _label );
        if( it == edges.end() )
            return "";
        else
            return it->second;
    }

    /// Get edge collection from Schema name
    static std::string getEdgeCollection(const std::string& schema_name)
    {
        auto it = edge_collections.find( schema_name );
        if( it == edge_collections.end() )
            return "";
        else
            return it->second;
    }

    /// Link Map of used edge collections  (schema name, collect_name)
    static const std::map<std::string, std::string>& usedEdgeCollections()
    {
        return edge_collections;
    }

    /// Get list of edges defined from schemas
    static const std::vector<std::string>& getEdgesAllDefined()
    {
        return all_edges_traverse;
    }

    static void update_from_schema( const schemas_t& schema_data );

    /// Constructor use define database vendor.
    DataBase(const std::string &db_url, const std::string &db_user,
             const std::string &user_passwd, const std::string &db_name);

    /// Constructor use specific database vendor.
    DataBase(std::shared_ptr<AbstractDBDriver> db_driver):
        current_driver(nullptr), collections_list(),
        driver_mutex(), collections_mutex()
    {
        updateDriver( db_driver );
    }

    ///  Constructor used internal ArangoDBClient
    DataBase();

    /// Destructor
    virtual ~DataBase();

    virtual std::shared_ptr<DataBase> clone(const std::string& new_db_name) {
       std::shared_ptr<AbstractDBDriver> new_driver{theDriver()->clone(new_db_name)};
       return std::make_shared<jsonio::DataBase>(new_driver);
    }

    ///  Switch to the specified database driver (provided it exists and the user can connect to it).
    ///  From this point on, any following action in the same shell or connection will use the specified database.
    void updateDriver( std::shared_ptr<AbstractDBDriver> db_driver );

    /// Get current database driver
    AbstractDBDriver* theDriver() const
    {
        std::shared_lock lock(driver_mutex);
        return current_driver.get();
    }

    /// This report message is about checking how the Database server is responding to incoming HTTP requests.
    std::string status() const;
    /// Return Database server status.
    bool connected() const;

    /// Load the collection with the given colname or create new if no such collection exists.
    /// \param type - type of collection ( "document", "schema", "vertex", "edge" )
    /// \param colname - name of collection
    std::shared_ptr<DBCollection> collection( const std::string& colname, const std::string& type = "document"  ) const
    {
        {
            std::shared_lock lock(collections_mutex);
            auto itr = collections_list.find(colname);
            if( itr != collections_list.end() )
                return itr->second;
        }
        return add_collection( colname, type );
    }

protected:

    /// Current Database Driver
    std::shared_ptr<AbstractDBDriver> current_driver;

    /// List of loaded collections
    mutable std::map< std::string, std::shared_ptr<DBCollection> > collections_list;

    mutable std::shared_mutex driver_mutex;
    mutable std::shared_mutex collections_mutex;

    /// Load the collection with the given colname or create new if no such collection exists.
    /// \param type - type of collection ( "undef", "schema", "vertex", "edge" )
    /// \param colname - name of collection
    std::shared_ptr<DBCollection> add_collection( const std::string& colname, const std::string& type  ) const ;

};

} // namespace jsonio


