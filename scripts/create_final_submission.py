import db
import os
import subprocess
import sys

conn = db.get_connection()
cursor = conn.cursor(dictionary=True)
cursor.execute("SELECT t.problem_id, p.name AS problem_name, tm.trace_id, tm.energy_autoscorer, tm.s3url "
               "FROM tbltrace_metadata tm "
               "JOIN tbltrace t ON tm.trace_id = t.id "
               "JOIN tblproblem p ON t.problem_id = p.id "
               "WHERE tm.energy_autoscorer IS NOT NULL AND tm.energy_autoscorer > 0 AND p.is_lightning = FALSE")
best_traces = dict()
for trace in cursor:
    prob_name = trace['problem_name']
    if prob_name in best_traces:
        if best_traces[prob_name]['energy_autoscorer'] > trace['energy_autoscorer']:
            best_traces[prob_name] = trace
    else:
        best_traces[prob_name] = trace

os.mkdir('tmp')
names = sorted(best_traces.keys())
for name in names:
    trace = best_traces[name]
    if trace['s3url']:
        print('%s: %s (energy=%s)' % (name, trace['s3url'].strip(), trace['energy_autoscorer']))
        os.system('curl -o tmp/%s.nbt %s' % (trace['problem_name'], trace['s3url'].strip()))
    else:
        print('%s: blob(trace_id=%d, energy=%s)' % (name, trace['trace_id'], trace['energy_autoscorer']))
        cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace['trace_id'],))
        blob = cursor.fetchone()['body']
        with open('tmp/' + trace['problem_name'] + '.nbt', 'bw') as f:
            f.write(blob)

print("Validate traces")
valid = True
for key in sorted(best_traces.keys()):
    source_file = 'assets/problemsF/%s_src.mdl' % (key,)
    target_file = 'assets/problemsF/%s_tgt.mdl' % (key,)
    trace_file = 'tmp/%s.nbt' % (key,)
    if key[0:2] == 'FA':
        command = ['./autoscorer/autoscorer', target_file, trace_file]
    elif key[0:2] == 'FD':
        command = ['./autoscorer/autoscorer', '--disassembly', source_file, trace_file]
    else:
        command = ['./autoscorer/autoscorer', '--reassembly', source_file, target_file, trace_file]

    print("Validating %s" % (key,))
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    energy = int(result.stdout)
    if energy == best_traces[key]['energy_autoscorer']:
        print("Success")
    else:
        print("Failed. Expected score=%d but got %d" % (best_traces[key]['energy_autoscorer'], energy))
        print(result.stderr)
        valid = False

if not valid:
    sys.exit(1)
