require 'nokogiri'
require 'json'

html = `curl https://icfpcontest2018.github.io/full/live-standings.html`
doc = Nokogiri::HTML(html)

values = []
doc.xpath('//h3[starts-with(@id, "problem-")]').each do |problem_node|
  problem_id = problem_node['id'].match(/problem-(.*)/)[1]
  score_node = problem_node.xpath('following-sibling::table[1]/tbody/tr')
  best_name = score_node.xpath('td[1]/pre/text()')[0].to_s
  best_score = score_node.xpath('td[2]/pre/text()')[0].to_s.to_i

  puts "#{problem_id}: #{best_name} #{best_score}"
  values << { problem_name: problem_id.tr('a-z', 'A-Z'), name: best_name, score: best_score }
end

puts `curl -X POST -H 'Content-type: application/json' --data '#{values.to_json}' http://nanachi.kadingel.osak.jp/rankings/update`
