/** Apache Thrift IDL definition for text import-export interfaces */
/** PMATCH++ prototype   April 2016 */
/** Created by KD 03-4-2016, SD 13-10-2016 */

namespace * impex


/** Classes of direction types */
enum DirectionType {
    /** import */
    IMPORT
    /** export */
    EXPORT
    /** update */
    UPDATE
}

// /** Intermediate value to be read from file (import) or to be printed to file (export) */

/** Definition of the data value format in imported/exported file */
struct FormatValue
{
   /** Format scanf/printf (to string first): "%s" | "in" | "out" | "endl" | "txel" | "txkw"; "in" | "out" | "endl" for stream input*/
   1: string format
   /** Factor != 0, default 1; Each num.value is multiplied (import) or divided (export) by factor */
   2: optional double factor
   /** Increment, default 0; added to each numerical value (import) or subtracted from (export) */
   3: optional double increment
}

/** Format to read/print keywords in key-value pair file */
struct FormatKeyword {
   /** scanf/printf format for keyword */
   1: string format
   /** delimiter for keyword begin e.g. "\"" | "<" | "" */
   2: optional string delim_begin
   /** delimiter for keyword end e.g. "\"" | ">" | "" */
   3: optional string delim_end
}

/** Type of object from the imported or exported file (for use in keyword lookup list or map) */
struct DataType {
   /** Basis type "string" | "float" | "double" | "i16" | "i32" | "bool" | ... */
   1: string datatype
   /** Organization: "" | "list" | "set" | "map" | "group" | "embedded" | ...  */
   2: string organization
   /** Dimensions (>0 for arrays): "0" | 1" | "2" | ... */
}

/** Thrift key of data object "8" or "3.8" or "2.3.8" or "" to ignore (import); any string not starting from a digit as comment (export) */
struct DataObject {
   /** Either Thrift key or name of the data field in recursive form (s e.g. "4.3.1" or name1.name2 ) */
   1: string field
   /** Set to true if the corresponding value in file has to be ignored (default: false) */
   2: optional bool ignore
   /** Default "" or contains lua script for operation on data values in block  */
   3: optional string script
   /** Default empty or contains pair(s) read_value : saved_value e.g. "e": "4" (usually for setting enum values)  */
   4: optional map<string,string> convert
   // may need
}

/** Definition of value, line, row, block, comment, end-of-data separators */
struct Separators {
   /** Value separator (for arrays) " " | "," | "\t" | "integer" (=fixed field width) */
   1: string v_sep
   /** Line separator "\n" ... */
   2: string l_sep
   /** Row separator (table), "\n" ... */
   3: optional string r_sep
   /** Head comment separator  e.g. "#" or '%' */
   4: optional string c_head
   /** End comment separator e.g. "\n" */
   5: optional string c_end
   /** string indicating end of data (as list of blocks) in file or "" as default ']' (end of file) */
   6: optional string eod
   /** encoding ("" for standard system encoding) */
   7: optional string encoding
   /** Delimiter for strings - default "\"" */
   8: optional string str_delim
   /** string indicating begin of data (as list of blocks) in file or "" as default '[' */
   9: optional string bod
}

/** Text block format in file corresponding to one database document (record) */
struct FormatBlock {
   /** Default Key, Value pairs to DOM (import)  or to output (export) */
   1: map<string,string> defaults
   /** Lookup map of keyword-value pair format */
   2: map<string,DataType> pairs
   /** >=1 keywd, DataObject pairs connecting the block of data in file with DOM. */
   3: required map<string,DataObject> matches
   /** Default "" or contains lua script for operation on data values in full DOM */
   4: optional string script
}

// Export/import of each data record to/from (formatted) text file with strict data order
//

/** Definition of text data file format */
struct FormatTextFile {
   /** Format definition for one or more blocks for data records - default 1 block */
   1: required FormatBlock block
   /** Will be format lines list */
   2: required list<string> lines
   /** Label of data type (vertex type), e.g. "datasource", "element" ... */
   3: required string label
   /** Definition of value, line, row, block, comment, end-of-data separators */
   4: Separators separs
   /** Export: the whole comment text; Import: the comment begin markup string (to skip until endl) */
   5: optional string comment
   /** File name or "console" for export */
   6: optional string file_name
   /** number of data block in file >=1, 0 if unknown */
   7: optional i32 Nblocks
   /** number of text lines in file (>=1), 0 if unknown */
   8: optional i32 Nlines
   /** total number of characters in file, 0 if unknown */
   9: optional i32 Nchars
   /** direction */
   10: DirectionType direction
}

// Export/import of each data record to/from text file organized in blocks of key-value pairs
// Key-Value pairs may follow in arbitrary order in the input (imported) file
//

/** Definition of key-value import/export format
    We use Regular Expression in case of import and Print Format in case of export.
    Use names "head0", ..., "headN", "end0", ..., "endN" to import/export data from/to head and end part
*/
struct FormatKeyValue {
    /** Head of Block:  "\\s*([^\\s]+)\\s*;\\s*(([\\w\\t \\+\\-\\(\\):\\.]+)\\s*=\\s*([^;]+))" (import)
                         or  "\n%head0\n\t%head1\n" (export)   */
    1: required string head_regexp
    /** End of Block: "([^\n]+)" (import)  or  "%end0\n" (export) */
    2: required string end_regexp
    /** Keyword:  "\\s*;\\s*\\-{0,1}([a-zA-Z]\\w*)\\s*" (import)  or  "\t-%key\t" (export)  */
    3: required string key_regexp
    /** Data Value(s):  "\\s*([^#\\n;]*)" (import)  or  "%value" (export)  */
    4: string value_regexp
    /** Key-Value pair end delimiter (used if empty value_regexp or export mode )  */
    5: string value_next = "\\n"
    /** Regular Expression to iterate over matches  (used to convert value to string list or if export mode ) */
    6: optional string value_token_regexp = " "
    /** Regular Expression for skip comments */
    7: optional string comment_regexp
    /** number of data items per block (0 if not set) */
    8: optional i32 Ndata
    /** Data String Value(s): only for export  "\'%value\'"  */
    9: string strvalue_exp
    /** Keyword order list:  only for export */
    10: optional list<string> key_order
}

/** Definition of text file with key-value pair data */
struct FormatKeyValueFile {
   /** Format for one or more blocks for data records */
   1: required FormatBlock block
   /** Definition of key-value block in file */
   2: required  FormatKeyValue format
   /** Rendering syntax for the foreign key-value file "GEMS3K" | "BIB" | "RIS" | ... */
   3: required string renderer
   /** Label of data type (vertex type), e.g. "datasource", "element" ... */
   4: required string label
   /** Definition of value, line, row, block, comment, end-of-data separators */
   5: Separators separators
   /** Export: the whole comment text; Import: the comment begin markup string (to skip until endl) */
   6: optional string comment
   /** File name or "console" for export */
   7: optional string fname
   /** number of data blocks (records) >=1, 0 if unknown */
   8: optional i32 Nblocks
   /** total number of text lines in the file, 0 if unknown */
   9: optional i32 Nlines
   /** direction */
   10: DirectionType direction
}

// Export/import of data records to table format file (strict order of columns and rows)
//

/** Definition of of the table input format
    if defined  colsizes, split by sizes
    if defined  value_regexp, split used regexpr
    otherwise split by  columns delimiter
*/
struct FormatTable {
     /** Number of header columns */
     1: i32 Nhcols
     /** Number of header rows (start porting from row ) */
     2: i32 Nhrows
     /** Names of header columns */
     3: list<string> headers

     /** Row delimiter  */
     4: string rowend = "\\n"
     /** Number of lines in one block (could be more than one) */
     5: i32 rows_one_block = 1
     /** Regular expression:  next block head in case the number of lines in one block is not fixed  */
     6: string row_header_regexp = ""
     /** Columns delimiters  */
     7: string colends = "\\t "
     /** Quoted field as text */
     8: bool   usequotes = true
     /** Can be more than one delimiter between columns */
     9: bool   usemore = false
     /** Regular Expression for skip comments */
     10: optional string comment_regexp

     /** Row size in case of using fixed colsizes */
     11: optional i32 row_size = 0
     /** Fixed size of columns for importing ( apply to all if one item ) */
     12: optional list<i32> colsizes
     /** Regular Expression for column value(s) ( apply to all if one item ) */
     13: optional list<string> value_regexp
}

/** Definition of table text file format */
struct FormatTableFile {
   /** Format for one or more blocks for data records */
   1: required FormatBlock block
   /** Definition of key-value block in file */
   2: required  FormatTable format
   /** Rendering syntax for the foreign key-value file "GEMS3K" | "BIB" | "RIS" | ... */
   3: required string renderer
   /** Label of data type (vertex type), e.g. "datasource", "element" ... */
   4: required string label
   /** Definition of value, line, row, block, comment, end-of-data separators */
   5: Separators separators
   /** Export: the whole comment text; Import: the comment begin markup string (to skip until endl) */
   6: optional string comment
   /** File name or "console" for export */
   7: optional string fname
   /** number of data blocks (records) >=1, 0 if unknown */
   8: optional i32 Nblocks
   /** total number of text lines in the file, 0 if unknown */
   9: optional i32 Nlines
   /** direction */
   10: DirectionType direction
}


// Export/import of data records to/from text file in legacy JSON/YAML/XML (foreign keywords)
// Maybe using a generated JSON schema of that input file format?
//

/** Definition of foreign structured data JSON/YAML/XML text file */
struct FormatStructDataFile {
   /** Format for one or more blocks for data records */
   1: required FormatBlock block
   /** Rendering syntax for the foreign file "JSON" | "YAML" | "XML" | ... */
   2: required string renderer = "JSON"
   /** Label of data type (vertex type), e.g. "datasource", "element" ... */
   3: required string label
   /** Definition of value, line, row, block, comment, end-of-data separators */
   4: optional string comment
   /** File name or "console" for export */
   5: optional string fname
   /** number of data blocks (records) >=1, 0 if unknown */
   6: optional i32 Nblocks
   /** total number of text lines in the file, 0 if unknown */
   7: optional i32 Nlines
   /** direction */
   10: DirectionType direction
}

/** Generalized import-export data file format */
union FormatImportExportFile
{
   /** Definition of text data file format */
   1: FormatTextFile       ff_text
   /** Definition of data table file format */
   2: FormatTableFile       ff_table
   /** Definition of file format with key-value pair data */
   3: FormatKeyValueFile    ff_keyvalue
   /** Definition of foreign structured data JSON/YAML/XML file format */
   4: FormatStructDataFile  ff_stdata
   //	...
}


// Variant for ArangoDB backend
 /** description of import/export format record */
struct ImpexFormat {   // Database record of type impex
    /** Handle (id) of this record or 0 if unknown */
    1: required string _id
    /** ID of this record within the impex collection (part of _id) */
    2: required string _key
    /** Code of revision (changed by the system at every update) */
    3: required string _rev
    /** short description/keywd (used as key field) */
    4: required string name
    /** impex schema  */
    5: required string impexschema
    /** record schema */
    6: string schema
    /** description of record */
    7: string comment
    /** direction */
    8: DirectionType direction
    /** format structure */
    9: required string impex
    /** Id/description of foreign file format */
    10: optional string format
    /** file extension */
    11: optional string extension
}

// End of file impex.thrift - - - - - - - - - - - - - - - - - - - - - - - - - - -
