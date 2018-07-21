require 'pp'

base = File.read('probl_sha1.txt').lines.map(&:split).map{|a| [a[0], [a[1]]]}.to_h
File.read('probf_sha1.txt').lines.map(&:split).each do |sha, f|
  base[sha] ||= []
  base[sha] << f
end

=begin
base.each do |key, val|
  puts "#{val[0]} = #{val[1..-1].join(', ')}"
end
=end

values = []
base.each_value do |val|
  key, *val = val
  if key =~ /^LA/
    key_name = key.sub(/_tgt.mdl/, '')
  else
    key_name = key.sub(/.mdl/, '')
  end
  val.each do |v|
    if v =~ /^FR/
      val_name = v.sub(/.mdl/, '')
    else
      val_name = v.sub(/_(tgt|src).mdl/, '')
    end
    values << "('#{key_name}', '#{val_name}')"
  end
end
puts "INSERT INTO tblmodel_name_alias (base, alias) VALUES #{values.join(',')}"
