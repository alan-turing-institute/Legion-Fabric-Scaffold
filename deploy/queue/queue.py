from fabric.api import *
from mako.template import Template
import mako
import time
import os

@task
def sub(processes=1, to_exec='simple_mpi', sudo=False):
    env.processes=processes
    env.to_exec=to_exec
    with open(env.template_file_path) as template:
        script=Template(template.read()).render(**env)
        with open(env.script_local_path,'w',newline='\n') as script_file:
            script_file.write(script)
    with cd(env.deploy_to):
        put(env.script_local_path,'example.sh')
        if sudo:
            sudo(env.sub+'example.sh') # Workaround for crazy setup
        else:
            run(env.sub+'example.sh')

@task
def stat():
    run(env.stat)

@task
def wait():
  """Wait until all jobs currently qsubbed are complete, then return"""
  while "job-ID" in run(env.stat):
      time.sleep(10)

@task
def fetch():
    with lcd(os.path.join(os.path.dirname(os.path.dirname(__file__)),'results')):
      with cd(env.run_at):
        get('*')
