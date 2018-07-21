CREATE TABLE `tblmodel_metadata` (
  `model_id` int(11) NOT NULL,
  `r` int(11) NOT NULL,
  `fill_count` int(11) NOT NULL,
  `num_components` int(11) NOT NULL,
  `largest_component_size` int(11) NOT NULL,
  `max_depth` int(11) NOT NULL,
  `num_void_spaces` int(11) DEFAULT NULL,
  PRIMARY KEY (`model_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE tbltrace_metadata (
  trace_id int NOT NULL,
  energy bigint NOT NULL,
  PRIMARY KEY (`trace_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;