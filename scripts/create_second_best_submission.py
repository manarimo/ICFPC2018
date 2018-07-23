import db
import os

conn = db.get_connection()
cursor = conn.cursor(dictionary=True)
cursor.execute("SELECT t.problem_id, p.name AS problem_name, tm.trace_id, tm.energy_autoscorer, tm.s3url "
               "FROM tbltrace_metadata tm "
               "JOIN tbltrace t ON tm.trace_id = t.id "
               "JOIN tblproblem p ON t.problem_id = p.id "
               "WHERE tm.energy_autoscorer IS NOT NULL AND tm.energy_autoscorer > 0 AND p.is_lightning = FALSE AND p.dummy_problem = FALSE")
best_traces = dict()
second_best_traces = dict()
for trace in cursor:
    prob_name = trace['problem_name']
    if prob_name in best_traces:
        if best_traces[prob_name]['energy_autoscorer'] > trace['energy_autoscorer']:
            second_best_traces[prob_name] = best_traces[prob_name]
            best_traces[prob_name] = trace
    else:
        best_traces[prob_name] = trace

for key in best_traces.keys():
    if key not in second_best_traces:
        second_best_traces[key] = best_traces[key]

best_energy = 0
for trace in best_traces.values():
    best_energy += trace['energy_autoscorer']
best_traces.clear()

second_best_energy = 0
for trace in second_best_traces.values():
    second_best_energy += trace['energy_autoscorer']
print("Best: ", best_energy)
print("Scnd: ", second_best_energy)
best_traces = second_best_traces


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
