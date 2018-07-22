#!/usr/bin/env ruby

require 'json'
require 'pp'

problem_id, author, comment, run_script, temp_dir = ARGV

work_dir = File.expand_path(__dir__ + '/..')
if temp_dir.nil?
  temp_dir = '/tmp'
end

Dir.chdir("#{work_dir}/#{File.dirname(run_script)}")
if problem_id =~ /^FA/
  puts "This is assembly problem"
  input_models = ["#{work_dir}/assets/problemsF/#{problem_id}_tgt.mdl"]
  scorer_option = ""
elsif problem_id =~ /^FD/
  puts "This is disassembly problem"
  input_models = ["#{work_dir}/assets/problemsF/#{problem_id}_src.mdl"]
  scorer_option = "--disassembly"
elsif problem_id =~ /^FR/
  puts "This is reassembly problem"
  input_models = ["#{work_dir}/assets/problemsF/#{problem_id}_src.mdl", "#{work_dir}/assets/problemsF/#{problem_id}_tgt.mdl"]
  scorer_option = "--reassembly"
end
asm_path = "#{temp_dir}/#{problem_id}.nbtasm"
nbt_path = "#{temp_dir}/#{problem_id}.nbt"
`bash "#{work_dir}/#{run_script}" #{input_models.join(' ')} > "#{asm_path}"`

Dir.chdir("#{work_dir}/hissen/src")
`javac Main.java`
`java Main "#{asm_path}" "#{nbt_path}"`

json = JSON.parse(`curl -X POST -F author="#{author}" -F name=#{problem_id} -F comment="#{comment}" -F nbt-blob=@"#{nbt_path}" http://nanachi.kadingel.osak.jp/traces/register`)
pp json

Dir.chdir("#{work_dir}/autoscorer")
`make`
energy = `./autoscorer #{scorer_option} #{input_models.join(' ')} "#{nbt_path}"`.to_i
if $?.success?
  puts "energy=#{energy}"
  json = JSON.parse(`curl -X POST -F energy=#{energy} http://nanachi.kadingel.osak.jp/traces/#{json['trace_id']}/update-autoscorer`)
  if json['status'] == 'failure'
    puts json
    exit 1
  end
else
  puts "Autoscorer failed"
  json = JSON.parse(`curl -X POST -F failed=true http://nanachi.kadingel.osak.jp/traces/#{json['trace_id']}/update-autoscorer`)
  if json['status'] == 'failure'
    puts json
    exit 1
  end
end