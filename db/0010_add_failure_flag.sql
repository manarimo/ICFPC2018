ALTER TABLE tbltrace_metadata ADD COLUMN failed TINYINT DEFAULT NULL;
UPDATE tbltrace_metadata SET failed = FALSE WHERE energy_autoscorer IS NOT NULL;