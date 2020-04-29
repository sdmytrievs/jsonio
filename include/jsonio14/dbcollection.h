#pragma once

#include <memory>
#include "jsonio14/dbdriverbase.h"

namespace jsonio14 {


class DataBase;
class DBDocumentBase;


/// Internal function to generate key template
std::string makeTemplateKey( const JsonBase *object, const std::vector<std::string>&  key_template_fields );


/// Class  TDBCollection - definition collections API
/// Documents are grouped into collections.
/// A collection contains zero or more documents.
class DBCollection
{
    friend class DataBase;

public:

    ///  Constructor
    DBCollection( const DataBase* adbconnect, const std::string& name );
    ///  Destructor
    virtual ~DBCollection()
    {}

    /// Load the collection or create new if no such collection exists.
    void load();
    /// Close the collection and free internal data
    void close();
    /// Refresh list of documents keys
    void reload();

    //--- Selectors

    //    /// Access to the database
    //    const DataBase* database() const
    //    {
    //        return db_connect;
    //    }

    /// Get the unique name of collection
    const std::string& name() const
    {
        return coll_name;
    }

    /// Get type of collection
    std::string type() const
    {
        return coll_type;
    }

    /// Get number of documents into collection
    std::size_t documentsCount() const
    {
        return key_record_map.size();
    }

    //--- Manipulation documents

    /// Add new opened document from collection
    void linkDocument( DBDocumentBase* doc)
    {
        documents_list.insert(doc);
    }
    /// Erase document from collection
    void unlinkDocument( DBDocumentBase* doc)
    {
        auto itr =  documents_list.find(doc);
        if( itr != documents_list.end() )
            documents_list.erase(doc);
    }

    //--- Manipulation records

    /// Build list of key fields for query
    virtual std::set<std::string> keyFields()
    {
        return {"_id"};
    }

    /// Extract documents key from json object
    virtual std::string extract_key_from( const JsonBase* object );

    /// Generate new oid or other pointer of location
    virtual std::string generate_id( const std::string& key_template );

    /// Checks whether a document exists.
    bool exist_key( const std::string& key );

    /// Creates a new document in the collection from the given data.
    /// \param document - object with data
    std::string create_new_record_in_db( DBDocumentBase* document );

    /// Retrive one document from the collection
    ///  \param document - object with data
    ///  \param key      - key of document
    void read_record_from_db( DBDocumentBase* document, const std::string& key );

    /// Updates an existing document or creates a new document described by the key,
    /// which must be an object containing the _id or _key attribute.
    ///  \param document - object with data
    ///  \param key      - key of document
    std::string save_record_to_db( DBDocumentBase* document, const std::string& key );

    /// Removes document from the collection
    /// \param key      - key of document
    void delete_record_from_db( const std::string& key );

    //--- Manipulation list of records (tables)

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void selectQuery( const DBQueryBase& query,  SetReaded_f setfnc )
    {
        db_driver->select_query( name(), query, setfnc );
    }

    /// Looks up the documents in the specified collection using the array of keys provided.
    ///  \param rkeys -      array of keys
    ///  \param setfnc -   callback function fetching document data
    void lookupByKeys( const std::vector<std::string>& rkeys,  SetReaded_f setfnc )
    {
        auto ids = ids_from_keys(  rkeys );
        db_driver->lookup_by_ids( name(), ids, setfnc );
    }

    /// Looks up the documents in the specified collection using the array of ids provided.
    ///  \param ids -      array of _ids
    ///  \param setfnc -   callback function fetching document data
    void lookupByIds( const std::vector<std::string>& ids,  SetReaded_f setfnc )
    {
        db_driver->lookup_by_ids( name(), ids, setfnc );
    }

    /// Fetches all documents from a collection.
    ///  \param queryFields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    void allQuery( const std::set<std::string>& queryFields, SetReadedKey_f setfnc )
    {
        db_driver->all_query( name(), queryFields, setfnc );
    }

    /// Delete all edges linked to vertex record.
    ///  \param vertexid - vertex record id
    void deleteEdges( const std::string& vertexid )
    {
        db_driver->delete_edges( name(), vertexid );
        reload();
    }

    /// Removes all documents from the collection whose keys are contained in the keys array.
    ///  \param collname - collection name
    ///  \param rkeys -      array of keys
    void removeByKeys( const std::vector<std::string>& rkeys  )
    {
        auto ids = ids_from_keys(  rkeys );
        db_driver->remove_by_ids( name(), ids );
        reload();
    }

    ///  Provides 'distinct' operation over collection
    ///  \param fpath Field path to collect distinct values from.
    ///  \param return values by specified fpath and collection
    void fpathCollect( const std::string& fpath, std::vector<std::string>& values )
    {
        db_driver->fpath_collect( name(), fpath, values );
    }

protected:

    /// Internal type of collection ( "undef", "schema", "vertex", "edge" )
    /// There are two types of collections: document collection as well as edge collections.
    std::string coll_type = "undef";

    /// The unique name of collection
    std::string coll_name;

    /// Operations started in one database
    const DataBase* db_connect;

    /// Access to a specific database vendor implementation
    AbstractDBDriver* const db_driver;

    /// Documents are linked to collection
    std::set<DBDocumentBase*> documents_list;

    // Definition of record key list - internal loaded data

    /// List all documents keys into collection
    keysmap_t key_record_map;
    /// Current item in key_record_map
    keysmap_t::iterator key_record_itr;

    /// Reconnect DataBase ( switch to the new database driver)
    void change_driver( AbstractDBDriver* adriver );

    // internal functions

    /// Load all keys from one collection
    virtual void loadCollectionFile( const std::set<std::string>& query_fields );
    /// Close collection file
    virtual void closeCollectionFile() {}

    /// Close collection
    void closeCollection()
    {
        closeCollectionFile();
    }

    /// Load collection
    void loadCollection()
    {
        // create collection if no exist
        db_driver->create_collection( name(), coll_type );
        loadCollectionFile( keyFields() );
    }

    /// Generate new _id from key template
    std::string get_id_from_template( const std::string& key_template ) const;

    /// Add key from json structure to key_record_map
    void add_record_to_map( const std::string& jsondata, const std::string& keydata );

    /// Convert record keys to ids
    std::vector<std::string> ids_from_keys( const std::vector<std::string>& rkeys ) const;

    /// Get ids list for a wildcard search
    std::set<std::string> get_ids_as_template( const std::string& id_part ) const;

    /// Check if pattern/undefined in record key
    bool is_pattern( const std::string& akey ) const;
};



} // namespace jsonio14


