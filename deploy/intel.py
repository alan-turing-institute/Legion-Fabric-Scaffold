from fabric.api import *
from mako.template import Template
import mako
import time
import os

env.user='jamespjh'

env.run_at="/home/"+env.user+"/Scratch/Scaffold/output"
env.deploy_to="/home/"+env.user+"/devel/mpi-scaffold"
env.clone_url="https://github.com/alan-turing-institute/Turing-Fabric-Scaffold.git"
env.hosts=['cobra.hpclab.net:2005']

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
                run('cmake ..')
                run('make')
                run('test/catch')

@task
def warm(branch='master'):
  with cd(env.deploy_to+'/Turing-Fabric-Scaffold/build'):
      with path("/home/hut23/install/bin"):
        run('git checkout '+branch)
        run('git pull')
        run('cmake ..')
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
            run('cmake ..')
            run('make')
            run('test/catch')

@task
def sub(processes=4):
    env.processes=processes
    template_file_path=os.path.join(os.path.dirname(__file__),'intel.sh.mko')
    script_local_path=os.path.join(os.path.dirname(__file__),'intel.sh')
    with open(template_file_path) as template:
        script=Template(template.read()).render(**env)
        with open(script_local_path,'w',newline='\n') as script_file:
            script_file.write(script)
    with cd(env.deploy_to):
        put(script_local_path,'example.sh')
        run('sbatch example.sh')

@task
def stat():
    run('squeue')

@task
def wait():
  """Wait until all jobs currently qsubbed are complete, then return"""
  while "job-ID" in run('squeue'):
      time.sleep(10)

@task
def fetch():
    with lcd(os.path.join(os.path.dirname(os.path.dirname(__file__)),'results')):
      with cd(env.run_at):
        get('*')
