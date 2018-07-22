ALTER TABLE tbltrace ADD UNIQUE `sha1_model_id` (sha1, model_id);
ALTER TABLE tbltrace DROP INDEX sha1;