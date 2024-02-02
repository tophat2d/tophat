@echo off
setlocal enableDelayedExpansion

set modules=umka/anim.um umka/audio.um umka/csv.um umka/ent.um umka/image.um umka/input.um umka/misc.um umka/canvas.um umka/ray.um umka/rect.um umka/tilemap.um umka/window.um umka/particles.um umka/lerp.um umka/font.um umka/th.um umka/signal.um umka/atlas.um umka/shader.um umka/color.um umka/coll.um umka/placeholders.um umka/nav.um umka/ui.um
set module_names=%modules:umka/=%
set umka=lib\umka\umka_windows_msvc\umka.exe
set em_target=src\staembed.c
set run=%umka% cmd\buildhelper.um
set images=etc/test.ff etc/icon.ff etc/button.ff
set docs=umka/anim.md umka/audio.md umka/csv.md umka/ent.md umka/image.md umka/input.md umka/misc.md umka/canvas.md umka/ray.md umka/rect.md umka/tilemap.md umka/window.md umka/particles.md umka/lerp.md umka/font.md umka/th.md umka/signal.md umka/atlas.md umka/shader.md umka/color.md umka/coll.md umka/placeholders.md umka/nav.md umka/ui.md

echo. > %em_target%
echo #include "tophat.h" > %em_target%
for %%f in (%modules%) do (
	set pat=%%f
	set pat=!pat:.um=.md!
	%umka% cmd\mmdoc\mmdoc.um %%f > !pat!
)

%run% em th_em_modulesrc %modules% >> %em_target%
%run% argarr th_em_modulenames %module_names% >> %em_target%
%run% em th_em_misc LICENSE version >> %em_target%
%run% em th_em_moduledocs %docs% >> %em_target%
%run% placeholders %images% >> %em_target%
