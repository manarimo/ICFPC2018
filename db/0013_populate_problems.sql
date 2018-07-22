-- lightning
INSERT INTO tblproblem (name, type, src_model_id, tgt_model_id, is_lightning)
SELECT models.name, 'ASSEMBLY', models.id, NULL, 1
FROM
  (SELECT id, name FROM tblmodel m 
   UNION 
  SELECT m.id AS id, mna.alias AS name FROM tblmodel_name_alias mna 
    JOIN tblmodel m on m.name = mna.base) models -- all models including aliases
WHERE name LIKE 'LA%' ORDER BY name;

-- full assembly
INSERT INTO tblproblem (name, type, src_model_id, tgt_model_id, is_lightning)
SELECT models.name, 'ASSEMBLY', models.id, NULL, 0
FROM
  (SELECT id, name FROM tblmodel m 
   UNION 
  SELECT m.id AS id, mna.alias AS name FROM tblmodel_name_alias mna 
    JOIN tblmodel m on m.name = mna.base) models -- all models including aliases
WHERE name LIKE 'FA%' ORDER BY name;

-- full disassembly
INSERT INTO tblproblem (name, type, src_model_id, tgt_model_id, is_lightning)
SELECT models.name, 'ASSEMBLY', NULL, models.id, 1
FROM
  (SELECT id, name FROM tblmodel m 
   UNION 
  SELECT m.id AS id, mna.alias AS name FROM tblmodel_name_alias mna 
    JOIN tblmodel m on m.name = mna.base) models -- all models including aliases
WHERE name LIKE 'FD%' ORDER BY name;

-- full reassembly