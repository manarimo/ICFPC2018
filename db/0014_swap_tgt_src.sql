UPDATE tblproblem SET tgt_model_id = src_model_id, src_model_id = NULL where type = 'ASSEMBLY';
UPDATE tblproblem SET src_model_id = tgt_model_id, tgt_model_id = NULL where type = 'DISASSEMBLY';