CREATE TABLE tblofficial_ranking (
  id int(10) PRIMARY KEY AUTO_INCREMENT,
  problem_id int(10) NOT NULL,
  name VARCHAR(200) NOT NULL,
  energy BIGINT NOT NULL,
  UNIQUE INDEX problem_id (problem_id)
) DEFAULT CHARSET utf8mb4;