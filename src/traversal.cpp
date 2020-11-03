
#include "jsonio17/traversal.h"
#include "jsonio17/dbconnect.h"

namespace jsonio17 {

GraphTraversal::GraphTraversal( const DataBase& dbconnect )
{
   std::string  schemaName = DataBase::getVertexesList()[0];
   vertex_doc = std::make_shared<DBVertexDocument>( schemaName, dbconnect  );
   vertex_doc->setMode(true);

   schemaName = DataBase::getEdgesList()[0];
   edge_doc = std::make_shared<DBEdgeDocument>( schemaName, dbconnect  );
   edge_doc->setMode(true);
}

// visit Vertex and all linked Edges
bool GraphTraversal::parseVertex( const std::string& idVertex, GraphElement_f afunc  )
{
  // Test Vertex is parsed before
  if( vertex_list.find( idVertex ) !=  vertex_list.end() )
    return false;

  std::cout << "idVertex " << idVertex << std::endl;
  vertex_list.insert( idVertex );
  vertex_doc->readDocument( idVertex );
  afunc( true, vertex_doc->getJson() );

  // read all edges
  std::vector<std::string> edgeslst;

  if( trav_type&trOut )
  {
    edgeslst = edge_doc->getOutEdgesKeys( idVertex );
    for(auto const &ent : edgeslst)
       parseEdge( ent,  afunc  );
  }

  if( trav_type&trIn )
  {
    edgeslst = edge_doc->getInEdgesKeys( idVertex );
    for(auto const &ent : edgeslst)
      parseEdge( ent, afunc  );
  }
  return true;

}

//  visit Edge and all linked Vertexes
bool GraphTraversal::parseEdge( const std::string& idEdge, GraphElement_f afunc  )
{
    std::string vertexId, vertexkey;
    // Test Edge is parsed before
    if( edge_list.find( idEdge ) !=  edge_list.end() )
      return false;

    std::cout << "idEdge " << idEdge << std::endl;
    edge_list.insert( idEdge );
    edge_doc->readDocument( idEdge );
    std::string tojson = edge_doc->getJson();

    if( (trav_type&trIn) )
    {
      if( edge_doc->getValueViaPath("_from", vertexId, std::string("")) )
      {
        vertexkey = vertexId;
        //if( vertex_doc->existsDocument( vertexkey ) )
           parseVertex( vertexId, afunc );
      }
    }

    if( (trav_type&trOut) )
    {
     if( edge_doc->getValueViaPath("_to", vertexId, std::string("")) )
     {
        vertexkey = vertexId;
        //if( vertex_doc->existsDocument( vertexkey ) )
               parseVertex( vertexId, afunc );
     }
    }
    afunc( false, tojson );
    return true;
}

void GraphTraversal::restoreGraphFromFile( const std::string& file_path )
{
    std::string type, id;
    std::string cur_record;
    std::cout << " restoreGraphFromFile " << std::endl;

    JsonArrayFile file( file_path);
    file.Open( TxtFile::ReadOnly );
    while( file.loadNext( cur_record ) )
    {
        id = extract_string_json( "_id", cur_record );
        type = extract_string_json( "_type", cur_record );
        std::cout << id << "  " <<  type << std::endl;
        if( type.empty() )
           continue;
        if( type == "edge" )
           edge_doc->updateFromJson( cur_record );
        else
            if( type == "vertex" )
               vertex_doc->updateFromJson( cur_record );
    }
    file.Close();
}


} // namespace jsonio17
