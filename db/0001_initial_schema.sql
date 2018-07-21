DROP TABLE IF EXISTS `tblmodel`;
DROP TABLE IF EXISTS `tbltrace`;

CREATE TABLE `tblmodel` (
  id INT AUTO_INCREMENT,
  name VARCHAR(200) UNIQUE,
  body LONGBLOB,
  PRIMARY KEY (id),
  INDEX name_index (`name`)
) DEFAULT CHARSET utf8mb4;

CREATE TABLE `tbltrace` (
  id INT AUTO_INCREMENT,
  model_id INT,
  body LONGBLOB,
  score BIGINT,
  PRIMARY KEY (id),
  INDEX score_index (score)
) DEFAULT CHARSET utf8mb4;

