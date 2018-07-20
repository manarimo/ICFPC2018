for f in raw/*.png; do
    target="thumbnails/$(basename $f)"
    if [ ! -f ${target} ]; then
        echo "Convert $f"
        convert -trim -resize 300 -gravity center -extent 400x400 -background black $f thumbnails/$(basename $f)
    fi
done
