ALTER TABLE tblproblem ADD COLUMN dummy_problem TINYINT DEFAULT FALSE;
INSERT INTO tblproblem (name, type, src_model_id, tgt_model_id, is_lightning, dummy_problem) VALUES
  ('FD187', 'DISASSEMBLY', 559, NULL, 0, 1), -- FR035_src
  ('FD188', 'DISASSEMBLY', 560, NULL, 0, 1), -- FR039_src
  ('FD189', 'DISASSEMBLY', 561, NULL, 0, 1), -- FR047_src
  ('FD190', 'DISASSEMBLY', 562, NULL, 0, 1), -- FR066_src
  ('FD191', 'DISASSEMBLY', 564, NULL, 0, 1), -- FR074_src
  ('FD192', 'DISASSEMBLY', 565, NULL, 0, 1), -- FR076_src
  ('FD193', 'DISASSEMBLY', 566, NULL, 0, 1), -- FR077_src
  ('FD194', 'DISASSEMBLY', 567, NULL, 0, 1), -- FR086_src
  ('FD195', 'DISASSEMBLY', 568, NULL, 0, 1), -- FR089_src
  ('FD196', 'DISASSEMBLY', 569, NULL, 0, 1), -- FR101_src
  ('FD197', 'DISASSEMBLY', 570, NULL, 0, 1), -- FR103_src
  ('FD198', 'DISASSEMBLY', 571, NULL, 0, 1), -- FR113_src
  ('FD199', 'DISASSEMBLY', 573, NULL, 0, 1), -- FR115_src
  ('FA187', 'ASSEMBLY', NULL, 563, 0, 1), -- FR070_tgt
  ('FA188', 'ASSEMBLY', NULL, 572, 0, 1); -- FR114_tgt

