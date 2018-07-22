require 'json'
require 'pp'

if ARGV[0] == '--skip-check'
  puts "Skip check mode"
  skip_check = true
end

Dir.chdir(__dir__)

traces = JSON.parse(`curl http://nanachi.kadingel.osak.jp/api/pending_traces`)
traces['traces'].each do |trace|
  puts "trace_id #{trace['trace_id']}: #{trace['model_name']} by #{trace['author']}"
  nbt_file = "/tmp/autoscorer-#{Process.pid}-#{trace['trace_id']}.nbt"
  `curl -o #{nbt_file} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/blob`

  if trace['src_model']
    src_mdl_file = "/tmp/autoscorer-#{Process.pid}-#{trace['src_model']}.mdl"
    `curl -o #{src_mdl_file} http://nanachi.kadingel.osak.jp/models/#{trace['src_model']}/blob`
  end
  if trace['tgt_model']
    tgt_mdl_file = "/tmp/autoscorer-#{Process.pid}-#{trace['tgt_model']}.mdl"
    `curl -o #{tgt_mdl_file} http://nanachi.kadingel.osak.jp/models/#{trace['tgt_model']}/blob`
  end

  if src_mdl_file && tgt_mdl_file
    puts "This is reassembly problem"
    score = `../autoscorer/autoscorer #{skip_check ? '--aperture-science-dangerously-skip-sanity-check' : ''} --reassembly #{src_mdl_file} #{tgt_mdl_file} #{nbt_file}`
  elsif src_mdl_file
    puts "This is disassembly problem"
    score = `../autoscorer/autoscorer #{skip_check ? '--aperture-science-dangerously-skip-sanity-check' : ''} --disassembly #{src_mdl_file} #{nbt_file}`
  else
    puts "This is assembly problem"
    score = `../autoscorer/autoscorer #{tgt_mdl_file} #{nbt_file}`
  end

  if $?.success?
    puts "#{trace['model_name']}: #{score}"
    puts `curl -X POST -F energy=#{score.to_i} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
  else
    puts "#{trace['model_name']}: Failed"
    puts `curl -X POST -F failed=true http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
  end
end

