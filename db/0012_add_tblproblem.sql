CREATE TABLE tblproblem (
    id int(10) PRIMARY KEY AUTO_INCREMENT,
    name varchar(200) NOT NULL,
    type enum('ASSEMBLY', 'DISASSEMBLY', 'REASSEMBLY'),
    src_model_id int(10) DEFAULT NULL,
    tgt_model_id int(10) DEFAULT NULL,
    is_lightning tinyint(1) NOT NULL,
    UNIQUE `name` (name)
) DEFAULT CHARSET utf8mb4;

ALTER TABLE tbltrace ADD problem_id int(10) NOT NULL;
ALTER TABLE tbltrace ADD UNIQUE problem_id_sha1 (problem_id, sha1);
ALTER TABLE tbltrace DROP INDEX sha1_model_id;
ALTER TABLE tblmodel_metadata DROP lightning;