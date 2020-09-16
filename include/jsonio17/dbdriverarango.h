#pragma once

#include "jsonio17/dbdriverbase.h"

namespace arangocpp {
enum class CollectionTypes;
struct ArangoDBConnection;
class ArangoDBCollectionAPI;
}

namespace jsonio17 {

/// Implementation of Database Driver using Low-Level C++ Driver for ArangoDB.
class ArangoDBClient: public AbstractDBDriver
{

public:

    ///  Constructor
    ArangoDBClient();

    /// Constructor
    explicit ArangoDBClient( const arangocpp::ArangoDBConnection& aconnect_data ):AbstractDBDriver()
    {
        reset_db_connection( aconnect_data );
    }

    ///  Destructor
    ~ArangoDBClient()
    {}

    // Collections API

    /// Create collection if no exist
    /// \param colname - name of collection
    /// \param type - type of collection ( "undef", "schema", "vertex", "edge" )
    void create_collection(const std::string& collname, const std::string& ctype) override;

    /// Returns all collections names of the given database.
    /// \param ctype - types of collection to select.
    std::set<std::string> get_collections_names( CollTypes ctype ) override;

    std::string get_server_key( const std::string& second ) const override
    {
        return second;
    }

    void set_server_key( std::string& second, const std::string& key ) override;

    // CRUD API

    /// Creates a new document in the collection from the given data or
    /// replaces an existing document described by the selector.
    /// \param collname - collection name
    /// \param jsonrec - json object with data
    /// \return the document-handle.
    std::string create_record( const std::string& collname, std::string& second, const JsonBase& recdata ) override;

    /// Returns the document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - object to receive data
    bool read_record( const std::string& collname, keysmap_t::iterator& it, JsonBase& recdata ) override;

    /// Update an existing document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    std::string update_record( const std::string& collname, keysmap_t::iterator& it, const JsonBase& recdata ) override;

    /// Removes a document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    bool delete_record(const std::string& collname, keysmap_t::iterator& it ) override;

    // Query API

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param collname - collection name
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void select_query( const std::string& collname, const DBQueryBase& query, SetReaded_f setfnc ) override;

    /// Looks up the documents in the specified collection using the array of ids provided.
    ///  \param collname - collection name
    ///  \param ids -      array of _ids
    ///  \param setfnc -   callback function fetching document data
    void lookup_by_ids( const std::string& collname,  const std::vector<std::string>& ids,  SetReaded_f setfnc ) override;

    /// Fetches all documents from a collection.
    ///  \param collname -    collection name
    ///  \param query_fields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    void all_query( const std::string& collname, const std::set<std::string>& query_fields,  SetReadedKey_f setfnc ) override;

    ///  Provides 'distinct' operation over collection
    ///  \param collname - collection name
    ///  \param fpath    - field path to collect distinct values from
    ///  \param  values  - return values by specified fpath and collname
    void fpath_collect( const std::string& collname, const std::string& fpath, std::vector<std::string>& values ) override;

    /// Delete all edges linked to vertex record.
    ///  \param collname - collection name
    ///  \param vertexid - vertex record id
    void delete_edges(const std::string& collname, const std::string& vertexid ) override;

    /// Removes all documents from the collection whose keys are contained in the keys array.
    ///  \param collname - collection name
    ///  \param ids -      array of keys
    void remove_by_ids( const std::string& collname,  const std::vector<std::string>& ids  ) override;

    /// Check the document-handle example in to contain only
    /// characters officially allowed by ArangoDB.
    /// \return  a document-handle that contain only only allowed characters.
    std::string sanitization( const std::string& documentHandle ) override;

protected:

    /// ArangoDB connection data
    std::shared_ptr<arangocpp::ArangoDBConnection> arando_connect = nullptr;

    /// ArangoDB connection data
    std::shared_ptr<arangocpp::ArangoDBCollectionAPI> arando_db = nullptr;

    /// Reset connections to ArangoDB server
    void reset_db_connection( const arangocpp::ArangoDBConnection& connect_data );

    bool is_comlex_fields( const std::set<std::string>& query_fields);

    arangocpp::CollectionTypes to_arrango_types(CollTypes ctype) const;

};


} // namespace jsonio17


