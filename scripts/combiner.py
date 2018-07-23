import db
import os
import submit
from pathlib import Path

conn = db.get_connection()
cursor = conn.cursor(dictionary=True)

def find_best_trace(problem_ids):
    cursor = conn.cursor(dictionary=True)
    placeholder = '(' + ','.join(['%s'] * len(problem_ids)) + ')'
    cursor.execute("SELECT t.id AS trace_id, energy_autoscorer FROM tbltrace_metadata tm JOIN tbltrace t ON t.id = tm.trace_id WHERE t.problem_id IN " + placeholder, problem_ids)
    best_disasm_trace_id = None
    best_disasm_energy = None
    for row in cursor:
        if best_disasm_energy is None:
            best_disasm_trace_id = row['trace_id']
            best_disasm_energy = row['energy_autoscorer']
        else:
            if best_disasm_energy > row['energy_autoscorer']:
                best_disasm_trace_id = row['trace_id']
                best_disasm_energy = row['energy_autoscorer']
    cursor.close()
    return best_disasm_trace_id

def trace_url(trace_id):
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT s3url FROM tbltrace_metadata WHERE trace_id=%s", (trace_id,))
    s3url = cursor.fetchone()['s3url']
    cursor.close()
    if s3url:
        return s3url.strip()
    return 'http://nanachi.kadingel.osak.jp/traces/' + str(trace_id) + '/blob'


cursor.execute("SELECT * FROM tblproblem WHERE name LIKE '%FR%'")
for problem in cursor.fetchall():
    print(problem['name'])
    cursor2 = conn.cursor(dictionary=True)

    cursor2.execute("SELECT id FROM tblproblem WHERE src_model_id = %s AND name LIKE '%FD%'", (problem['src_model_id'],))
    ids = [row['id'] for row in cursor2]
    if len(ids) == 0:
        print("No corresponding FD exists. Skipping")
        continue
    best_disasm_trace = find_best_trace(ids)
    disasm_url = trace_url(best_disasm_trace)
    print(best_disasm_trace)

    cursor2.execute("SELECT id FROM tblproblem WHERE tgt_model_id = %s AND name LIKE '%FA%'", (problem['tgt_model_id'],))
    ids = [row['id'] for row in cursor2]
    if len(ids) == 0:
        print("No corresponding FA exists. Skipping")
        continue
    best_asm_trace = find_best_trace(ids)
    asm_url = trace_url(best_asm_trace)
    print(best_asm_trace)
    cursor2.close()

    outfile = '%s.nbt' % (problem['name'],)
    os.system('curl -o disasm.nbt "' + disasm_url + '"')
    os.system('curl -o asm.nbt "' + asm_url + '"')
    os.system('head -c-1 disasm.nbt > %s' % (outfile,))
    os.system('cat asm.nbt >> %s' % (outfile,))

    digest, url = submit.upload_to_s3(Path(outfile))
    os.system('curl -X POST -F author=osak -F comment=combiner -F s3url="%s"' % (url,))
