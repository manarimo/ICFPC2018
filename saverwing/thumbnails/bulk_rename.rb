files = `ls *.png`.split
files.each do |file|
  to_name = file.gsub(/_tgt/, '')
  puts "#{file} -> #{to_name}"
  `git mv #{file} #{to_name}`
end
