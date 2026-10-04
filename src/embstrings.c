#include <stdlib.h>
#include "embstrings.h"
#define NamedStrPointer NamedStr*


/**
 * Packs a name and a value into a NamedStr structure.
 *
 * Both `name_in_heap` and `value_in_heap` must have been allocated on the heap
 * (e.g., using malloc, strdup, etc.). The returned `NamedStr` object becomes
 * the owner of both `name_in_heap` and `value_in_heap`, so they do not need to
 * be freed separately.
 *
 * @param name_in_heap  Pointer to the name string allocated in the heap.
 * @param value_in_heap Pointer to the value string allocated in the heap.
 * @param chain         Pointer to the NamedStr node where the new object will be chained.
 *                      If it should not be chained to any node, pass `NULL`.
 * @return Pointer to the newly created NamedStr object.
 */
NamedStr* pack_namedstring(const UTF8* name_in_heap,
                           const UTF8* value_in_heap,
                           NamedStr*   chain)
{
    NamedStr* namedstr = malloc(sizeof(NamedStr));
    namedstr->name  = name_in_heap;
    namedstr->value = value_in_heap;
    namedstr->_next = chain;
    return namedstr;
}


/**
 * Destroys the provided NamedStr object and frees its resources.
 *
 * ATTENTION: It does not remove the subsequent objects in the chain.
 *
 * @param namedstr Pointer to the NamedStr object to destroy.
 * @return Pointer to the next object in the chain,
 *         or `NULL` if there is no next object.
 */
const NamedStr* destroy_namedstring(const NamedStr* namedstr)
{
    if( !namedstr ) {
        return NULL;
    }
    NamedStr* next_in_chain = namedstr->_next;
    free( (void*)namedstr->name  );
    free( (void*)namedstr->value );
    free( (void*)namedstr );
    return next_in_chain;
}



/**
 * Packs a linked chain of NamedStr nodes into an EmbStrings structure.
 *
 * Allocates memory for an EmbStrings structure and populates it with the
 * provided chain of NamedStr nodes. It counts the nodes, allocates an
 * array of pointers to hold them, and fills the array in reverse order
 * (from last to first) while preserving the original chain order.
 *
 * @param chain_in_heap Pointer to the first node of the linked chain.
 * @return Pointer to the newly created EmbStrings structure.
 */
EmbStrings* pack_embstrings(NamedStr* chain_in_heap)
{
    NamedStr* current;
    EmbStrings* embstrings = malloc(sizeof(EmbStrings));

    // count the number of elements in the chain
    current = chain_in_heap;
    int count = 0; while (current != NULL) {
        count++;
        current = current->_next;
    }

    // initialize EmbStrings and allocate space for 'items' to hold all chain elements
    embstrings->items  = malloc(sizeof(NamedStrPointer) * count);
    embstrings->count  = count;
    embstrings->_chain = chain_in_heap;

    // populate the 'items' array with each element from the chain
    current = chain_in_heap;
    int idx = count; while (current != NULL) {
        embstrings->items[--idx] = current;
        current = current->_next;
    }

    return embstrings;
}


void destroy_embstrings(const EmbStrings* self)
{
    // destroy each element in the chain
    const NamedStr* current = self->_chain;
    while (current != NULL) {
        current = destroy_namedstring(current);
    }
    
    // free the array of pointers and the EmbStrings itself
    free((void*)self->items);
    free((void*)self);
}
