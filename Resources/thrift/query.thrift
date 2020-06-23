/** Apache Thrift IDL definition for the query service interface */
/** PMATCH++ prototype   January 2016 */
/** Revised by SD 13-10-2016 */

namespace * query

/** Classes of query style */
enum QueryStyle {
  /** Undefined query */
  QUndef = -1
  /** Fields template query */
  QTemplate = 0
  /** Select all records */
  QAll = 1
  /** Select outgoing edges */
  QEdgesFrom = 2
  /** Select incoming edges */
  QEdgesTo = 3
  /** Select edges by query */
  QEdgesAll = 5
  /** AQL-style query */
  QAQL = 6
  /** EJDB-style query */
  QEJDB = 7
}

/** Struct for query condition */
struct ConditionData
{
  /** query style */
  1: required  QueryStyle style
  /** query string */
  2: string find
  /** bind values string */
  3: string bind
  /** return fields map */
  4: map<string,string> fields
  /** options values string */
  5: string options
}

 /** description of query record */
struct Query {   // Database record of type query
    /** id of this query record or 0 if unknown */
    1: required string _id
    /** ID of this record within the impex collection (part of _id) */
    2: required string _key
    /** Code of revision (changed by the system at every update) */
    3: required string _rev
    /** short name of query (used as key field) */
    4: required string name
    /** description of query */
    5: string comment
    /** query filter condtition */
    6: required  ConditionData condition
    /** list of fields to collect */
    7: required list<string> collect
    /** query for schema */
    8: string qschema
    /** query generator state */
    9: string filter_generator
}


// End of file query.thrift - - - - - - - - - - - - - - - - - - - - - - - - - -
