#include "jsonio17/exceptions.h"


void jsonio17::JSONIO_THROW(const std::string &title, int id, const std::string &message)
{
    throw jsonio_exception::create(title, id, message);
}

void jsonio17::JSONIO_THROW_IF(bool error, const std::string &title, int id, const std::string &message)
{
    if(error) throw jsonio_exception::create(title, id, message);
}
