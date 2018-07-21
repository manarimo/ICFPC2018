require 'pp'

base = File.read('probl_sha1.txt').lines.map(&:split).map{|a| [a[0], [a[1]]]}.to_h
File.read('probf_sha1.txt').lines.map(&:split).each do |sha, f|
  base[sha] ||= []
  base[sha] << f
end

base.each do |key, val|
  puts "#{val[0]} = #{val[1..-1].join(', ')}"
end
