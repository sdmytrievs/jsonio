#pragma once

#include "jsonio/dbdocument.h"
#include "jsonio/jsonfree.h"


namespace jsonio {


/// \class DBJsonDocument - implementation of the database document  as free structure JSON
class DBJsonDocument : public DBDocumentBase
{
    /// Do it before write document to database
    void before_save_update( std::string&  ) override;

protected:

    DBJsonDocument( const DataBase& dbconnect,
                    const std::string& coltype, const std::string& colname,
                    const std::vector<std::string>& key_template_fields ):
        DBDocumentBase( dbconnect, coltype, colname ),
        to_key_template_fields(key_template_fields), current_object( JsonFree::object() )
    {}

public:

    static DBJsonDocument* newJsonDocumentQuery( const DataBase& dbconnect, const std::string& collcName,
                                                 const std::vector<std::string>& key_template_fields,
                                                 const DBQueryBase& query = DBQueryBase::emptyQuery() );

    static DBJsonDocument* newJsonDocument( const DataBase& dbconnect, const std::string& collcName,
                                            const std::vector<std::string>& key_template_fields = {} );

    ///  Constructor
    DBJsonDocument( const DataBase& dbconnect, const std::string& colname,
                    const std::vector<std::string>& key_template_fields = {} ):
        DBJsonDocument( dbconnect, "document", colname, key_template_fields )
    {}

    ///  Constructor
    DBJsonDocument( DBCollection* collection, const std::vector<std::string>& key_template_fields = {}  ):
        DBDocumentBase( collection ),
        to_key_template_fields(key_template_fields), current_object( JsonFree::object() )
    {}

    ///  Destructor
    virtual ~DBJsonDocument(){}

    /// Link to internal data
    const JsonFree& loaded_data() const override
    {
        return  current_object;
    }

    /// Generate new document-handle (_id) or other pointer of location
    std::string genOid( const std::string& key_template ) override
    {
        auto thetemplate = key_template;
        if( thetemplate.empty() )
            thetemplate = make_template_key( current_object, to_key_template_fields );
        return collection_from->generateOid( thetemplate );
    }

    /// Return curent document as json string
    std::string getJson( bool dense = false ) const override
    {
        return current_object.dump(dense);
    }
    /// Load document from json string
    void setJson( const std::string& json_document ) override
    {
        current_object.loads(json_document);
    }

protected:

    /// Names of fields to generate key template
    std::vector<std::string>  to_key_template_fields;

    /// Current document object
    JsonFree current_object;

    /// Link to internal data
    JsonFree& current_data() const override
    {
        return  const_cast<JsonFree&>( current_object );
    }

    /// Run current query, rebuild internal table of values
    void update_query() override;

    /// Build default query for collection ( by default all documents )
    DBQueryBase make_default_query_template() const override
    {
        return DBQueryBase(DBQueryBase::qAll);
    }

    /// Build default query fields ( by default internal )
    std::vector<std::string>  make_default_query_fields() const override
    {
        auto fields = to_key_template_fields;
        fields.push_back( "_id" );
        return  fields;
    }

};

} // namespace jsonio


