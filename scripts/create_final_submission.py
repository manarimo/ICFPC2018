import db
import os

conn = db.get_connection()
cursor = conn.cursor(dictionary=True)
cursor.execute("SELECT t.problem_id, p.name AS problem_name, tm.trace_id, tm.energy_autoscorer, tm.s3url "
               "FROM tbltrace_metadata tm "
               "JOIN tbltrace t ON tm.trace_id = t.id "
               "JOIN tblproblem p ON t.problem_id = p.id "
               "WHERE tm.energy_autoscorer IS NOT NULL AND tm.energy_autoscorer > 0")
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
        print('%s: %s' % (name, trace['s3url']))
        os.system('curl -o tmp/%s.nbt %s' % (trace['problem_name'], trace['s3url']))
    else:
        print('%s: blob(trace_id=%d)' % (name, trace['trace_id']))
        cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace['trace_id'],))
        blob = cursor.fetchone()['body']
        with open('tmp/' + trace['problem_name'] + '.nbt', 'bw') as f:
            f.write(blob)
