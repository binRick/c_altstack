#//***********************************************************/
project(
    'c_altstack',          ['c'],         version : '0.0.1',         
    default_options : ['warning_level=2','c_std=gnu99'],
)
#//***********************************************************/
host_system = host_machine.system()

cc = meson.get_compiler('c')
sh = find_program('sh', required : true)
passh = find_program('passh', required : true)

#//***********************************************************/
test_deps = []
#//***********************************************************/
incdir = include_directories(
  './include', 
  './deps', 
  '../',
  '../metalang99/include',
  '../ok',
)
#//***********************************************************/
test_altstack_module1 = executable('test_altstack_module1', [ 
    'tests/test_altstack_module1.c',  
  ], include_directories : [incdir], 
  dependencies : [],
)
#//***********************************************************/
stk_maxes = [ 
  '5',
  '50',
  '500',
  '1100',
  '1500',
]
vla_maxes = [ 
 '5',
 '10',
 '50',
 '150',
 '250',
]

tests = [
  ['m1.log', 'test altstack module 1', test_altstack_module1, 'wrap', stk_maxes, vla_maxes, ],
]
#//***********************************************************/

#//***********************************************************/
test_env = environment()
test_env.set('__SH', sh.full_path())
test_env.set('__BD', meson.current_build_dir())
test_env.set('__SD', meson.current_source_dir())
#//***********************************************************/
foreach i : tests
foreach stk : i[4]
foreach vla : i[5]
test(i[1], passh, args: [ '-L', i[0], '--', i[2], i[3], stk, vla, ], env: test_env)
endforeach
endforeach
endforeach
#//***********************************************************/
#//***********************************************************/


