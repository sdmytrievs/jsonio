#pragma once

#include <memory>
#include "jsonio14/exceptions.h"
#include "jsonio14/dbcollection.h"
#include "jsonio14/dbquerybase.h"

namespace jsonio14 {


//// some extern functions
//int extractIntField( const std::string& key, const std::string& query );
//std::string extractStringField( const std::string& key, const std::string& query );
//void addFieldsToQueryAQL( DBQueryBase& query, const field_value_map_t& fldvalues );


/// Interface for working with documents.
/// Documents are JSON objects. These objects can be nested (to any depth) and may contain lists.
/// Each document has a unique primary key which identifies it within its collection.
/// Furthermore, each document is uniquely identified by its document handle across all collections in the same database.
class DBDocumentBase
{
    friend class DBCollection;

    /// Do it after read document from database
    virtual void after_load( const std::string&  ) {}
    /// Do it before remove document with key from collection.
    virtual void before_remove( const std::string&  ) {}
    /// Do it after remove document with key from collection.
    virtual void after_remove( const std::string&  ) {}
    /// Do it before write document to database
    virtual void before_save_update( const std::string&  ) {}
    /// Do it after write document to database
    virtual void after_save_update( const std::string&  ) {}


public:

    ///  Constructor when create/load collection
    DBDocumentBase( const DataBase* dbconnect, const std::string& coltype, const std::string& colname  );
    ///  Constructor used loaded collection
    DBDocumentBase( DBCollection* collection  );

    ///  Destructor
    virtual ~DBDocumentBase()
    {
        collection_from->eraseDocument(this);
    }

    /// Get type of document ( collection )
    std::string type() const
    {
        return collection_from->type();
    }

//    /// Access to the database
//    const DataBase* database() const
//    {
//        return collection_from->database();
//    }


    //--- Manipulation documents

    /// Checks whether a current document exists into database.
    bool existsDocument()
    {
        return collection_from->existsDocument( getKeyFromCurrent() );
    }

    /// Checks whether a document exists.
    bool existsDocument( const std::string& key )
    {
        return collection_from->existsDocument( key );
    }

    /// Creates a new document in the collection from the current data.
    /// \param key      - key of document
    /// \return new key of document ( generate from template if key undefined )
    std::string createDocument( const std::string& key = "" )
    {
        setOid( key ); // add key _id to structure
        before_save_update( key );
        auto rid = collection_from->createDocument( this );
        after_save_update( key );
        return rid;
    }

    /// Retrive one document from the collection
    ///  \param key      - key of document
    void readDocument( const std::string& key )
    {
        testUpdateSchema( key );
        collection_from->readDocument( this, key );
        after_load( key );
    }

    /// Updates an existing document or creates a new document described by the key.
    ///  \param key      - key of document
    std::string updateDocument( const std::string& key  )
    {
        setOid( key ); // add key _id to structure
        before_save_update( key);
        auto rid = collection_from->saveDocument( this, key );
        after_save_update( key);
        return rid;
    }

    /// Removes current document from the collection
    void deleteDocument()
    {
        auto key = getKeyFromCurrent();
        before_remove( key );
        collection_from->deleteDocument(key);
        after_remove( key );
    }

    /// Removes document from the collection
    /// \param key      - key of document
    void deleteDocument( const std::string& key )
    {
        before_remove( key );
        collection_from->deleteDocument(key);
        after_remove( key );
    }

    // The document-handle API

    /// Generate new document-handle (_id) or other pointer of location
    virtual std::string genOid( const std::string& key_template  )
    {
        return collection_from->generateOid( key_template );
    }
    /// Set document-handle(_id) to document
    virtual void setOid( const std::string&  newid ) = 0;
    /// Extract document-handle(_id) from current document
    virtual std::string getOid() const = 0;


    /// Test document before read
    virtual void testUpdateSchema( const std::string&  /*pkey*/ )
    { }

    /// Return curent document as json string
    virtual std::string getJson( bool dense = false ) const = 0;
    /// Load document from json string
    virtual void setJson( const std::string& json_document ) = 0;

    /// Extract key from current document
    virtual std::string getKeyFromCurrent() const = 0;
    /// Get document key from dom object
    virtual std::string getKeyFromDom( const JsonBase* object  ) const
    {
        return collection_from->getKeyFrom(object);
    }

    /// Load document from json string
    /// \return current document key
    virtual std::string recFromJson( const std::string& jsondata ) = 0;


    //--- Selection/query functions

//    virtual FieldSetMap extractFields( const std::vector<std::string> queryFields, const JsonDom* domobj ) const =  0;
//    virtual FieldSetMap extractFields( const std::vector<std::string> queryFields, const std::string& jsondata ) const = 0;

    /// Execute query
    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void selectQuery( const DBQueryBase& query,  SetReaded_f setfnc ) const
    {
        // fields deprecated (EJDB)
        collection_from->selectQuery( query, setfnc );
    }

//    /// Execute query
//    /// Fetches all documents from a collection that match the specified condition.
//    ///  \param query -    selection condition
//    ///  \return list of json strings with query result
//    std::vector<std::string> runQuery( const DBQueryBase& query ) const;

    /// Looks up the documents in the specified collection using the array of keys provided.
    ///  \param rkeys -      array of keys
    ///  \param setfnc -   callback function fetching document data
    void lookupByKeys( const std::vector<std::string>& rkeys,  SetReaded_f setfnc ) const
    {
        collection_from->lookupByKeys( rkeys, setfnc );
    }

//    /// Looks up the documents in the specified collection using the array of keys provided.
//    ///  \param rkeys -      array of keys
//    ///  \return list of json strings with query result
//    std::vector<std::string> runByKeys( const std::vector<std::string>& rkeys ) const;

//    /// Test existence documents by query
//    ///  \param query -    selection condition
//    bool existKeysByQuery( DBQueryData& query ) const;

//    /// Fetches all documents keys (_id) from a collection that match the specified condition.
//    ///  \param query -    selection condition
//    std::vector<std::string> getKeysByQuery( DBQueryData& query ) const;

//    /// Build table of fields values by query
//    ///  \param query - query condition
//    ///  \param  queryFields - list of fields (columns) in result table
//    ///  \return table of field values
//    ValuesTable downloadDocuments( const DBQueryData&  query, const std::vector<std::string>& queryFields ) const;

//    /// Build table of fields values by their keys
//    ///  \param rkeys - list of top level record keys
//    ///  \param  queryFields - list of fields (columns) in result table
//    ///  \return table of field values
//    ValuesTable downloadDocuments( const std::vector<std::string>& keys, const std::vector<std::string>& queryFields ) const;

    ///  Provides 'distinct' operation over collection
    ///  \param fpath Field path to collect distinct values from.
    ///  \return Unique values by specified fpath and collection
    std::vector<std::string> fieldValues( const std::string& fpath ) const
    {
        std::vector<std::string> values;
        collection_from->fpathCollect( fpath, values );
        return values;
    }

    /// Removes all documents from the collection whose keys are contained in the keys array.
    ///  \param rkeys - list of top level record keys
    ///  Possible do not remove edges for vertices
    void removeByKeys( const std::vector<std::string>& keys  )
    {
        collection_from->removeByKeys( keys);
    }

    //--- Addition complex functions

    /// Creates a new document in the collection from the given data.
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document ( generate from template if key undefined into the given data )
    std::string CreateWithTestValues( bool testValues = false );

    /// Updates an existing document or creates a new document described by the given data (key must present in data).
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void UpdateWithTestValues(  bool testValues = false );

    /// Creates a new document in the collection from the given json data.
    /// \param jsondata - data to save
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document ( generate from template if key undefined )
    std::string CreateFromJson(  const std::string& jsondata, bool testValues = false )
    {
        recFromJson( jsondata );
        return CreateWithTestValues( testValues );
    }

    /// Updates an existing document.
    /// \param  jsondata - data to save ( must contain the key )
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void UpdateFromJson( const std::string& jsondata, bool testValues = false )
    {
        recFromJson( jsondata );
        UpdateWithTestValues( testValues );
    }

    /*
     *  Most of this functions used in import part.
     *  If  testValues is false all this function work as usual CRUD with addition setting data.
     *  If testValues is true, we compare the current data with the internally loaded values,
     *    and if all the values are the same, then we update the selected record instead of creating new ones.
     *
     *   All this functions could be call with  testValues == true only if executed before SetQuery, otherwise will be exception.
    */

    // internal selection part

    /// Set&execute query for document
    virtual void setQuery( DBQueryBase&& query, std::vector<std::string>  fieldsList = {} );

    /// Set&execute query for document
    virtual void setQuery( const DBQueryDef& querydef );

    /// Run current query, rebuild internal table of values
    virtual void updateQuery() = 0;

    const DBQueryResult* currentQueryResult() const
    {
     JARANGO_THROW_IF( query_result.get() == nullptr, "DBDocument", 1, "'currentQueryResult' could be execute only into selection mode." );
      return query_result.get();
    }

protected:

    /// Documents are grouped into collection
    DBCollection* collection_from;

    /// Internal type of collection ( "document", "schema", "vertex", "edge" )
    /// There are two types of collections: document collection as well as edge collections.
    std::string coll_type = "document";

    /// Last query result if exists
    std::shared_ptr<DBQueryResult>  query_result;

    /// Prepare data to save to database
    virtual const JsonBase& current_data( time_t crtt, char* oid ) const = 0;
    virtual JsonBase* current_data() const = 0;

    /// Add line to view table
    void add_line( const std::string& key_str, const JsonBase* nodedata, bool isupdate )
    {
        if( query_result.get() != nullptr )
            query_result->add_line( key_str, nodedata, isupdate );
    }

    /// Update line into view table
    void update_line( const std::string& key_str, const JsonBase* nodedata )
    {
        if( query_result.get() != nullptr )
            query_result->update_line( key_str, nodedata );
    }

    /// Delete line from view table
    void delete_line( const std::string& key_str )
    {
        if( query_result.get() != nullptr )
            query_result->delete_line( key_str );
    }

    /// Build default query for collection ( by default all documents )
    virtual DBQueryBase make_default_query_template() const
    {
      return DBQueryBase(DBQueryBase::qAll);
    }

    /// Build default query fields ( by default internal )
    virtual std::vector<std::string>  make_default_query_fields() const
    {
       return  { "_id", "_key","_rev"};
    }

    /// Find key from current data
    /// Compare to data into query table
    virtual std::string get_key_from_query_result() const
    {
      JARANGO_THROW_IF( query_result.get() == nullptr, "DBDocument", 1, "'get_key_from_query_result' could be execute only into selection mode." );
      return  query_result->getKeyFromValue( current_data() );
    }

};

} // namespace jsonio14


