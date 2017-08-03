from fabric.api import *
import os

env.project='d420'
env.user='jamespjh'

env.home="/lustre/home/"+env.project+'/'+env.user
env.run_at=env.home+"/Scratch/Scaffold/output"
env.deploy_to=env.home+"/devel/mpi-scaffold"
env.clone_url="https://github.com/alan-turing-institute/Turing-Fabric-Scaffold.git"
env.hosts=['login.cirrus.ac.uk']
env.modules="module load intel-mpi-17 cmake-3.7.1-intel-17.0.2-vhhhji6"

env.template_file_path=os.path.join(os.path.dirname(__file__),'intel.sh.mko')
env.script_local_path=os.path.join(os.path.dirname(__file__),'intel.sh')

from ..builder.build import *
from ..pbs.pbs import *
