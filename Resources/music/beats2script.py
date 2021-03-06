import string
import json
import sys
import os

def gen_script(filename, name):
    former = 0
    delta_array = list()
    with open(filename) as f:
        while True:
            s = f.readline()
            if not s:
                break;
            time = string.atof(s)
            time = int(time * 1000)
            d = time - former
            delta_array.append(d)
            former = time
    
    script = dict()
    script["rhythmPoints"] = delta_array
    script["rhythmEvents"] = dict()
    script["rhythmEvents"]["attack"] = [i for i in range(0, len(delta_array)) if i % 6 == 2]
    script["rhythmEvents"]["nod"] = []
    script["rhythmEvents"]["addEnemy"] = [i for i in range(0, len(delta_array))]
    script["rhythmEvents"]["bomb"] = [i for i in range(0, len(delta_array)) if i % 6 == 0 or i % 6 == 1]
    script["rhythmEvents"]["spikeAttack"] = [i for i in range(0, len(delta_array)) if i % 6 == 4]
    
    script_str = json.dumps(script, indent=2)
    outfile = open(name + ".json", "w")
    outfile.write(script_str)
    outfile.close()
    

def main(argv):
    param = argv[1] if 1 < len(argv) else None 
    name = argv[2] if 2 < len(argv) else None
    usage = 'usage: python beats2script.py [beats file name] [prefix of two script]'
    if param == '-h':
        print usage
    else:
        beats_file = param
        if not beats_file:
            print 'require beats file'
            print usage
            return
        if not os.path.exists(beats_file):
            print 'file "%s" not exists' % beats_file
            return
        if not os.path.isfile(beats_file):
            print '"%s" is not a file' % beats_file
            return
        if not name:
            print 'prefix is empty'
            print usage
            return
        gen_script(beats_file, name)

if __name__ == '__main__':
    main(sys.argv)
