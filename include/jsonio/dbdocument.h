#pragma once

#include <memory>
#include "jsonio/exceptions.h"
#include "jsonio/dbcollection.h"
#include "jsonio/dbquerybase.h"


namespace jsonio {


/// Interface for working with documents.
/// Documents are JSON like objects. These objects can be nested (to any depth) and may contain lists.
/// Each document has a unique primary key which identifies it within its collection.
/// Furthermore, each document is uniquely identified by its document handle across all collections in the same database.
class DBDocumentBase
{
    friend class DBCollection;

    /// Do it before read document from database ( check schema )
    virtual void before_load( const std::string&  ) {}
    /// Do it after read document from database
    virtual void after_load( const std::string&  ) {}
    /// Do it before remove document with key from collection.
    virtual void before_remove( const std::string&  ) {}
    /// Do it after remove document with key from collection.
    virtual void after_remove( const std::string&  ) {}
    /// Do it before write document to database
    virtual void before_save_update( std::string&  ) {}
    /// Do it after write document to database
    virtual void after_save_update( const std::string&  ) {}

public:

    ///  Constructor when create/load collection
    DBDocumentBase( const DataBase& dbconnect, const std::string& collection_type, const std::string& collection_name );
    ///  Constructor used loaded collection
    DBDocumentBase( DBCollection* collection  );

    ///  Destructor
    virtual ~DBDocumentBase()
    {
        collection_from->eraseDocument(this);
        std::lock_guard<std::shared_mutex> g(query_result_mutex);
    }

    /// Get type of document ( collection )
    std::string type() const
    {
        return collection_from->type();
    }

    /// Internal loaded data
    virtual const JsonBase& loaded_data()  const = 0;

    /// Explicit type conversion between the JSON path value and a compatible primitive value.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// The value is filled into the input parameter.
    /// @return true if JSON value exist and can be converted to value type.
    template <typename T>
    bool getValueViaPath( const std::string& jsonpath, T& val, const T& defval   ) const
    {
        return current_data().get_value_via_path( jsonpath, val, defval );
    }

    /// Use jsonpath to modify any value in a JSON object.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// @return true if jsonpath present in a JSON object.
    virtual bool setValueViaPath( const std::string& jsonpath, const std::string& val  )
    {
        return current_data().set_value_via_path( jsonpath, val );
    }

    /// Use jsonpath to modify any value in a JSON object.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// @return true if jsonpath present in a JSON object.
    template <typename T>
    bool setValueViaPath( const std::string& jsonpath, const T& val  )
    {
        return current_data().set_value_via_path( jsonpath, val );
    }

    /// Return a reference to object[jsonpath] if an object can be create, exception otherwise.
    void setObjectViaPath( const std::string &jsonpath, const std::string &json_object )
    {
        decltype(current_data())& obj = current_data().add_object_via_path( jsonpath );
        if( !json_object.empty() )
            obj.loads(json_object);
    }

    /// Return a reference to object[jsonpath] if an array can be create, exception otherwise.
    void setArrayViaPath( const std::string &jsonpath, const std::string &json_array )
    {
        decltype(current_data())& arr = current_data().add_array_via_path( jsonpath );
        if( !json_array.empty() )
            arr.loads(json_array);
    }

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
        std::string rid = key;
        setOid( rid ); // add key _id to structure
        before_save_update( rid );
        rid = collection_from->createDocument( this );
        after_save_update( rid );
        return rid;
    }

    /// Retrive one document from the collection
    ///  \param key      - key of document
    void readDocument( const std::string& key )
    {
        before_load( key );
        collection_from->readDocument( this, key );
        after_load( key );
    }

    /// Updates an existing document or creates a new document described by the key.
    ///  \param key      - key of document
    std::string updateDocument( const std::string& key  )
    {
        std::string rid = key;
        setOid( rid ); // add key _id to structure
        before_save_update( rid );
        rid = collection_from->saveDocument( this, rid );
        after_save_update( rid );
        return rid;
    }

    /// Removes current document from the collection
    void deleteDocument()
    {
        auto key = getKeyFromCurrent();
        before_remove( key );
        collection_from->deleteDocument(this);
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
    virtual void setOid( const std::string&  newid )
    {
        current_data().set_oid(newid);
    }
    /// Extract document-handle(_id) from current document
    virtual std::string getOid() const
    {
        std::string oid;
        current_data().get_value_via_path( "_id", oid, std::string("") );
        return oid;
    }

    /// Return curent document as json string
    virtual std::string getJson( bool dense = false ) const = 0;
    /// Load document from json string
    virtual void setJson( const std::string& json_document ) = 0;

    /// Extract key from current document
    virtual std::string getKeyFromCurrent() const
    {
        return collection_from->getKeyFrom( current_data() );
    }

    /// Load document from json string
    /// \return current document key
    virtual std::string recFromJson( const std::string& jsondata, bool  )
    {
        setJson( jsondata );
        return getKeyFromCurrent( );
    }

    //--- Selection/query functions

    /// Build table of fields values by query
    ///  \param query - query condition
    ///  \param  queryFields - list of fields (columns) in result table
    ///  \return table of field values
    values_table_t downloadDocuments( const DBQueryBase&  query, const std::vector<std::string>& queryFields ) const;

    /// Build table of fields values by their keys
    ///  \param rkeys - list of top level record keys
    ///  \param  queryFields - list of fields (columns) in result table
    ///  \return table of field values
    values_table_t downloadDocuments( const std::vector<std::string>& keys, const std::vector<std::string>& queryFields ) const;

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void selectQuery( const DBQueryBase& query,  SetReaded_f setfnc ) const
    {
        collection_from->selectQuery( query, setfnc );
    }

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \return list of json strings with query result
    values_t selectQuery( const DBQueryBase& query ) const;

    /// Looks up the documents in the specified collection using the array of keys provided.
    ///  \param rkeys -      array of keys
    ///  \param setfnc -   callback function fetching document data
    void lookupByKeys( const std::vector<std::string>& rkeys,  SetReaded_f setfnc ) const
    {
        collection_from->lookupByKeys( rkeys, setfnc );
    }

    /// Looks up the documents in the specified collection using the array of keys provided.
    ///  \param rkeys -      array of keys
    ///  \return list of json strings with query result
    values_t lookupByKeys( const std::vector<std::string>& rkeys ) const;

    /// Test existence documents by query
    ///  \param query -    selection condition
    bool existDocumentsByQuery( DBQueryBase&& query ) const;

    /// Fetches all documents keys (_id) from a collection that match the specified condition.
    ///  \param query -    selection condition
    std::vector<std::string> getKeysByQuery( DBQueryBase&& query ) const;


    ///  Provides 'distinct' operation over collection
    ///  \param fpath Field path to collect distinct values from.
    ///  \return Unique values by specified fpath and collection
    values_t fieldValues( const std::string& fpath ) const
    {
        values_t values;
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
    /// \param  test_values - If test_values is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document ( generate from template if key undefined into the given data )
    std::string createWithTestValues( bool test_values = false );

    /// Updates an existing document or creates a new document described by the given data (key must present in data).
    /// \param  test_values - If test_values is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void updateWithTestValues(  bool test_values = false );

    /// Creates a new document in the collection from the given json data.
    /// \param jsondata - data to save
    /// \param  test_values - If test_values is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document ( generate from template if key undefined )
    std::string createFromJson(  const std::string& jsondata, bool test_values = false )
    {
        recFromJson( jsondata, test_values );
        return createWithTestValues( test_values );
    }

    /// Updates an existing document.
    /// \param  jsondata - data to save ( must contain the key )
    /// \param  test_values - If test_values is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void updateFromJson( const std::string& jsondata, bool test_values = false )
    {
        recFromJson( jsondata, test_values );
        updateWithTestValues( test_values );
    }

    /*
     *  Most of this functions used in import part.
     *  If  test_values is false all this function work as usual CRUD with addition setting data.
     *  If test_values is true, we compare the current data with the internally loaded values,
     *    and if all the values are the same, then we update the selected record instead of creating new ones.
     *
     *   All this functions could be call with  test_values == true only if executed before SetQuery, otherwise will be exception.
    */

    // internal selection part
    bool queryDefined() const
    {
        return query_result.get()!=nullptr;
    }


    /// Set&execute query for document
    virtual void setQuery( const DBQueryBase& query, std::vector<std::string>  fieldsList = {} );

    /// Set&execute query for document
    virtual void setQuery( const DBQueryDef& querydef );

    /// Run current query, rebuild internal table of values
    void updateQuery();

    const DBQueryResult& currentQueryResult() const
    {
        std::shared_lock<std::shared_mutex> g(query_result_mutex);

        JSONIO_THROW_IF( query_result.get() == nullptr, "DBDocument", 1,
                         "'currentQueryResult' could be execute only into selection mode." );
        return *query_result;
    }

    /// Identify key from current data.
    /// The current data is compared with the internally loaded values,
    /// and if all values are the same, the selected key is returned,
    /// otherwise, an empty string is returned.
    virtual std::string getKeyFromValue(const JsonBase& node) const
    {
        std::shared_lock<std::shared_mutex> g(query_result_mutex);

        JSONIO_THROW_IF( query_result.get() == nullptr, "DBDocument", 10,
                         "'getKeyFromValue' could be execute only into selection mode." );
        return  query_result->getKeyFromValue( node );
    }

protected:

    /// Documents are grouped into collection
    std::shared_ptr<DBCollection> collection_from;

    /// Last query result if exists
    std::shared_ptr<DBQueryResult>  query_result;

    mutable std::shared_mutex query_result_mutex;

    /// Prepare data to save to database
    virtual JsonBase& current_data() const = 0;

    /// Run current query, rebuild internal table of values
    virtual void update_query() = 0;

    /// Add line to view table
    void add_line( const std::string& key_str, const JsonBase& nodedata, bool isupdate )
    {
        std::lock_guard<std::shared_mutex> g(query_result_mutex);
        if( query_result.get() != nullptr )
            query_result->add_line( key_str, nodedata, isupdate );
    }

    /// Update line into view table
    void update_line( const std::string& key_str, const JsonBase& nodedata )
    {
        std::lock_guard<std::shared_mutex> g(query_result_mutex);
        if( query_result.get() != nullptr )
            query_result->update_line( key_str, nodedata );
    }

    /// Delete line from view table
    void delete_line( const std::string& key_str )
    {
        std::lock_guard<std::shared_mutex> g(query_result_mutex);
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


    virtual field_value_map_t extract_fields( const std::vector<std::string> queryFields, const JsonBase& domobj ) const;
    virtual field_value_map_t extract_fields( const std::vector<std::string> queryFields, const std::string& jsondata ) const;

};

} // namespace jsonio


