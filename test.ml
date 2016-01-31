open Mathsat
open OUnit

let assert_equal_result = assert_equal ~printer:msat_result_to_string

let assert_sat env phi =
  msat_push_backtrack_point env;
  msat_assert_formula env phi;
  assert_equal_result (msat_solve env) Sat;  
  msat_pop_backtrack_point env

let assert_unsat env phi =
  msat_push_backtrack_point env;
  msat_assert_formula env phi;
  assert_equal_result (msat_solve env) Unsat;  
  msat_pop_backtrack_point env

let empty_cfg = msat_create_config ()

let types_test () =
  let env = msat_create_env empty_cfg in
  let bool_typ = msat_get_bool_type env in
  let rat_typ = msat_get_rational_type env in
  let int_typ = msat_get_integer_type env in
  assert_equal (msat_is_bool_type env bool_typ) true;
  assert_equal (msat_is_rational_type env rat_typ) true;
  assert_equal (msat_is_integer_type env int_typ) true

let incremental_test () =
  let env = msat_create_env empty_cfg in
  let ktrue = msat_make_true env in
  let kfalse = msat_make_false env in

  msat_assert_formula env ktrue;
  assert_equal (msat_num_backtrack_points env) 0;
  assert_equal_result (msat_solve env) Sat;

  msat_push_backtrack_point env;
  msat_assert_formula env kfalse;
  assert_equal_result (msat_solve env) Unsat;
  assert_equal (msat_num_backtrack_points env) 1;

  msat_pop_backtrack_point env;
  assert_equal_result (msat_solve env) Sat;
  assert_equal (msat_num_backtrack_points env) 0

let arith_test () =
  let env = msat_create_env empty_cfg in
  let one = msat_make_number env "1" in
  let two = msat_make_number env "2" in
  let zero = msat_make_number env "0" in
  assert_sat env (msat_make_leq env zero one);
  assert_unsat env (msat_make_leq env one zero);
  assert_sat env (msat_make_equal env two (msat_make_plus env one one));
  assert_sat env (msat_make_equal env two (msat_make_times env two one))

let constant_test () =
  let env = msat_create_env empty_cfg in
  let one = msat_make_number env "1" in
  let two = msat_make_number env "2" in
  let int_typ = msat_get_integer_type env in
  let x_decl = msat_declare_function env "x" int_typ in
  let x = msat_make_constant env x_decl in
  assert_sat env
    (msat_make_and env (msat_make_leq env one x) (msat_make_leq env x two));
  assert_unsat env
    (msat_make_and env (msat_make_leq env two x) (msat_make_leq env x one))

let interpolation_test () =
  let cfg = msat_create_config () in
  msat_set_option cfg "interpolation" "true";
  let env = msat_create_env cfg in
  let int_typ = msat_get_integer_type env in
  let zero = msat_make_number env "0" in
  let x = msat_make_constant env (msat_declare_function env "x" int_typ) in
  let y = msat_make_constant env (msat_declare_function env "y" int_typ) in
  let z = msat_make_constant env (msat_declare_function env "z" int_typ) in
  let a_group = msat_create_itp_group env in
  let b_group = msat_create_itp_group env in

  msat_set_itp_group env a_group;
  msat_assert_formula env (msat_make_equal env x y);
  msat_assert_formula env (msat_make_leq env zero x);

  msat_set_itp_group env b_group;
  msat_assert_formula env (msat_make_leq env y z);
  msat_assert_formula env (msat_make_leq env z zero);
  msat_assert_formula env (msat_make_not env (msat_make_equal env z zero));

  assert_equal_result (msat_solve env) Unsat;
  assert_equal
    ~printer:(msat_to_smtlib2 env)
    ~cmp:msat_term_equal
    (msat_get_interpolant env [a_group])
    (msat_make_leq env zero y)


let number_test () =
  let env = msat_create_env empty_cfg in
  let one = msat_make_number env "1" in
  let frac = msat_make_number env "123/456" in
  assert_equal (msat_term_is_number env one) true;
  assert_equal (msat_term_is_number env frac) true;
  assert_equal (msat_term_to_number env one) (Mpq.of_int 1);
  assert_equal (msat_term_to_number env frac) (Mpq.of_frac 123 456)

let model_test () =
  let cfg = msat_create_config () in
  msat_set_option cfg "model_generation" "true";
  let env = msat_create_env cfg in
  let int_typ = msat_get_integer_type env in
  let x_decl = msat_declare_function env "x" int_typ in
  let y_decl = msat_declare_function env "y" int_typ in
  let x = msat_make_constant env x_decl in
  let y = msat_make_constant env y_decl in
  let one = msat_make_number env "1" in
  let two = msat_make_number env "2" in
  msat_assert_formula env (msat_make_equal env x one);
  msat_assert_formula env (msat_make_equal env y two);
  assert_equal (msat_solve env) Sat;
  let model = msat_get_model env in
  let x_val = msat_model_eval model x in
  assert_equal (msat_term_is_number env x_val) true;
  assert_equal (msat_term_to_number env x_val) (Mpq.of_int 1);
  let y_val = msat_model_eval model y in
  assert_equal (msat_term_is_number env y_val) true;
  assert_equal (msat_term_to_number env y_val) (Mpq.of_int 2)

let all =
  "MathSAT" >::: [
    "types" >:: types_test;
    "incremental" >:: incremental_test;
    "arith" >:: arith_test;
    "constant" >:: constant_test;
    "interpolation" >:: interpolation_test;
    "number" >:: number_test;
    "model" >:: model_test
  ]

let _ =
  OUnit.run_test_tt_main all
