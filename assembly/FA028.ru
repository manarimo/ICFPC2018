23.step(1, -2) do |i|
puts "gvoid 0 -1 0 %d 0 %d" %[i,i]
puts "gvoid 0 -1 0 -%d 0 -%d" %[i,i]
puts "lmove -1 0 0 0 0 -1"
puts "gvoid 0 -1 0 %d 0 -%d" %[i,i]
puts "lmove 1 0 0 0 0 -1"
puts "gvoid 0 -1 0 -%d 0 %d" %[i,i]
puts "lmove -1 0 0 0 0 1"
puts "lmove 1 0 0 0 0 1"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "smove 0 1 0"
puts "lmove 1 0 0 0 0 1"
puts "lmove -1 0 0 0 0 -1"
puts "gfill 0 -1 0 -%d 0 -%d" %[i+2,i+2]
puts "lmove 1 0 0 0 0 -1"
puts "gfill 0 -1 0 %d 0 -%d" %[i+2,i+2]
puts "lmove -1 0 0 0 0 1"
puts "gfill 0 -1 0 -%d 0 %d" %[i+2,i+2]
puts "gfill 0 -1 0 %d 0 %d" %[i+2,i+2]
end