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
SELECT models.name, 'DISASSEMBLY', NULL, models.id, 0
FROM
  (SELECT id, name FROM tblmodel m 
   UNION 
  SELECT m.id AS id, mna.alias AS name FROM tblmodel_name_alias mna 
    JOIN tblmodel m on m.name = mna.base) models -- all models including aliases
WHERE name LIKE 'FD%' ORDER BY name;

-- fill problem_id
UPDATE tbltrace t,
  (SELECT t.id AS id, p.id AS problem_id FROM tbltrace t 
  JOIN tblmodel m ON t.model_id = m.id 
  JOIN tblproblem p ON m.name = p.name) p
SET t.problem_id = p.problem_id
WHERE t.id = p.id;