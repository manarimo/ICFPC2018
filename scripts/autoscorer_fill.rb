require 'json'
require 'pp'

Dir.chdir(__dir__)

traces = JSON.parse(`curl http://nanachi.kadingel.osak.jp/api/pending_traces`)
traces['traces'].each do |trace|
  puts "trace_id #{trace['trace_id']}: #{trace['model_name']} by #{trace['author']}"
  nbt_file = "/tmp/autoscorer-#{Process.pid}-#{trace['trace_id']}.nbt"
  mdl_file = "/tmp/autoscorer-#{Process.pid}-#{trace['model_name']}.mdl"
  `curl -o #{nbt_file} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/blob`
  `curl -o #{mdl_file} http://nanachi.kadingel.osak.jp/models/#{trace['model_name']}/blob`
  score = `../autoscorer/autoscorer #{mdl_file} #{nbt_file}`
  if $?.success?
    puts "#{trace['model_name']}: #{score}"
    puts `curl -X POST -F energy=#{score.to_i} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
  else
    puts "#{trace['model_name']}: Failed"
    puts `curl -X POST -F failed=true http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
  end
end

