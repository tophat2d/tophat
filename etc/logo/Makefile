
all: normal normal-notext white white-notext big big-white

clean:
	rm -rf logo-normal.png logo-notext.png logo-normal-white.png logo-notext-white.png logo-big.png logo-big-white.png

help:
	@echo "options:"
	@echo "    normal        - default logo"
	@echo "    normal-notext - default logo, but without the engine label"
	@echo "    white         - default logo, but white"
	@echo "    white-notext  - white logo, but without the engine label"
	@echo "    big           - version of the logo, where engine label is next to the hat"
	@echo "    big-white     - big logo, but it's white"

normal:
	convert logos.png -crop 21x18+2+3 logo-normal.png

normal-notext:
	convert logos.png -crop 21x14+2+3 logo-notext.png

white:
	convert logos.png -crop 21x18+25+3 logo-normal-white.png

white-notext:
	convert logos.png -crop 21x14+25+3 logo-notext-white.png

big:
	convert logos.png -crop 118x14+1+27 logo-big.png

big-white:
	convert logos.png -crop 118x14+1+43 logo-big-white.png
