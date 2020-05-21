#pragma once

#include <functional>
#include <memory>
#include <map>
#include <set>

namespace jsonio14 {

class JsonBase;
class DBQueryBase;

/// Table key -> the selector, which must be an object containing the _id or _key attribute.
using keysmap_t = std::map<std::string, std::unique_ptr<char> >;
// Now we use "_id" as key and "_id" as selector, but for internal key we can use other data in future
// and for selector data can be used other data in driver

/// Callback function fetching document from a collection that match the specified condition
using  SetReaded_f = std::function<void( const std::string& jsondata )>;
/// Callback function fetching document and _id from a collection that match the specified condition
using  SetReadedKey_f = std::function<void( const std::string& jsondata, const std::string& key )>;

/// Interface for Abstract Database Driver
/// The driver works like an adaptor which connects a generic interface
/// to a specific database vendor implementation.
class AbstractDBDriver
{

public:

    /// Types of collection to select
    enum CollTypes {
        clVertex  = 0x0001,        ///< No edges collections
        clEdge    = 0x0002,        ///< Edges
        clAll = clVertex|clEdge    ///< List all vertexes&edges
    };

    ///  Constructor
    AbstractDBDriver()
    {}

    ///  Destructor
    virtual ~AbstractDBDriver()
    {}

    // Collections API

    /// Create collection if no exist
    /// \param colname - name of collection
    /// \param type - type of collection ( "undef", "schema", "vertex", "edge" )
    virtual void create_collection(const std::string& collname, const std::string& ctype) = 0;

    /// Returns all collections names of the given database.
    /// \param ctype - types of collection to select.
    virtual std::set<std::string> get_collections_names( CollTypes ctype ) = 0;

    virtual std::string get_server_key( const std::unique_ptr<char>& second ) const = 0;

    virtual void set_server_key( std::unique_ptr<char>& second, const std::string& key ) = 0;

    // CRUD API

    /// Creates a new document in the collection from the given data or
    /// replaces an existing document described by the selector.
    /// \param collname - collection name
    /// \param jsonrec - json object with data
    /// \return the document-handle.
    virtual std::string create_record( const std::string& collname, std::unique_ptr<char>& second, const JsonBase& recdata ) = 0;

    /// Returns the document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - object to receive data
    virtual bool read_record( const std::string& collname, keysmap_t::iterator& it, JsonBase& recdata ) = 0;

    /// Update an existing document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    virtual std::string update_record( const std::string& collname, keysmap_t::iterator& it, const JsonBase& recdata ) = 0;

    /// Removes a document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    virtual bool delete_record(const std::string& collname, keysmap_t::iterator& it ) = 0;

    // Query API

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param collname - collection name
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    virtual void select_query( const std::string& collname, const DBQueryBase& query, SetReaded_f setfnc ) = 0;

    /// Looks up the documents in the specified collection using the array of ids provided.
    ///  \param collname - collection name
    ///  \param ids -      array of _ids
    ///  \param setfnc -   callback function fetching document data
    virtual void lookup_by_ids( const std::string& collname,  const std::vector<std::string>& ids,  SetReaded_f setfnc ) = 0 ;

    /// Fetches all documents from a collection.
    ///  \param collname -    collection name
    ///  \param query_fields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    virtual void all_query( const std::string& collname,
                            const std::set<std::string>& query_fields,  SetReadedKey_f setfnc ) = 0;

    ///  Provides 'distinct' operation over collection
    ///  \param collname - collection name
    ///  \param fpath    - field path to collect distinct values from
    ///  \param  values  - return values by specified fpath and collname
    virtual void fpath_collect( const std::string& collname, const std::string& fpath,
                                std::vector<std::string>& values ) = 0;

    /// Delete all edges linked to vertex record.
    ///  \param collname - collection name
    ///  \param vertexid - vertex record id
    virtual void delete_edges(const std::string& collname, const std::string& vertexid ) = 0;

    /// Removes all documents from the collection whose keys are contained in the keys array.
    ///  \param collname - collection name
    ///  \param ids -      array of keys
    virtual void remove_by_ids( const std::string& collname,  const std::vector<std::string>& ids  ) = 0;

    /// Check the document-handle example in to contain only
    /// characters officially allowed by driver.
    /// \return  a document-handle that contain only only allowed characters.
    virtual std::string sanitization( const std::string& documentHandle ) = 0;
};


} // namespace jsonio14


