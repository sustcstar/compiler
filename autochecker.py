import pathlib
import subprocess


DATA = pathlib.Path('./')


def jsonchecker_output(json_file):
    out = subprocess.check_output(['./bin/splc', json_file])
    return out.decode().strip()

def check_jsonchecker_error():
    for casefile in DATA.glob('declaration_basic_test.spl'):
        out = jsonchecker_output(casefile)
        # answer = f'duplicate key: "{casefile.name[5:6]}"' 
        # print(f'For {casefile.name}:', end=' ')
        # assert out == answer
        # print('CORRECT')
        # print('-'*80)
        print(out)

check_jsonchecker_error()
