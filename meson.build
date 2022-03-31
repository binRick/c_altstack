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
MEMORY_LIMITS_MB = [ 
  '32',
]
MEMORY_REQUESTS_MB = [ 
 '24',
 '48',
 '128',
 '1024',
]

tests = [
  ['m1.log', 'test altstack module ', test_altstack_module1, 'wrap', MEMORY_LIMITS_MB, MEMORY_REQUESTS_MB, ],
]
#//***********************************************************/

#//***********************************************************/
test_env = environment()
test_env.set('__SH', sh.full_path())
test_env.set('__BD', meson.current_build_dir())
test_env.set('__SD', meson.current_source_dir())
#//***********************************************************/
foreach i : tests
foreach MEMORY_LIMIT_MB : i[4]
foreach MEMORY_REQ_MB : i[5]
  test(i[1] + MEMORY_REQ_MB + 'MB' + '/' + MEMORY_LIMIT_MB + 'MB', passh, args: [ '-L', i[0], '--', i[2], i[3], MEMORY_LIMIT_MB, MEMORY_REQ_MB, ], env: test_env)
endforeach
endforeach
endforeach
#//***********************************************************/
#//***********************************************************/


