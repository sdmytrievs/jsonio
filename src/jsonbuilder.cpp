#include "service.h"
#include "jsonbuilder.h"

namespace jsonio14 {

JsonBuilder &JsonBuilder::addScalar(const std::string &key, const std::string &value)
{
    long ival = 0;
    double dval=0.;

    if( value == "~" )
        addNull( key );
    else
        if( value == "null" )
            addNull( key );
        else
            if( value == "true" )
                addBool( key, true );
            else
                if( value == "false" )
                    addBool( key, false );
                else
                    if( is<long>( ival, value ) )
                        addInt(  key, ival );
                    else
                        if( is<double>( dval, value ))
                            addDouble( key, dval );
                        else
                            addString( key, value );
    return *this;
}

} // namespace jsonio14
