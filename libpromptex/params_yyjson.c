
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <promptex/types.h>
#include "yyjson.h"
#include "params_yyjson.h"
#include "internal.h"





void pe_set_yylong(PeLong* long_, yyjson_val *object, const PeUTF8* key) {
    assert(long_ != NULL);
    if (!object || !key) { return; }

    // Buscar la clave en el objeto JSON
    yyjson_val *val = yyjson_obj_get(object, (const char *)key);
    if (!val) { return; }

    // Enteros con signo
    if (yyjson_is_sint(val)) {
        int64_t sval = yyjson_get_sint(val);
        long_->has_value = true;
        long_->value     = (long)sval;
        long_->_converr  = (sval < LONG_MIN || LONG_MAX < sval);
    } 
    // Enteros sin signo
    else if (yyjson_is_uint(val)) {
        uint64_t uval = yyjson_get_uint(val);
        long_->has_value = true;
        long_->value     = (long)uval;
        long_->_converr  = ((uint64_t)LONG_MAX < uval);
    } 
    // Números de punto flotante
    else if (yyjson_is_real(val)) {
        double dval = yyjson_get_real(val);
        long_->has_value = true;
        long_->value     = (long)dval;
        long_->_converr  = (dval < (double)LONG_MIN || (double)LONG_MAX < dval);
    } 
    // Booleanos (true -> 1, false -> 0)
    else if (yyjson_is_bool(val)) {
        long_->has_value = true;
        long_->value     = yyjson_get_bool(val) ? 1L : 0L;
        long_->_converr  = false;
    }
    // Conversión desde cadenas de texto (String -> long)
    else if (yyjson_is_str(val)) {
        char *endptr;
        const char *str_value = yyjson_get_str(val);

        // saltar espacios en blanco
        while (pe_isspace(*str_value)) { str_value++;  }
        
        // convertir a long y verificar que no haya errores
        errno = 0;
        long_->has_value = true;
        long_->value     = strtol(str_value, &endptr, 10);
        long_->_converr  = (endptr == str_value) || (errno == ERANGE);
    }

    // Cualquier otro tipo no soportado (null, arrays, objetos)
    // deja a la estructura intacta sin modificar
}