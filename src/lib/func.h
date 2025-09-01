#ifndef FUNC_H
#define FUNC_H

//local headers
#include "cmore.h"
#include "debug.h"


// -- [meta type]

//external
void cm_meta_type_set(cm_meta_type * value, const void * data);
int cm_meta_type_upd(cm_meta_type * value, const int type_id,
                     const void * data, const size_t sz);
int cm_meta_type_cpy(cm_meta_type * dst_value,
                     const cm_meta_type * src_value);

int cm_new_meta_type(cm_meta_type * value,
                     const int type_id, const void * data, const size_t sz);
void cm_del_meta_type(cm_meta_type * meta_value);



// -- [monad]

//external
int cm_monad_compose(cm_monad * monad,
                     cm_meta_type * (* cb)(cm_meta_type *, void * ctx));
int cm_monad_eval(cm_monad * monad, cm_meta_type * value, void * ctx);

void cm_new_monad(cm_monad * monad);
void cm_del_monad(cm_monad * monad);

#endif
