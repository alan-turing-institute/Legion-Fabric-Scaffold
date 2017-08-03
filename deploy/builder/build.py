from fabric.api import *

@task
def cold(branch='master'):
    run('rm -rf '+env.deploy_to)
    run('mkdir -p '+env.deploy_to)
    run('mkdir -p '+env.run_at)
    with cd(env.deploy_to):
        with path(env.deploy_to):
            run('git clone '+env.clone_url)
            run('mkdir Turing-Fabric-Scaffold/build')
            with cd('Turing-Fabric-Scaffold/build'):
                run('git checkout '+branch)
                with prefix(env.modules):
                    run('cmake ..')
                    run('make')
                    run('simple_mpi/test/catch_simple_mpi') # just smoke

@task
def warm(branch='master'):
  with cd(env.deploy_to+'/Turing-Fabric-Scaffold/build'):
      with path(env.deploy_to):
        run('git checkout '+branch)
        run('git pull')
        with prefix(env.modules):
            run('cmake ..')
            run('make')
            run('simple_mpi/test/catch_simple_mpi')

@task
def patch():
  with cd(env.deploy_to+'/Turing-Fabric-Scaffold'):
    local('git diff > patch.diff')
    put('patch.diff','patch.diff')
    with path(env.deploy_to):
        run('git checkout .')
        run('git apply patch.diff')
        with cd('build'):
            with prefix(env.modules):
                run('cmake ..')
                run('make')
                run('simple_mpi/test/catch_simple_mpi')
