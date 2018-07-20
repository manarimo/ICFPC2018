for f in `ls ../assets/problemsL/*_tgt.mdl`; do
    name=$(basename $f)
    echo -n INSERT IGNORE INTO tblmodel_metadata \(model_id, r, fill_count, num_components, largest_component_size, max_depth, num_void_spaces\) SELECT id,\ 
    ./a.out < $f 2> /dev/null
    echo \ FROM tblmodel WHERE name = \"${name%_*}\"\;
done