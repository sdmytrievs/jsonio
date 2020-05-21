#pragma once

#include <functional>
#include <memory>
#include <map>
#include <set>
#include "jsonio14/service.h"

namespace arangocpp {
class ArangoDBQuery;
}

namespace jsonio14 {

class JsonBase;
class JsonFree;

/// Map of query fields  <field name>-><field into json record>
/// Used to generate return values for AQL
using fields2query_t = std::map<std::string, std::string >;

/// Record values type
using values_t = std::vector<std::string>;

/// Lines of colums values table
using values_table_t = std::vector< values_t >;

/// Table of results <key>-><extracted values>
using key_values_table_t = std::map<std::string, values_t >;

/// Map of fields values  <field path>-><value from into record>
using field_value_map_t = std::map<std::string, std::string >;

/// Function for testing the matching of the record key to the template
using MatchKeyTemplate_f = std::function<bool( const std::string& keypat, const std::string& akey )>;

/// \class DBQueryBase describing the query and query parameters.
/// Used to retrieve data that are stored in the DataBase.
/// Query: the query which you want explained;
/// If the query references any bind variables, these must also be passed in the attribute bindVars.
/// Additional options for the query can be passed in the options attribute.
class DBQueryBase
{

public:

    enum QType
    {
        qUndef = -1,
        qTemplate = 0 /* most of old queries*/,
        qAll = 1,
        qEdgesFrom = 2,
        qEdgesTo = 3,
        qEdgesAll = 4,
        qAQL = 6,
        qEJDB = 7
    };


    /// Empty or All  query constructor.
    explicit DBQueryBase( QType atype = qAll  );

    /// An AQL query text or json template constructor.
    DBQueryBase( const std::string& condition, QType atype );

    /// Copy constructor
    DBQueryBase( const DBQueryBase& data) = default;
    /// Move constructor
    DBQueryBase( DBQueryBase&& data) = default;
    /// Copy assignment
    DBQueryBase &operator =( const DBQueryBase &other) = default;
    /// Move assignment
    DBQueryBase &operator =( DBQueryBase &&other)= default;

    ///  Destructor
    virtual ~DBQueryBase()
    {}

    virtual void toJson( JsonBase& object ) const;
    virtual void fromJson( const JsonBase& object );

    /// Test for empty query.
    virtual bool empty() const;

    /// Get query type.
    virtual QType type() const;

    /// Get string with query ( an AQL query text or json template  ).
    virtual const std::string& queryString() const;

    /// Set json string with the bind parameter values need to be passed
    /// along with the query when it is executed.
    virtual void  setBindVars( const std::string& json_bind );

    /// Set json string with the bind parameter values need to be passed
    /// along with the query when it is executed.
    void  setBindVars( const JsonBase& bind_object );

    /// Get the json string with bind values used in the query
    virtual const std::string& bindVars() const;

    /// Set json string with the  key/value object with extra options for the query
    ///  need to be passed along with the query when it is executed.
    virtual void  setOptions( const std::string& json_options );

    /// Get the json string with  key/value object with extra options for the query.
    virtual const std::string& options() const;

    /// Set the fixed set of attributes from the collection is queried,
    /// then the query result values will have a homogeneous structure.
    virtual void  setQueryFields( const fields2query_t& map_fields );

    /// Get the fixed set of attributes from the collection is queried.
    virtual const fields2query_t& queryFields() const;

    template <typename Container>
    void  setQueryFields( const Container& list_fields )
    {
        fields2query_t map_fields;

        typename Container::const_iterator itr = list_fields.begin();
        typename Container::const_iterator end = list_fields.end();
        for (; itr != end; ++itr)
        {
            std::string fld = *itr;
            replace_all( fld, ".", '_');
            map_fields[fld] = *itr;
        }
        setQueryFields( map_fields );
    }

protected:

    /// ArangoDB query data
    std::shared_ptr<arangocpp::ArangoDBQuery> arando_query = nullptr;

    friend class ArangoDBClient;
};

/// \class DBQueryDef is a query description into a Database record.
class DBQueryDef final
{

public:

    /// Constructor
    DBQueryDef( const std::shared_ptr<DBQueryBase>& condition, const std::vector<std::string>& fields = {} ):
        fields_collect(fields), query_condition(condition)
    { }

    void toJson( JsonFree& object ) const;
    void fromJson( const JsonBase& object );

    void setName( const std::string& name )
    {
        key_name = name;
    }
    const std::string& name() const
    {
        return key_name;
    }

    void setComment( const std::string& comm )
    {
        rec_comment = comm;
    }
    const std::string& comment() const
    {
        return rec_comment;
    }

    void setSchema( const std::string& shname )
    {
        toschema = shname;
    }
    const std::string& schema() const
    {
        return toschema;
    }

    void setCondition( const std::shared_ptr<DBQueryBase>& condition )
    {
        query_condition= condition;
    }
    const DBQueryBase* condition() const
    {
        return query_condition.get();
    }

    void setFields(const std::vector<std::string>& fields )
    {
        fields_collect = fields;
    }
    template <typename Container>
    void  addFields( const Container& listFields )
    {
        fields_collect.insert(fields_collect.end(), listFields.begin(), listFields.end());
    }
    const std::vector<std::string>& fields() const
    {
        return fields_collect;
    }

protected:

    /// Short name of query (used as key field)
    std::string key_name;
    /// Description of query
    std::string rec_comment;
    /// Schema for query about
    std::string toschema;
    /// List of fieldpaths to collect
    std::vector<std::string> fields_collect;

    /// Query is used to retrieve data that are stored in DataBase
    std::shared_ptr<DBQueryBase> query_condition;
};

/// \class  DBQueryResult used to store query definition and result.
class DBQueryResult final
{
    friend class DBDocumentBase;
    friend class DBJsonDocument;
    friend class DBSchemaDocument;

public:

    DBQueryResult( const DBQueryDef& aquery ):
        query_data(aquery)
    { }
    ~DBQueryResult() {}


    const DBQueryDef& query() const
    {
        return query_data;
    }

    const DBQueryBase& condition() const
    {
        return *query_data.condition();
    }

    void setQuery( const DBQueryDef& qrdef )
    {
        query_data = qrdef;
        query_result.clear();
    }
    void updateSchema( const std::string& shname )
    {
        query_data.setSchema(shname);
    }

    void clear()
    {
        query_result.clear();
    }

    /// Get query result table
    const key_values_table_t& queryResult() const
    {
        return query_result;
    }

    ///  Get all keys list for current query
    std::size_t getKeysValues( std::vector<std::string>& aKeyList,
                               std::vector<values_t>& aValList ) const;

    /// Get keys list for current query and defined condition function
    std::size_t getKeysValues( std::vector<std::string>& aKeyList, std::vector<values_t>& aValList,
                               const char* keypart, MatchKeyTemplate_f compareTemplate ) const;

    /// Get keys list for current query and defined field values
    std::size_t getKeysValues( std::vector<std::string>& aKeyList, std::vector<values_t>& aValList,
                               const std::vector<std::string>& fieldnames, const std::vector<std::string>& fieldvalues ) const;

    /// Extract key from data
    std::string getKeyFromValue( const JsonBase& node ) const;

    /// Extract first key from data
    std::string getFirstKey() const;

protected:

    /// Description query
    DBQueryDef      query_data;
    /// Table of values were gotten from query
    key_values_table_t  query_result;

    /// Make line to view table
    void node_to_values(  const JsonBase& node, values_t& values ) const;

    /// Add line to view table
    void add_line( const std::string& key_str,  const JsonBase& nodedata, bool isupdate );
    /// Update line into view table
    void update_line( const std::string& key_str,  const JsonBase& nodedata );
    /// Delete line from view table
    void delete_line( const std::string& key_str );

};

} // namespace jsonio14

