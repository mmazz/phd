
all:
	ffmpeg -f image2 -r 24 -i ../data/animation_imgs/animation%d.png -vcodec mpeg4 -y movie.mp4


.PHONY: clean

clean:
	rm -rf *.jpg
