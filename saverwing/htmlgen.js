console.log('<table border>');
for(var i = 1; i <= 186; ++i) {
    var label;
    if (i < 10) label = `00${i}`;
    else if (i < 100) label = `0${i}`;
    else label = `${i}`;
    console.log(`<tr><td>${label}</td><td><img src="thumbnails/LA${label}_tgt.mdl.png"></td><td><img src="thumbnails/LA${label}_tgt.mdl-up-45deg.png"</td><td><img src="thumbnails/LA${label}_tgt.mdl-up-315deg.png"</td></tr>`);
}
console.log('</table>');