7.step(27, 2) do |i|
    puts "lmove -1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
    puts "smove 0 0 -1"
    puts "smove 0 0 -1"
    puts "smove 0 0 1"
    puts "smove 0 0 1"
    puts "smove 0 0 -1"
    puts "smove 0 0 -1"
    puts "smove 0 0 1"
    puts "smove 0 0 1"
    puts "gfill 0 -1 0 %d 0 %d" % [i,i]
    puts "gfill 0 -1 0 -%d 0 %d" % [i,i]
    puts "gfill 0 -1 0 -%d 0 -%d" % [i,i]
    puts "gfill 0 -1 0 %d 0 -%d" % [i,i]
    puts "gvoid 0 -1 0 %d 0 %d" % [i-4,i-4]
    puts "gvoid 0 -1 0 -%d 0 %d" % [i-4,i-4]
    puts "gvoid 0 -1 0 -%d 0 -%d" % [i-4,i-4]
    puts "gvoid 0 -1 0 %d 0 -%d" % [i-4,i-4]
    puts "lmove -1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
end
puts "---"
25.step(5, -2) do |i|
    puts "lmove 1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
    puts "smove 0 0 1"
    puts "smove 0 0 1"
    puts "smove 0 0 -1"
    puts "smove 0 0 -1"
    puts "smove 0 0 1"
    puts "smove 0 0 1"
    puts "smove 0 0 -1"
    puts "smove 0 0 -1"
    puts "gfill 0 -1 0 %d 0 %d" % [i,i]
    puts "gfill 0 -1 0 -%d 0 %d" % [i,i]
    puts "gfill 0 -1 0 -%d 0 -%d" % [i,i]
    puts "gfill 0 -1 0 %d 0 -%d" % [i,i]
    puts "gvoid 0 -1 0 %d 0 %d" % [i-4,i-4]
    puts "gvoid 0 -1 0 -%d 0 %d" % [i-4,i-4]
    puts "gvoid 0 -1 0 -%d 0 -%d" % [i-4,i-4]
    puts "gvoid 0 -1 0 %d 0 -%d" % [i-4,i-4]
    puts "lmove 1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
    puts "lmove -1 0 0 0 1 0"
    puts "lmove 1 0 0 0 1 0"
end