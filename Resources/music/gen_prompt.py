import json
import os

rhythm_script_file = 'alice.json'
move_to_time = 2000

text = ''
with open(rhythm_script_file) as json_file:
    text = json_file.read()
script = json.loads(text)

src = script["rhythmPoints"]
select_point = script["rhythmEvents"]["attack"]

out = list()

accumulate = 0
for i in range(0, len(src)):
    accumulate = accumulate + src[i]
    if i in select_point:
        out.append(accumulate)
        accumulate = 0

out[0] = out[0] - move_to_time

prompt_script = dict()
prompt_script["rhythmPoints"] = out
prompt_script["rhythmEvents"] = dict()
prompt_script["rhythmEvents"]["addPrompt"] = [i for i in range(0, len(out))]

script_str = json.dumps(prompt_script)

filename = os.path.basename(rhythm_script_file)
script_name, ext = os.path.splitext(filename)
outfile = open(script_name + "_prompt.json", "w")
outfile.write(script_str)
outfile.close()
