require 'json'
require 'pp'
require 'timeout'
require 'open3'

if ARGV.index('--skip-check')
  puts "Skip check mode"
  skip_check = true
end

if ARGV.index('--skip-icfpc')
  puts "Skip icfpc mode"
  skip_icfpc = true
end

trace_id_threshold = ENV['TRACE_ID_THRESHOLD'] ? ENV['TRACE_ID_THRESHOLD'].to_i : nil

Dir.chdir(__dir__)

traces = JSON.parse(`curl http://nanachi.kadingel.osak.jp/api/pending_traces`)
traces['traces'].each do |trace|
  next if skip_icfpc && trace['author'] == 'icfpc2018'
  next if trace_id_threshold && trace['trace_id'] < trace_id_threshold

  puts "trace_id #{trace['trace_id']}: #{trace['model_name']} by #{trace['author']}"
  nbt_file = "/tmp/autoscorer-#{Process.pid}-#{trace['trace_id']}.nbt"
  if trace['s3url']
    `curl -o #{nbt_file} #{trace['s3url']}`
  else
    `curl -o #{nbt_file} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/blob`
  end

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
    command = "../autoscorer/autoscorer #{(skip_check && trace['author'] == 'icfpc2018') ? '--aperture-science-dangerously-skip-sanity-check' : ''} --reassembly #{src_mdl_file} #{tgt_mdl_file} #{nbt_file}"
  elsif src_mdl_file
    puts "This is disassembly problem"
    command = "../autoscorer/autoscorer #{(skip_check && trace['author'] == 'icfpc2018') ? '--aperture-science-dangerously-skip-sanity-check' : ''} --disassembly #{src_mdl_file} #{nbt_file}"
  else
    puts "This is assembly problem"
    command = "../autoscorer/autoscorer #{tgt_mdl_file} #{nbt_file}"
  end

  stdin, stdout, thr = Open3.popen2(command)
  arr = IO.select([stdout], [], [], (ENV['TIMEOUT'] || 180).to_i)
  if arr
    out = stdout.read
    status = thr.value
    if status.success?
      score = out.to_i
      puts "#{trace['model_name']}: #{score}"
      puts `curl -X POST -F energy=#{score} http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
    else
      puts "#{trace['model_name']}: Failed"
      puts `curl -X POST -F failed=true http://nanachi.kadingel.osak.jp/traces/#{trace['trace_id']}/update-autoscorer`
    end
  else
    puts "autoscorer timed out. Skip evaluating this problem for this time."
    Process.kill('TERM', thr[:pid])
    begin
      Process.wait(thr[:pid])
    rescue
    end
  end
end

