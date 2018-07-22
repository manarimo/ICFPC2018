INSERT INTO tblproblem (name, type, src_model_id, tgt_model_id, is_lightning)
SELECT name, 'REASSEMBLY', CAST(GROUP_CONCAT(src_id) AS SIGNED) AS src_id, CAST(GROUP_CONCAT(tgt_id) AS SIGNED) AS tgt_id, 0 FROM
  (SELECT IF(SUBSTRING_INDEX(name, '_', -1)='src', id, NULL) AS src_id,
   IF(SUBSTRING_INDEX(name, '_', -1)='tgt', id, NULL) AS tgt_id,
   SUBSTRING_INDEX(name, '_', 1) AS name FROM tblmodel m 
   UNION
   SELECT IF(SUBSTRING_INDEX(mna.alias, '_', -1)='src', m.id, NULL) AS src_id, 
   IF(SUBSTRING_INDEX(mna.alias, '_', -1)='tgt', m.id, NULL) AS tgt_id, 
   SUBSTRING_INDEX(mna.alias, '_', 1) AS name FROM tblmodel_name_alias mna 
   JOIN tblmodel m on m.name = mna.base) t 
WHERE name LIKE 'FR%' GROUP BY name ORDER BY name;