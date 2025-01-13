#pragma once

#include "jsonio/dbedgedoc.h"

namespace jsonio {

/// The function is executed for all vertexes and edges
using  GraphElement_f = std::function<void( bool isVertex,  const std::string& data )>;

/// \class GraphTraversal implementation traversal of graph  for Database.
class GraphTraversal
{
    int trav_type = trAll;
    std::set<std::string> vertex_list = {};
    std::set<std::string> edge_list = {};
    std::shared_ptr<DBVertexDocument> vertex_doc = nullptr;
    std::shared_ptr<DBEdgeDocument> edge_doc = nullptr;

    /// Visit Vertex and all linked Edges
    bool parseVertex( const std::string& idVertex, GraphElement_f afunc );

    /// Visit Edge and all linked Vertexes
    bool parseEdge( const std::string& idEdge, GraphElement_f afunc  );

public:

    /// Types of Matrix table mode
    enum TRAVERCE_TYPES {
        trNo  = 0x0000,      ///< No edges
        trIn  = 0x0001,      ///< Containing all vertexes connected by incoming edges
        trOut = 0x0002,      ///< Containing all vertexes connected by outgoing edges
        trAll = trIn|trOut   ///< Containing all vertexes
       };

    /// Constructor
    /// Params are needed to define DataBase
    GraphTraversal( const DataBase& dbconnect );

    /// Graph Traversal started from one vertex or edge
    void traversal( bool startFromVertex, const std::string& id, GraphElement_f afunc, int atravType = trAll )
    {
      trav_type  =atravType;
      vertex_list.clear();
      edge_list.clear();

      if( startFromVertex )
        parseVertex( id, afunc );
      else
        parseEdge( id, afunc );
    }

    ///  Graph Traversal started from list vertexes or edges
    void Traversal( bool startFromVertex, const std::vector<std::string>& ids,
                    GraphElement_f afunc, int atravType = trAll )
    {
      trav_type  = atravType;
      vertex_list.clear();
      edge_list.clear();

      if( startFromVertex )
      {
          for(auto const &id : ids)
             parseVertex( id, afunc );
      }
      else
      {
          for(auto const &id : ids)
             parseEdge( id, afunc );
      }
      io_logger->info("Traverse {} vertexes and  {} edges ", vertex_list.size(), edge_list.size());
    }

    /// Read graph from JSON file
    void restoreGraphFromFile( const std::string& filePath );

};

} // namespace jsonio

