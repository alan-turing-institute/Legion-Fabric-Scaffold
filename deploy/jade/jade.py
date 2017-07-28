from fabric.api import *
import os

env.project='JAD005'
env.group='jph01'
env.user='jph74-jph01'

env.home="/jmain01/home/"+env.project+'/'+env.group+'/'+env.user
env.run_at=env.home+"/Scratch/Scaffold/output"
env.deploy_to=env.home+"/devel/mpi-scaffold"
env.clone_url="https://github.com/alan-turing-institute/Turing-Fabric-Scaffold.git"
env.hosts=['jade.hartree.stfc.ac.uk']

env.template_file_path=os.path.join(os.path.dirname(__file__),'jade.sh.mko')
env.script_local_path=os.path.join(os.path.dirname(__file__),'jade.sh')

from ..simple_mpi.simple_mpi import *
from ..slurm.slurm import *
