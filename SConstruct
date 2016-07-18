AddOption('--dbg', action='append_const', dest='cflags', const='-g')
AddOption('--opt', action='append_const', dest='cflags', const='-O0')

common_env = Environment()

common_env.MergeFlags(GetOption('cflags'))

common_env.Append(CPPDEFINES={'VERSION': 1})

# Our release build is derived from the common build environment...
release_env = common_env.Clone()
# ... and adds a RELEASE preprocessor symbol ...
release_env.Append(CPPDEFINES=['RELEASE'])
# ... and release builds end up in the "build/release" dir
release_env.VariantDir('build/release', 'src')

# We define our debug build environment in a similar fashion...
debug_env = common_env.Clone()
debug_env.Append(CPPDEFINES=['DEBUG'])
debug_env.VariantDir('build/debug', 'src')

# Now that all build environment have been defined, let's iterate over
# them and invoke the lower level SConscript files.
for mode, env in dict(release=release_env, 
    	       	      debug=debug_env).iteritems():
    env.SConscript('build/%s/SConscript' % mode, {'env': env})

#This takes care of the java component
Java('src/java/Trainer/', 'src/java/Trainer/Trainer.java')
