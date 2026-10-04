#ifndef EMBSTRINGS_H
#define EMBSTRINGS_H

#define UTF8 char


/*============================ NamedStr OBJECT ============================*/

/**
 * @struct NamedStr
 * @brief Represents a named string that can be chained to form a linked list.
 * A NamedStr object holds a `name` and a `value`, both of which are UTF-8 strings.
 */
struct NamedStr {
    const UTF8*      name;  /**< The name of the string.  */
    const UTF8*      value; /**< The string value itself. */
    struct NamedStr* _next; /**< Pointer to the next string in the chain (NULL if last). */
};
typedef struct NamedStr  NamedStr;

/**
 * @brief Packs a name and a value into a NamedStr structure.
 *
 * @param name_in_heap  Pointer to the name string allocated in the heap.
 * @param value_in_heap Pointer to the value string allocated in the heap.
 * @param next          Pointer to the next NamedStr in the chain, or NULL.
 * @return Pointer to the newly created NamedStr object.
 */
NamedStr*       pack_namedstring(const UTF8* name_in_heap, const UTF8* value_in_heap, NamedStr* next);

/**
 * @brief Destroys the provided NamedStr object and frees its resources.
 *
 * Note: This function frees the name, value, and the NamedStr structure itself,
 * but it does not remove subsequent objects in the chain.
 *
 * @param namedstr Pointer to the NamedStr object to destroy.
 * @return Pointer to the next object in the chain, or NULL if there is no next object.
 */
const NamedStr* destroy_namedstring(const NamedStr* namedstr);



/*=========================== EmbStrings OBJECT ===========================*/

/**
 * @struct EmbStrings
 * @brief Represents a collection of NamedStr items.
 *
 * EmbStrings holds an array of pointers to NamedStr items, a count of items,
 * and a pointer to the head of the linked chain of NamedStr nodes.
 */
struct EmbStrings {
    NamedStr**  items;  /**< Array of pointers to NamedStr items. */
    int         count;  /**< Number of items in the array. */
    NamedStr*   _chain; /**< Pointer to the head of the linked chain of NamedStr nodes. */
};
typedef struct EmbStrings EmbStrings;

/**
 * @brief Destroys the EmbStrings structure and its associated resources.
 *
 * @param ptr Pointer to the EmbStrings structure to destroy.
 */
extern void destroy_embstrings(const EmbStrings* ptr);

/**
 * @brief Packs a linked chain of NamedStr nodes into an EmbStrings structure.
 *
 * @param chain_in_heap Pointer to the first node of the linked chain.
 * @return Pointer to the newly created EmbStrings structure.
 */
EmbStrings*  pack_embstrings(NamedStr* chain_in_heap);

#endif /* EMBSTRINGS_H */
