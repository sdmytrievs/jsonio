#pragma once

#include "jsonio14/dbdocument.h"
#include "jsonio14/jsonschema.h"


namespace jsonio14 {


/// Definition of schema based database document
class DBSchemaDocument : public  dvf  DBDocumentBase
{
    friend class GraphTraversal;

public:

    static DBSchemaDocument* newSchemaDocumentQuery( const TDataBase* dbconnect, const std::string& schemaName,
                         const std::string& collcName, const DBQueryData& query = emptyQuery  );

    static DBSchemaDocument* newSchemaDocument( const TDataBase* dbconnect,
              const std::string& schemaName, const std::string& collcName  );

    ///  Constructor
    TDBSchemaDocument( const std::string& aschemaName,
                       const TDataBase* dbconnect, const std::string& colname ):
        TDBSchemaDocument( aschemaName, dbconnect, "schema", colname )
    {
        resetSchema( aschemaName, true );
    }

    ///  Constructor
    TDBSchemaDocument( const std::string& schemaName, TDBCollection* collection  );

    /// Constructor from configuration data
    TDBSchemaDocument( TDBCollection* collection, const JsonDom *object );
    virtual void toJsonNode( JsonDom *object ) const;
    virtual void fromJsonNode( const JsonDom *object );

    ///  Destructor
    virtual ~TDBSchemaDocument(){}

    /// Change current schema
    virtual void resetSchema( const std::string& aschemaName, bool change_queries );

    /// Get the name of thrift schema
    const std::string& getSchemaName() const
    {
        return _schemaName;
    }

    /// Link to internal dom data
    virtual const JsonDomSchema* getDom() const
    {
        return _currentData.get();
    }

    /// Set _id to document
    void setOid( const std::string& _oid  )
    {
        _currentData->setOid_( _oid );
    }
    /// Extract _id from current document
    std::string getOid() const
    {
        std::string stroid;
        getValue( "_id", stroid );
        return stroid;
    }

    /// Return curent document as json string
    std::string GetJson( bool dense = false ) const;
    /// Load document from json string
    void SetJson( const std::string& sjson );


    /// Extract key from current document
    std::string getKeyFromCurrent() const;
    /// Load document from json string
    /// \return current document key
    std::string recFromJson( const std::string& jsondata );


    /// Get field value from document
    /// If field is not type T, the false will be returned.
    template <class T>
    bool getValue( const std::string& fldpath, T& value  ) const
    {
        return  _currentData->findValue( fldpath, value );
    }

    /// Update field value to document
    template <class T>
    bool setValue( const std::string& fldpath, const T& value  )
    {
        return _currentData->setFieldValue( fldpath, value );
    }

    FieldSetMap extractFields( const std::vector<std::string> queryFields, const JsonDom* domobj ) const;
    FieldSetMap extractFields( const std::vector<std::string> queryFields, const std::string& jsondata ) const;

    /// Set&execute query for document
    void SetQuery( const DBQueryDef& querydef );

    /// Set&execute query for document
    void SetQuery( DBQueryData query, std::vector<std::string>  fieldsList = {} );

    /// Run current query, rebuild internal table of values
    void updateQuery();

protected:

    /// Link to current schema definition
    const ThriftSchema *_schema;

    /// Current schema name
    std::string _schemaName = "";

    /// Current document object
    std::shared_ptr<JsonDomSchema> _currentData;

    /// Constructor
    TDBSchemaDocument( const std::string& schemaName, const TDataBase* dbconnect,
                       const std::string& coltype, const std::string& colname );

    /// Prepare data to save to database
    JsonDomSchema* recToSave( time_t crtt, char* oid );

    /// Build default query fields ( by default internal )
    std::vector<std::string>  makeDefaultQueryFields() const;

    /// Find key from current data
    /// Compare to data into query table
    std::string getKeyFromValueNode() const
    {
      jsonioErrIf( _queryResult.get() == nullptr, "testValues", "Could be execute only into selection mode.");
      return  _queryResult->getKeyFromValue(_currentData.get());
    }

};

} // namespace jsonio

#endif // TDBSCHEMADOC_H
