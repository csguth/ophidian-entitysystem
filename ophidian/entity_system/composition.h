#ifndef OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
#define OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H

#include "entity_system.h"
#include "property.h"

#include "association.h"

namespace ophidian {
namespace entity_system {


class entity_system;
class composition;

class composition : public association
{
public:
    composition(const entity_system & composite, entity_system & component);
    virtual ~composition();
    void destroy(entity_system::entity en);
    void clear();
};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
