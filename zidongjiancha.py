import pathlib
import subprocess

DATA = pathlib.Path('./test/')

def jsonchecker_output(json_file):
    out = subprocess.check_output(['./bin/splc', json_file])
    return out.decode().strip()

def check_jsonchecker_error():
    casefile_list = []
    for casefile in DATA.glob('*.spl'):
        casefile_list.append(casefile)
    
    casefile_list.sort()
    for casefile in casefile_list:
        out = jsonchecker_output(casefile)
        # answer = f'duplicate key: "{casefile.name[5:6]}"' 
        print(str(casefile) + " splc_result:")
        print(out)
        print("-" * 100)
        # 把相应的文件读进来并且打印出来
        # casefile_string = str(casefile)
        # answerfile = casefile_string[:len(casefile_string)-3]
        # answerfile += "out"
        # print(answerfile + ": ")
        # file = open(answerfile, "rw")
        # alllines = file.readlines()
        # for aline in alllines:
        #     print(aline, end = "")
        # file.close()
        # print("=" * 100)

check_jsonchecker_error()
