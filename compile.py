#!/usr/bin/env python3

import sys
import os
import json
import subprocess

# -r: set 'run_at_start' to a different type than the default mentioned in this file
# -d [DEMO_NAME]: selects a demo, by default it is demos[0]
# -n: rebuild the entire project and ignore the cache file

Settings = {
    # set to true if you want the program to run after compilation (can be activated with a flag)
    'run_at_start' : True,
    'ignore_cache' : False,
    'cache_filename' : 'compile_cache.json',
    'cache_file_exists' : False,
    'working_dir' : '.',

    # the current selected demo
    'selected_demo' : 13,

    # folders that we are looking at for files to compile
    'folders' : [
        'wonderer/src',
        'vendor/imgui',
        'vendor/stb',
        'physc/src'
    ],

    # demos can be selected with a flag 
    'demos' : [
        (None, None),
        ('platformer', 'demos/platformer'),
        ('basic', 'demos/basic'),
        ('text', 'demos/text_rendering'),
        ('btext', 'demos/builtin_text/'),
        ('orbiter', 'demos/orbiter/'),
        ('world', 'demos/world'),
        ('tetris', 'demos/tetris'),
        ('obj_loader', 'demos/obj_loading'),
        ('framebuffers', 'demos/framebuffers'),
        ('compute_shaders', 'demos/compute_shaders'),    
        ('SSBOs', 'demos/SSBOs'),
        ('perlin_noise', 'demos/perlin_noise'),
        ('atomas', 'demos/atomas')
    ],

    # filetypes to look for
    'filetypes' : ['c', 'cpp'],

    # compilers to use for each filetype
    'compilers' : ['gcc', 'g++'],

    # linker that will be used to link the program
    'linker': 'g++',

    # cflags required for compilation
    'cflags' : ['-g'],#, '-fno-stack-protector'],

    # libs that needed to be linked
    'libs' : [],

    # dependencies of the program (runs pkg-config on each of them
    # adds the output to the 'cflags' and 'libs')
    'dependencies' : ['glfw3', 'glew', 'cglm'],

    # Object files relevant for linking 
    'objs': [],

    # An array of tupels 
    'cached_files': [],

    'compile_files' : []
    
}

curr_cache = {
    'files' : [], # ('filepath', 'last modified date')
    'demo' : None,
}

prev_cache = {}

def interprate_args(args):

    flag_argument = False

    for i in range(len(args)):
        arg = args[i]

        if flag_argument:
            flag_argument = False
            continue

        # toggle 'run_at_start'
        elif arg == '-r': Settings['run_at_start'] = not Settings['run_at_start']
        # toggle the 'ignore_cache'
        elif arg == '-n': Settings['ignore_cache'] = not Settings['ignore_cache']

        # selects a demo
        elif arg == '-d': 
            if i == len(args) - 1 or args[i + 1][0] == '-':
                print("please specify a demo right after the '-d' flag")
                return False

            found = False
            for j in range(len(Settings['demos'])): 

                if Settings['demos'][j][0] == args[i + 1]: 
                    found = True
                    Settings['selected_demo'] = j
                    break 

            if not found: 
                print("cant find specified demo")
                return False
            else: flag_argument = True

        elif Settings['working_dir'] == '.' and os.path.isdir(arg):
            Settings['working_dir'] = arg

        else: 
            print(f'Cant understand "{arg}"')
            return False

    return True

def read_cache():
    config_path = os.path.join(Settings['working_dir'], Settings['cache_filename'])
    if os.path.exists(config_path):

        Settings['cache_file_exists'] = True
        
        f = open(config_path)
        data = json.load(f)
        f.close()

        current_demo = Settings['demos'][Settings['selected_demo']][0] == data['demo']
        
        for file in data['files']:
            divided_path = file[0].split('/')
            is_demo = False 
            for p in divided_path:
                    is_demo = True
                    break
            if not is_demo or current_demo: 
                Settings['cached_files'].append(tuple(file))

def look_at_directories():

    files = []

    Settings['folders'].append(Settings['demos'][Settings['selected_demo']][1])

    for folder in Settings['folders']:
        relative_folder_path = os.path.join(Settings['working_dir'], folder)
        for file in os.listdir(relative_folder_path):
            relative_file_path = os.path.join(relative_folder_path, file)
            files.append((relative_file_path, os.stat(relative_file_path).st_mtime))

    for file in files:
        add_to_list = True
        for cached_file in Settings['cached_files']:
            if cached_file[0] == file[0]:
                add_to_list = file[1] != cached_file[1]

        if add_to_list:
            for i in range(len(Settings['filetypes'])):
                if file[0].endswith(f'.{Settings["filetypes"][i]}'):
                    Settings['compile_files'].append((file[0], file[1], Settings['compilers'][i]))
                    break 

    # Updating the cached files (getting it ready for rewrite)
    if not Settings['ignore_cache']:

        for file in Settings['compile_files']:
            for cached_file in Settings['cached_files']:
                if file[0] == cached_file[0]:
                    Settings['cached_files'].remove(cached_file)
                    break

            Settings['cached_files'].append((file[0], file[1]))

def generate_flags_and_libs():

    for dep in Settings['dependencies']:

        # cflags
        process = subprocess.Popen(['pkg-config', '--cflags', dep], stdout=subprocess.PIPE)
        output, error = process.communicate()
        
        curr_cflags = output.decode('utf-8').split('\n')[0].split(' ')[:-1]
        for cf in curr_cflags:
            found = False 
            for f in Settings['cflags']:
                if cf == f:
                    found = True
                    break
            if not found:
                Settings['cflags'].append(cf)

        # libs 
        process = subprocess.Popen(['pkg-config', '--libs', dep], stdout=subprocess.PIPE)
        output, error = process.communicate()

        curr_libs = output.decode('utf-8').split('\n')[0].split(' ')[:-1]
        for cl in curr_libs:
            found = False 
            for l in Settings['libs']:
                if cl == l:
                    found = True 
                    break 
            if not found:
                Settings['libs'].append(cl)
    
def generate_objs():
    build_path = os.path.join(Settings['working_dir'], 'build')

    if not os.path.exists(build_path):
       os.mkdir(build_path) 
    
    objs_path = os.path.join(build_path, 'objs')

    if not os.path.exists(objs_path):
        os.mkdir(objs_path)

    for f in Settings['cached_files']:
        obj_path = os.path.join(objs_path, '_'.join(f[0].split('/')[1:]) + '.o')
        Settings['objs'].append((f[0], obj_path))

def compile_files():
    for f in Settings["compile_files"]:

        obj = ''
        for o in Settings['objs']:
            if f[0] == o[0]:
                obj = o[1]

        compile_command = [f[2]]
        compile_command.extend(Settings['cflags'])
        compile_command.append('-c')
        compile_command.append(f[0])
        compile_command.append('-o')
        compile_command.append(obj)
        print(f'executes: {" ".join(compile_command)}')

        process = subprocess.Popen(compile_command, stdout=subprocess.PIPE)
        output, error = process.communicate()

        if error != None:
            print(f"ERROR: {error}")

def link_program():

    demo_name = Settings['demos'][Settings['selected_demo']][0]
    if demo_name == None: return 

    bin_path = os.path.join(Settings['working_dir'], 'build/bin')
    if not os.path.exists(bin_path):
        os.mkdir(bin_path)

    objs = []
    for o in Settings['objs']:
        objs.append(o[1])

    link_command = [Settings['linker']]
    link_command.extend(Settings['cflags'])
    link_command.append('-o')
    link_command.append(os.path.join(bin_path, demo_name))
    link_command.extend(objs)
    link_command.extend(Settings['libs'])

    print(f'\nexecutes: {" ".join(link_command)}')
    process = subprocess.Popen(link_command, stdout=subprocess.PIPE)
    output, error = process.communicate()

    if error != None:
        print(f"ERROR\n{error}")

def update_cache():
    cache_path = os.path.join(Settings['working_dir'], Settings['cache_filename'])

    output_dict = {
        "demo" : Settings['demos'][Settings['selected_demo']][0],
        "files" : Settings['cached_files']
    }

    f = open(cache_path, 'w+')
    f.write(json.dumps(output_dict, indent=2))
    f.truncate()
    f.close()

def main(args):
    if not interprate_args(args): return
    if not Settings['ignore_cache']: read_cache()
    
    look_at_directories()
    generate_flags_and_libs()
    generate_objs()
    compile_files()
    link_program()

    if not Settings['ignore_cache']: update_cache()

    if Settings['run_at_start']: 
        os.system(os.path.join(Settings['working_dir'], 'build/bin', Settings['demos'][Settings['selected_demo']][0]))

if __name__ == "__main__":
    main(sys.argv[1:])
