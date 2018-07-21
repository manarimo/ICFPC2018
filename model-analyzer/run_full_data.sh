#!/bin/bash
files=(FR035_src.mdl FR039_src.mdl FR047_src.mdl FR066_src.mdl FR070_tgt.mdl FR074_src.mdl FR076_src.mdl FR077_src.mdl FR086_src.mdl FR089_src.mdl FR101_src.mdl FR103_src.mdl FR113_src.mdl FR114_tgt.mdl FR115_src.mdl)
for name in "${files[@]}"; do
    f="../assets/problemsF/${name}"
    echo -n INSERT IGNORE INTO tblmodel_metadata \(model_id, r, fill_count, num_components, largest_component_size, max_depth, num_void_spaces\) SELECT id,\ 
    ./a.out < $f 2> /dev/null
    echo \ FROM tblmodel WHERE name = \"${name%.*}\"\;
done
