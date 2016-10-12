#include <stdio.h>
#include <string.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <mathsat.h>

struct custom_operations configs;
struct custom_operations envs;
struct custom_operations terms;
struct custom_operations types;
struct custom_operations decls;
struct custom_operations models;

/* macro definitions, borrowed from the BuDDy OCaml bindings:
   https://github.com/abate/ocaml-buddy
 */

#define CONFIG_val(v) (*((msat_config*)Data_custom_val(v)))
#define ENV_val(v) (*((msat_env*)Data_custom_val(v)))
#define TERM_val(v) (*((msat_term*)Data_custom_val(v)))
#define TYPE_val(v) (*((msat_type*)Data_custom_val(v)))
#define DECL_val(v) (*((msat_decl*)Data_custom_val(v)))
#define MPQ_val(v) (*((mpq_t*)Data_custom_val(v)))
#define MODEL_val(v) (*((msat_model*)Data_custom_val(v)))

#define RESULT_val(v) Int_val(v)

void _make_config(value* vptr, msat_config x) {
    *vptr = alloc_custom(&configs, sizeof(msat_config), 0, 1);
    CONFIG_val(*vptr) = x;
}
void _make_env(value* vptr, msat_env x) {
    *vptr = alloc_custom(&envs, sizeof(msat_env), 0, 1);
    ENV_val(*vptr) = x;
}
void _make_term(value* vptr, msat_term x) {
    *vptr = alloc_custom(&terms, sizeof(msat_term), 0, 1);
    TERM_val(*vptr) = x;
}
void _make_type(value* vptr, msat_type x) {
    *vptr = alloc_custom(&types, sizeof(msat_type), 0, 1);
    TYPE_val(*vptr) = x;
}
void _make_decl(value* vptr, msat_decl x) {
    *vptr = alloc_custom(&decls, sizeof(msat_decl), 0, 1);
    DECL_val(*vptr) = x;
}
void _make_model(value* vptr, msat_model x) {
    *vptr = alloc_custom(&models, sizeof(msat_model), 0, 1);
    MODEL_val(*vptr) = x;
}

#define FUN_ARG_msat_config(x, v) \
    msat_config x = CONFIG_val(v);
#define FUN_ARG_msat_env(x, v) \
    msat_env x = ENV_val(v);
#define FUN_ARG_msat_term(x, v) \
    msat_term x = TERM_val(v);
#define FUN_ARG_msat_type(x, v) \
    msat_type x = TYPE_val(v);
#define FUN_ARG_msat_decl(x, v) \
    msat_decl x = DECL_val(v);
#define FUN_ARG_msat_model(x, v) \
    msat_model x = MODEL_val(v);
#define FUN_ARG_int(x, v) \
  int x = Int_val(v);
#define FUN_ARG_string(x, v) \
  char *x = String_val(v)

#define FUN_RET_int(eval) \
  CAMLreturn(Val_int(eval));
#define FUN_RET_bool(eval) \
  CAMLreturn(Val_bool(eval));
#define FUN_RET_unit(eval) \
  eval; \
  CAMLreturn0;
// SPACE LEAK!
#define FUN_RET_string(eval)			\
    CAMLlocal1(r);				\
    r = caml_copy_string(eval);			\
    CAMLreturn(r)
#define FUN_RET_msat_config(eval) \
    CAMLlocal1(r);		  \
    _make_config(&r, eval);	  \
    CAMLreturn(r);
#define FUN_RET_msat_env(eval) \
    CAMLlocal1(r);	       \
    _make_env(&r, eval);       \
    CAMLreturn(r);
#define FUN_RET_msat_term(eval) \
    CAMLlocal1(r);	       \
    _make_term(&r, eval);       \
    CAMLreturn(r);
#define FUN_RET_msat_type(eval) \
    CAMLlocal1(r);	       \
    _make_type(&r, eval);       \
    CAMLreturn(r);
#define FUN_RET_msat_decl(eval) \
    CAMLlocal1(r);		\
    _make_decl(&r, eval);       \
    CAMLreturn(r);
#define FUN_RET_msat_model(eval) \
    CAMLlocal1(r);		\
    _make_model(&r, eval);       \
    CAMLreturn(r);
#define FUN_RET_msat_result(eval) \
    CAMLreturn(Val_int(eval))

#define FUN0(name, ret_type) \
CAMLprim value wrapper_##name() \
{  \
  CAMLparam0(); \
  FUN_RET_##ret_type(name()); \
}

/* same as above but returns void to avoid a compiler warning */
#define FUN00(name, ret_type) \
void wrapper_##name() \
{  \
  CAMLparam0(); \
  FUN_RET_##ret_type(name()); \
}

#define FUN1(name, arg0_type, ret_type) \
CAMLprim value wrapper_##name(value v0) \
{  \
  CAMLparam1(v0); \
  FUN_ARG_##arg0_type(x, v0); \
  FUN_RET_##ret_type(name(x)); \
}

#define FUN11(name, arg0_type, ret_type) \
void wrapper_##name(value v0) \
{  \
  CAMLparam1(v0); \
  FUN_ARG_##arg0_type(x, v0); \
  FUN_RET_##ret_type(name(x)); \
}

#define FUN2(name, arg0_type, arg1_type, ret_type)  \
CAMLprim value wrapper_##name(value v0, value v1) \
{ \
  CAMLparam2(v0, v1);  \
  FUN_ARG_##arg0_type(x, v0); \
  FUN_ARG_##arg1_type(y, v1); \
  FUN_RET_##ret_type(name(x, y)); \
}

#define FUN3(name, arg0_type, arg1_type, arg2_type, ret_type) \
CAMLprim value wrapper_##name(value v0, value v1, value v2) \
{ \
  CAMLparam3(v0, v1, v2); \
  FUN_ARG_##arg0_type(x, v0); \
  FUN_ARG_##arg1_type(y, v1); \
  FUN_ARG_##arg2_type(z, v2); \
  FUN_RET_##ret_type(name(x, y, z)); \
}

#define FUN4(name, arg0_type, arg1_type, arg2_type, arg3_type, ret_type) \
  CAMLprim value wrapper_##name(value v0, value v1, value v2, value v3) \
{ \
  CAMLparam4(v0, v1, v2, v3); \
  FUN_ARG_##arg0_type(x, v0); \
  FUN_ARG_##arg1_type(y, v1); \
  FUN_ARG_##arg2_type(z, v2); \
  FUN_ARG_##arg3_type(w, v3); \
  FUN_RET_##ret_type(name(x, y, z, w)); \
}

static inline value tuple( value a, value b) {
  CAMLparam2( a, b );
  CAMLlocal1( tuple );

  tuple = caml_alloc(2, 0);

  Store_field( tuple, 0, a );
  Store_field( tuple, 1, b );

  CAMLreturn(tuple);
}

static inline value append( value hd, value tl ) {
  CAMLparam2( hd , tl );
  CAMLreturn(tuple( hd, tl ));
}

static inline size_t length (value l) {
  size_t len = 0;
  while (l != Val_emptylist) { len++ ; l = Field(l, 1); }
  return len;
}


FUN0(msat_create_config, msat_config)
FUN1(msat_parse_config, string, msat_config)
FUN1(msat_destroy_config, msat_config, unit)
FUN1(msat_create_env, msat_config, msat_env)
FUN2(msat_create_shared_env, msat_config, msat_env, msat_env)
FUN1(msat_destroy_env, msat_env, unit)
CAMLprim value wrapper_msat_gc_env(value env, value tokeep) {
    CAMLparam2(env, tokeep);
    size_t len = length(tokeep);
    msat_term keep[len];
    int i = 0;
    while (tokeep != Val_emptylist) {
	keep[i++] = TERM_val(Field(tokeep, 0));
	tokeep = Field(tokeep, 1);
    }
    CAMLreturn(Val_int(msat_gc_env(ENV_val(env), keep, len)));
}


FUN3(msat_set_option, msat_config, string, string, int)

FUN1(msat_get_bool_type, msat_env, msat_type)
FUN1(msat_get_rational_type, msat_env, msat_type)
FUN1(msat_get_integer_type, msat_env, msat_type)

CAMLprim value wrapper_msat_get_function_type(value env, value args, value ret) {
    CAMLparam3(env, args, ret);
    size_t len = length(args);
    msat_type arg_types[len];
    int i = 0;
    while (args != Val_emptylist) {
	arg_types[i++] = TYPE_val(Field(args, 0));
	args = Field(args, 1);
    }
    FUN_RET_msat_type(msat_get_function_type(ENV_val(env),
					     arg_types,
					     len,
					     TYPE_val(ret)));
}

FUN2(msat_is_bool_type, msat_env, msat_type, bool)
FUN2(msat_is_rational_type, msat_env, msat_type, bool)
FUN2(msat_is_integer_type, msat_env, msat_type, bool)

FUN3(msat_declare_function, msat_env, string, msat_type, msat_decl)

/* Term creation */
FUN1(msat_make_true, msat_env, msat_term)
FUN1(msat_make_false, msat_env, msat_term)
FUN3(msat_make_iff, msat_env, msat_term, msat_term, msat_term)
FUN3(msat_make_or, msat_env, msat_term, msat_term, msat_term)
FUN3(msat_make_and, msat_env, msat_term, msat_term, msat_term)
FUN2(msat_make_not, msat_env, msat_term, msat_term)
FUN3(msat_make_equal, msat_env, msat_term, msat_term, msat_term)
FUN3(msat_make_leq, msat_env, msat_term, msat_term, msat_term)
FUN3(msat_make_plus, msat_env, msat_term, msat_term, msat_term)
FUN3(msat_make_times, msat_env, msat_term, msat_term, msat_term)
FUN2(msat_make_floor, msat_env, msat_term, msat_term)
FUN2(msat_make_number, msat_env, string, msat_term)
FUN4(msat_make_term_ite, msat_env, msat_term, msat_term, msat_term, msat_term)
FUN2(msat_make_constant, msat_env, msat_decl, msat_term)

CAMLprim value wrapper_msat_make_uf(value env, value decl, value cargs) {
    CAMLparam3(env, decl, cargs);
    size_t len = length(cargs);
    msat_term args[len];
    int i = 0;
    while (cargs != Val_emptylist) {
	args[i++] = TERM_val(Field(cargs, 0));
	cargs = Field(cargs, 1);
    }
    FUN_RET_msat_term(msat_make_uf(ENV_val(env), DECL_val(decl), args));
}

FUN3(msat_make_copy_from, msat_env, msat_term, msat_env, msat_term)

/* Term access and navigation */
FUN1(msat_term_id, msat_term, int)
FUN1(msat_term_arity, msat_term, int)
FUN2(msat_term_get_arg, msat_term, int, msat_term)
FUN1(msat_term_get_type, msat_term, msat_type)
FUN2(msat_term_is_true, msat_env, msat_term, bool)
FUN2(msat_term_is_false, msat_env, msat_term, bool)
FUN2(msat_term_is_boolean_constant, msat_env, msat_term, bool)
FUN2(msat_term_is_atom, msat_env, msat_term, bool)
FUN2(msat_term_is_number, msat_env, msat_term, bool)
CAMLprim value wrapper_msat_term_to_number(value env, value term, value mpq) {
    CAMLparam3(env, term, mpq);
    int ret = msat_term_to_number(ENV_val(env), TERM_val(term), MPQ_val(mpq));
    FUN_RET_int(ret);
}

FUN2(msat_term_is_and, msat_env, msat_term, bool)
FUN2(msat_term_is_or, msat_env, msat_term, bool)
FUN2(msat_term_is_not, msat_env, msat_term, bool)
FUN2(msat_term_is_iff, msat_env, msat_term, bool)
FUN2(msat_term_is_uf, msat_env, msat_term, bool)
FUN2(msat_term_is_constant, msat_env, msat_term, bool)
FUN2(msat_term_is_equal, msat_env, msat_term, bool)
FUN2(msat_term_is_leq, msat_env, msat_term, bool)
FUN2(msat_term_is_plus, msat_env, msat_term, bool)
FUN2(msat_term_is_times, msat_env, msat_term, bool)
FUN2(msat_term_is_term_ite, msat_env, msat_term, bool)
FUN2(msat_term_is_floor, msat_env, msat_term, bool)
FUN2(msat_find_decl, msat_env, string, msat_decl)
FUN1(msat_term_get_decl, msat_term, msat_decl)
FUN1(msat_decl_id, msat_decl, int)

/* Term parsing/printing */
FUN2(msat_from_string, msat_env, string, msat_term)
FUN2(msat_from_smtlib2, msat_env, string, msat_term)
FUN2(msat_to_smtlib2, msat_env, msat_term, string)

/* Problem solving */
FUN1(msat_push_backtrack_point, msat_env, int)
FUN1(msat_pop_backtrack_point, msat_env, int)
FUN1(msat_num_backtrack_points, msat_env, int)
FUN1(msat_reset_env, msat_env, int)
FUN1(msat_solve, msat_env, msat_result)
FUN2(msat_assert_formula, msat_env, msat_term, int)

/* Interpolation */
FUN1(msat_create_itp_group, msat_env, int)
FUN2(msat_set_itp_group, msat_env, int, int)
CAMLprim value wrapper_msat_get_interpolant(value env, value a_groups) {
    CAMLparam2(env, a_groups);
    size_t len = length(a_groups);
    int groups[len];
    int i = 0;
    while (a_groups != Val_emptylist) {
	groups[i++] = Int_val(Field(a_groups, 0));
	a_groups = Field(a_groups, 1);
    }
    FUN_RET_msat_term(msat_get_interpolant(ENV_val(env), groups, len));
}

/* Model computation */
FUN2(msat_get_model_value, msat_env, msat_term, msat_term)
FUN1(msat_get_model, msat_env, msat_model)
FUN1(msat_destroy_model, msat_model, unit)
FUN2(msat_model_eval, msat_model, msat_term, msat_term)
