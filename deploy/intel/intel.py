from fabric.api import *
import os

env.user='jamespjh'

env.run_at="/home/"+env.user+"/Scratch/Scaffold/output"
env.deploy_to="/home/"+env.user+"/devel/mpi-scaffold"
env.clone_url="https://github.com/alan-turing-institute/Turing-Fabric-Scaffold.git"
env.hosts=['cobra.hpclab.net:2005']

env.template_file_path=os.path.join(os.path.dirname(__file__),'intel.sh.mko')
env.script_local_path=os.path.join(os.path.dirname(__file__),'intel.sh')

from ..simple_mpi.simple_mpi import *
from ..slurm.slurm import *
