from fabric.api import *

@task
def cold(branch='master'):
    run('rm -rf '+env.deploy_to)
    run('mkdir -p '+env.deploy_to)
    run('mkdir -p '+env.run_at)
    with cd(env.deploy_to):
        with path("/home/hut23/install/bin"):
            run('git clone '+env.clone_url)
            run('mkdir Turing-Fabric-Scaffold/build')
            with cd('Turing-Fabric-Scaffold/build'):
                run('git checkout '+branch)
                run('cmake ../simple_mpi')
                run('make')
                run('test/catch')

@task
def warm(branch='master'):
  with cd(env.deploy_to+'/Turing-Fabric-Scaffold/build'):
      with path("/home/hut23/install/bin"):
        run('git checkout '+branch)
        run('git pull')
        run('cmake ../simple_mpi')
        run('make')
        run('test/catch')

@task
def patch():
  with cd(env.deploy_to+'/Turing-Fabric-Scaffold'):
    local('git diff > patch.diff')
    put('patch.diff','patch.diff')
    with path("/home/hut23/install/bin"):
        run('git checkout .')
        run('git apply patch.diff')
        with cd('build'):
            run('cmake ../simple_mpi')
            run('make')
            run('test/catch')
