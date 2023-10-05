# Script to call Cyberdefense offline workflow passing a json file

import json
import argparse
from urllib.parse import unquote
import os
import sys

print(f"CWD CLI: {os.getcwd()}")

from app_offline import app_offline_classification

# Set up CLI Arguments
parser = argparse.ArgumentParser()

# Required Arguments
parser.add_argument("-p", "--parameters", required=True,
                    help="JSON file path with the parameters for this operation")

# Grab the Arguments
args = parser.parse_args()

# try:
#     p = json.loads(pJSON)
# except Exception as err:
#     sys.exit("ABORTING: a valid JSON string file must to be provided.")

filepath = unquote(args.parameters)

if not os.path.exists(filepath):
    sys.exit("ABORTING: a valid path for JSON task file must to be provided.")

p = json.load(open(filepath, 'r'))

expected_params = { 'site_choice',
                    'start_date_key',
                    'end_date_key',
                    'start_date_anomaly_key',
                    'end_date_anomaly_key',
                    'start_time_anomaly_key', 
                    'end_time_anomaly_key',
                    'cut_pct_key',
                    'rnn_seq_key'
                    }

if not expected_params == set(p):
    missing =  str(expected_params.difference(p))
    sys.exit(f"ABORTING: is missing the following parameters: {missing}")

# Converting params to CyberDefense standard
site = p.get('site_choice')  # or use 'request.form['']' due to dict format
site = 'RIPE' if site == 'ripe' else 'RouteViews'
start_date = p.get('start_date_key')
end_date = p.get('end_date_key')
start_date_anomaly = p.get('start_date_anomaly_key')
end_date_anomaly = p.get('end_date_anomaly_key')
start_time_anomaly = p.get('start_time_anomaly_key')
end_time_anomaly = p.get('end_time_anomaly_key')
cut_pct = p.get('cut_pct_key')
rnn_seq = p.get('rnn_seq_key')

header_offLine = "Off-Line BGP Anomaly Classification"

input_exp_key = [site, start_date, end_date, start_date_anomaly, end_date_anomaly, start_time_anomaly,
                    end_time_anomaly, cut_pct, rnn_seq]
# Load the dict data for the front-end (off-line)
context_offLine = app_offline_classification(header_offLine, input_exp_key)

print(context_offLine)




# Received parameters for the off-Line experiment (route)
# @app.route('/bgp_ad_offline', methods=['POST'])
# def analyze_offline():
#     print('Dict. params. received from the front-end: \n', request.form)  # check if receive keys (name) from front-end
#     model_params = {'site_choice',
#                     'start_date_key', 'end_date_key',
#                     'start_date_anomaly_key', 'end_date_anomaly_key',
#                     'start_time_anomaly_key', 'end_time_anomaly_key',
#                     'cut_pct_key',
#                     'rnn_seq_key'}

#     if model_params == set(request.form.keys()):
#         print("\n Server checked: Parameter received from the client :-) \n")

#         site = p.get('site_choice')  # or use 'request.form['']' due to dict format
#         site = 'RIPE' if site == 'ripe' else 'RouteViews'
#         start_date = p.get('start_date_key')
#         end_date = p.get('end_date_key')
#         start_date_anomaly = p.get('start_date_anomaly_key')
#         end_date_anomaly = p.get('end_date_anomaly_key')
#         start_time_anomaly = p.get('start_time_anomaly_key')
#         end_time_anomaly = p.get('end_time_anomaly_key')
#         cut_pct = p.get('cut_pct_key')
#         rnn_seq = p.get('rnn_seq_key')

#         input_exp_key = [site, start_date, end_date, start_date_anomaly, end_date_anomaly, start_time_anomaly,
#                          end_time_anomaly, cut_pct, rnn_seq]
#         # Load the dict data for the front-end (off-line)
#         context_offLine = app_offline_classification(header_offLine, input_exp_key)

#         time.sleep(5)  # to be changed
#         print("--------------------Saving Results-begin--------------------------")
#         # if not os.path.exists('./src/STAT/results.zip'):
#         # zip results
#         zipObj = zipfile.ZipFile('./src/STAT/results.zip', 'w')
#         zipObj.write('./src/STAT/train_test_stat.txt')
#         # Use glob find the csv file with prefix "labels_"
#         for file in glob.glob('./src/STAT/labels_*.csv'):
#             zipObj.write(file)  # RIPE or Route Views
#         for file in glob.glob('./src/STAT/results_*.csv'):
#             zipObj.write(file)  # (* = cut_pct, site)
#         # zipObj.write('./src/STAT/labels_*.csv')
#         # zipObj.write('./src/STAT/results_*.csv')

#         # zipObj = zipfile.ZipFile('./src/STAT/sample.zip', 'w')  # for test Download function
#         # zipObj.write('./src/STAT/train_test_stat.txt')  # for test Download function
#         # zipObj.write('./src/STAT/labels_RIPE.csv')  # for test Download function
#         # zipObj.write('./src/STAT/results_64_RIPE.csv')  # for test Download function
#         zipObj.close()
#         print("--------------------Saving Results-end--------------------------")

#         return render_template('bgp_ad_offline.html', **context_offLine)
#         # return render_template('bgp_ad_offline.html', result_prediction=result_prediction)
#     else:
#         return render_template('bgp_ad_offline.html', header2=header_offLine)