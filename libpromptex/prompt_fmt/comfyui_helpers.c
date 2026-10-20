
#include <assert.h>
#include <promptex/types.h>
#include "yyjson.h"



yyjson_val* pecmf_get_inputs(yyjson_val* node) {
    assert(node!=NULL);
    yyjson_val* inputs = yyjson_is_obj(node) ? yyjson_obj_get(node, "inputs") : NULL;
    return inputs && yyjson_is_obj(inputs) ? inputs : NULL;
}
