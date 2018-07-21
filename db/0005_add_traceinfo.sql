ALTER TABLE tbltrace_metadata ADD submit_time timestamp NOT NULL;
ALTER TABLE tbltrace_metadata ADD author varchar(20) NOT NULL DEFAULT '';
ALTER TABLE tbltrace_metadata ADD comment varchar(2000) NOT NULL DEFAULT '';