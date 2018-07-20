for f in *.png; do
    convert -resize 400 $f thumbnails/$f
done
