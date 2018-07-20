for f in raw/*.png; do
    target="thumbnails/$(basename $f)"
    if [ ! -f ${target} ]; then
        echo "Convert $f"
        convert -resize 400 $f thumbnails/$(basename $f)
    fi
done
