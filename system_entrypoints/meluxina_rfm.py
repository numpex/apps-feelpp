import socket

hostname = socket.gethostname()

site_configuration = {
    'systems': [
        {
            'name': 'meluxina',
            'descr': 'meluxina',
            'hostnames': [f'{hostname}'],
            'modules_system': 'lmod',
            'partitions': [
                {
                    'name': 'cpu',
                    'scheduler': 'slurm',
                    'launcher': 'srun',
                    'max_jobs': 8,
                    'access': ['--partition=cpu'],
                    'environs': ['default'],
                    'processor': {
                        'num_cpus': 128
                    },
                    'devices':[
                        {
                            'type': 'cpu',
                            'num_devices': 573
                        }
                    ],
                    'container_platforms':[
                        {
                            'type': 'Apptainer',
                        }
                    ],
                    'extras':{
                        'memory_per_node':512
                    }
                }
            ]
        }

    ],
    'environments': [
        {
            'name': 'default',
            'modules': ['OpenMPI/5.0.3-GCC-13.3.0'],
            'env_vars':[['PMIX_MCA_psec','^munge']],
            'prepare_cmds':[
                'module load env/release/2024.1',
                "module load Apptainer/1.3.6-GCCcore-13.3.0",
                'export INSNAME=feelpp_kub_cem_instance',
                'export APPTAINER_IMAGE=/mnt/tier1/project/p200506/kub/sifs/kub_feature-cem_partitioning-sif',
                'srun -N ${SLURM_NNODES} --ntasks-per-node 1 -c 1 apptainer instance start --bind /mnt/tier1/project/p200506/kub/apptainer_home --bind /mnt/tier1/project/p200506/kub/inputdataset/:/input_data ${APPTAINER_IMAGE} ${INSNAME} &',
                'sleep 5'
            ],
            'target_systems': ['meluxina:cpu']
        }
    ]
}