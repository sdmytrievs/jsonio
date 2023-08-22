/** Apache Thrift IDL definition for the query service interface */
/** bsonio prototype   January 2017 */
/** Revised by SD 20-01-2017, DK 06-07-2018 */

namespace * docpages


/** Types of Help files */
enum DocFileTypes {
    /** Markdown text file */
    Markdown
    /** Image binary file */
    Image
}

 /** description of  Help record ( Markdown or image file ) */
struct DocPage {   // Database record for help content
    /** id of this help record or 0 if unknown */
    1: required string _id
    /** ID of this record within the impex collection (part of _id) */
    2: required string _key
    /** Code of revision (changed by the system at every update) */
    3: required string _rev
    /** name of file (used as key field) */
    4: required string name
    /** extension of file (used as key field) */
    5: required string ext
    /** type of content (used as key field) */
    6: required DocFileTypes type
    /** markdown content  */
    7: string markdown
    /** image content  */
    8: string image
}

/** Definition of an array of documentation pages or images */
struct Docpages {
    
 //   1: required string _key = "Docpages"

    /** List of docpages or images */ 
    2: list<Docpage> docpages
    
}

// End of file docpages.thrift - - - - - - - - - - - - - - - - - - - - - - - - - -
