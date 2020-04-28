#pragma once

#include <memory>
#include "jsonio14/dbdriverbase.h"

namespace jsonio14 {


class DataBase;
class DBDocumentBase;


/// Internal function to generate key template
std::string makeTemplateKey( const JsonBase *object, const std::vector<std::string>&  keyTemplateFields );


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
    void Load();
    /// Close the collection and free internal data
    void Close();

    /// Refresh list of documents keys
    void reloadCollection();

    //--- Selectors

    /// Get the unique name of collection
    const std::string& name() const
    {  return _name;   }


    /// Get type of collection
    std::string type() const
    {
        return _coltype;
    }

    /// Get number of documents into collection
    std::size_t getDocumentsCount() const
    {
        return _recList.size();
    }

    /// Access to the database
    const DataBase* database() const
    {
        return _dbconnect;
    }

    /// Add new opened document from collection
    void addDocument( DBDocumentBase* doc)
    {
        _documents.insert(doc);
    }
    /// Erase opened document from collection
    void removeDocument( DBDocumentBase* doc)
    {
        auto itr =  _documents.find(doc);
        if( itr != _documents.end() )
            _documents.erase(doc);
    }

    /// Get document key from dom object
    virtual std::string getKeyFromDom( const JsonBase* object );

    //--- Manipulation records

    /// Checks whether a document exists.
    bool Find( const std::string& key );

    /// Creates a new document in the collection from the given data.
    /// \param document - object with data
    std::string Create( DBDocumentBase* document );

    /// Retrive one document from the collection
    ///  \param document - object with data
    ///  \param key      - key of document
    void Read( DBDocumentBase* document, const std::string& key );

    /// Updates an existing document or creates a new document described by the key,
    /// which must be an object containing the _id or _key attribute.
    ///  \param document - object with data
    ///  \param key      - key of document
    std::string Update( DBDocumentBase* document, const std::string& key );

    /// Removes document from the collection
    /// \param key      - key of document
    void Delete( const std::string& key );

    /// Generate new _id or other pointer of location
    virtual std::string genOid( const std::string& _keytemplate )
    {
        //temporaly
        if( _keytemplate.empty())
            return "";
        return _name+"/"+genKeyFromTemplate(_keytemplate);
        //return _dbdriver->genOid(getName(), genKeyFromTemplate(_keytemplate));
    }

    //--- Manipulation list of records (tables)

    /// Build list of key fields for query
    virtual std::set<std::string> listKeyFields();

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void selectQuery( const DBQueryBase& query,  SetReaded_f setfnc );

    /// Looks up the documents in the specified collection using the array of keys provided.
    ///  \param rkeys -      array of keys
    ///  \param setfnc -   callback function fetching document data
    void lookupByDocumentKeys( const std::vector<std::string>& rkeys,  SetReaded_f setfnc )
    {
        auto ids = idsFromRecordKeys(  rkeys );
        _dbdriver->lookup_by_ids( name(), ids, setfnc );
    }

    /// Looks up the documents in the specified collection using the array of ids provided.
    ///  \param ids -      array of _ids
    ///  \param setfnc -   callback function fetching document data
    void lookupByIds( const std::vector<std::string>& ids,  SetReaded_f setfnc )
    {
        _dbdriver->lookup_by_ids( name(), ids, setfnc );
    }

    /// Fetches all documents from a collection.
    ///  \param queryFields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    void allQuery( const std::set<std::string>& queryFields, SetReadedKey_f setfnc )
    {
        _dbdriver->all_query( name(), queryFields, setfnc );
    }

    /// Delete all edges linked to vertex record.
    ///  \param vertexid - vertex record id
    void deleteEdges( const std::string& vertexid )
    {
        _dbdriver->delete_edges( name(), vertexid );
        reloadCollection();
    }

    /// Removes all documents from the collection whose keys are contained in the keys array.
    ///  \param collname - collection name
    ///  \param rkeys -      array of keys
    void removeByDocumentKeys( const std::vector<std::string>& rkeys  )
    {
        auto ids = idsFromRecordKeys(  rkeys );
        _dbdriver->remove_by_ids( name(), ids );
        reloadCollection();
    }

    ///  Provides 'distinct' operation over collection
    ///  \param fpath Field path to collect distinct values from.
    ///  \param return values by specified fpath and collection
    void fpathCollect( const std::string& fpath, std::vector<std::string>& values )
    {
        _dbdriver->fpath_collect( name(), fpath, values );
    }

protected:

    /// Internal type of collection ( "undef", "schema", "vertex", "edge" )
    /// There are two types of collections: document collection as well as edge collections.
    std::string _coltype = "undef";

    /// The unique name of collection
    std::string _name;

    /// Operations started in one database
    const DataBase* _dbconnect;

    /// Access to a specific database vendor implementation
    AbstractDBDriver* _dbdriver;

    /// Documents are linked to collection
    std::set<DBDocumentBase*> _documents; ///< Linked documents

    // Definition of record key list - internal loaded data

    /// List all documents keys into collection
    keysmap_t _recList;
    /// Current item in recList
    keysmap_t::iterator _itrL;

    /// Reconnect DataBase ( switch to the new database driver)
    void changeDriver( AbstractDBDriver* adriver );

    // internal functions

    /// Load all keys from one collection
    virtual void loadCollectionFile( const std::set<std::string>& queryFields );
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
        _dbdriver->create_collection( name(), _coltype );
        auto keys = listKeyFields();
        loadCollectionFile( keys );
    }

    /// Add key from json structure to recList
    void listKeyFromDom( const std::string& jsondata, const std::string& keydata );

    /// Generate new _id from key template
    std::string genKeyFromTemplate( const std::string& _keytemplate );

    /// Convert record keys to ids
    std::vector<std::string> idsFromRecordKeys( const std::vector<std::string>& rkeys );

    /// Get ids list for a wildcard search
    std::size_t GetIdsList( const std::string& idpart, std::set<std::string>& aKeyList );

    /// Check if pattern/undefined in record key
    bool isPattern( const std::string& akey );


};



} // namespace jsonio14


