import argparse
import logging
import os
import sys
import pickle

from src.dataDownload import data_downloader_multi
from src.featureExtraction import feature_extractor_multi
from src.label_generation import label_generator
from src.data_partition import data_partition
from src.data_process import normTrainTest
from src.subprocess_cmd import subprocess_cmd


logger = logging.getLogger(__name__)

gconfig = {
    "workdir" : ""
}

def configureCmdLineParser():
    cmdparser = argparse.ArgumentParser(description='Command line CyberDefense')

    cmdparser.add_argument('-d', '--workdir',
                       help='specify the working directory, cyberdefense_work is default',
                       default='cyberdefense_work')
    cmdparser.add_argument('--loglevel',
                           help='Log level - one of: DEBUG, INFO, WARNING, ERROR, CRITICAL',
                           default='WARNING')

    subparser = cmdparser.add_subparsers(dest='subcmd')

    download = subparser.add_parser('download')
    download.add_argument('-b', '--begindate',
                          help='A begin date for download of data YYYYMMDD')
    download.add_argument('-e','--enddate',
                          help='An end date for download of data YYYYMMDD')
    download.add_argument('-c', '--collector',
                        help='Collector, default rrc04',
                        default='rrc04')

    features = subparser.add_parser('extract')
    features.add_argument('-b', '--begindate',
                          help='A begin date for download of data YYYYMMDD')
    features.add_argument('-e','--enddate',
                          help='An end date for download of data YYYYMMDD')

    label = subparser.add_parser('labelgen')
    label.add_argument('--ab',
                       help='Anomaly begin date and time (YYYYMMDDTHH:MM, ex: 20230322T03:15')
    label.add_argument('--ae',
                       help='Anomaly end date and time (YYYYMMDDTHH:MM, ex 20230322T03:15')

    partition = subparser.add_parser('partition')
    partition.add_argument('-c',
                           help='Cut Percentage')
    partition.add_argument('--rrnseq',
                            help='RNN Sequence #')

    train = subparser.add_parser('train')
    train.add_argument('-c',
                        help='Cut Percentage')

    run = subparser.add_parser('run')
    run.add_argument('-c',
                     help='Cut Percentage')


    return cmdparser

loglevels = {
    'NOTSET' : logging.NOTSET,
    'DEBUG' : logging.DEBUG,
    'INFO' : logging.INFO,
    'WARNING' : logging.WARNING,
    'ERROR' : logging.ERROR,
    'CRITICAL' : logging.CRITICAL
}
def main():

    #
    site = 'RIPE'
    cmdparser = configureCmdLineParser()
    cmd = cmdparser.parse_args()

    # Get logging up and running
    gconfig['workdir'] = cmd.workdir
    workdir = gconfig['workdir']
    if os.path.exists(workdir):
        if not os.path.isdir(workdir):
            print('Error: working directory %s exists and is not a directory!' % workdir)
            sys.exit(-1)
    else:
        os.mkdir(workdir)

    loglevelstr = cmd.loglevel
    if loglevelstr not in loglevels:
        print("Error: invalid log level %s. Using default WARNING instead." % loglevelstr)
        loglevel = logging.WARNING
    else:
        loglevel = loglevels[loglevelstr]

    # Don't log before this configures logging.
    logging.basicConfig(filename = workdir + '/log',
                        level=loglevel,
                        format='%(asctime)s %(name)s %(levelname)s %(message)s')

    if cmd.subcmd == 'download':
        logger.info("Starting download (begindate = %s, enddate = %s, collector = %s)" % (
        cmd.begindate, cmd.enddate, cmd.collector))

        # PRECONDITIONS: none

        data_downloader_multi(cmd.begindate, cmd.enddate, site, cmd.collector)

    elif cmd.subcmd == 'extract':
        logger.info("Feature extraction (begindate = %s, enddate = %s)" % (cmd.begindate, cmd.enddate) )

        # PRECONDITIONS: zipped files exist for the interval specified

        output_file_list = feature_extractor_multi(cmd.begindate, cmd.enddate, site)
        print("Generated files:\n", ' '.join([str(i) for i  in output_file_list]))
        filelistfile = open(workdir + '/' + 'file_list', 'wb')
        pickle.dump(output_file_list, filelistfile)
        filelistfile.close()

    elif cmd.subcmd == 'labelgen':
        logger.info('Running labelgen')
        start = cmd.ab.split('T')
        start_date = start[0]
        start_time = start[1]
        end = cmd.ae.split('T')
        end_date = end[0]
        end_time = end[1]
        filelistfile = open(workdir + '/' + 'file_list', 'rb')
        output_file_list = pickle.load(filelistfile)
        labels = label_generator(start_date, end_date, start_time, end_time, site,
                                 output_file_list)
        labelsfile = open(workdir + '/' + 'labels', 'wb')
        pickle.dump(labels, labelsfile)
        labelsfile.close()
        print(' '.join(str(i) for i in labels))

    elif cmd.subcmd == 'partition':
        logger.info('Running partition')
        filelistfile = open(workdir + '/' + 'file_list','rb')
        output_file_list = pickle.load(filelistfile)
        filelistfile.close()
        labelsfile = open(workdir + '/' + 'labels', 'rb')
        labels = pickle.load(labelsfile)
        labelsfile.close()
        a = cmd.c
        b = cmd.rrnseq
        data_partition(cmd.c, site, output_file_list, labels, int(cmd.rrnseq))

    elif cmd.subcmd == 'train':
        logger.info('Training')
        normTrainTest(cmd.c, site)

    elif cmd.subcmd == 'run':
        cut_pct = cmd.c
        print("--------------------Experiment-Begin--------------------------")
        subprocess_cmd("cd src/; \
                        cp ./data_split/train_%s_%s_n.csv ./data_split/test_%s_%s_n.csv ./RNN_Running_Code/RNN_Run/dataset/ ; \
                        cd RNN_Running_Code/RNN_Run/dataset/; \
                        mv train_%s_%s_n.csv train.csv; mv test_%s_%s_n.csv test.csv; \
                        cd ..; cd ..; \
                        chmod +x integrate_run.sh; sh ./integrate_run.sh ; \
                        cd RNN_Run/; sh ./collect.sh; \
                        cp -r res_acc res_run ../data_representation/ ; \
                        cd .. ; cd data_representation/ ; \
                        python TableGenerator.py; " \
                       % (cut_pct, site, cut_pct, site, cut_pct, site, cut_pct, site))

        print("--------------------Experiment-end----------------------------")
        subprocess_cmd("cd src/; \
                        mv ./RNN_Running_Code/data_representation/data_representation_table.csv ./STAT/ ; \
        				mv ./STAT/data_representation_table.csv ./STAT/results_%s_%s.csv" \
                       % (cut_pct, site))

        # Remove generated folders
        subprocess_cmd("cd src/; \
                        cd RNN_Running_Code/RNN_Run/; \
                        #rm -rf ./experiment/ ./res_acc/ ./res_run/ ./tmp/")

    else:
        print("Unknown Command!")




if __name__ == '__main__':
    main()


#TODO : fix single site = 'RIPE'
#TODO : fix single collector = 'rrc04'