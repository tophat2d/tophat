@echo off
setlocal enableDelayedExpansion

set modules=^
	umka/anim.um^
	umka/atlas.um^
	umka/audio.um^
	umka/canvas.um^
	umka/coll.um^
	umka/color.um^
	umka/csv.um^
	umka/ent.um^
	umka/font.um^
	umka/image.um^
	umka/input.um^
	umka/lerp.um^
	umka/misc.um^
	umka/nav.um^
	umka/particles.um^
	umka/placeholders.um^
	umka/rect.um^
	umka/rt.um^
	umka/signal.um^
	umka/th.um^
	umka/tilemap.um^
	umka/ui.um^
	umka/window.um
set module_names=%modules:umka/=%
set docs=%modules:.um=.md%
set umka=lib\umka\umka_windows_msvc\umka.exe
set em_target=src\staembed.c
set run=%umka% cmd\buildhelper.um
set images=etc/test.ff etc/icon.ff etc/button.ff

echo // Generated via cmd/win-buildmodules.bat > %em_target%
echo #include "tophat.h" >> %em_target%
for %%f in (%modules%) do (
	set pat=%%f
	set pat=!pat:.um=.md!
	%umka% cmd\mmdoc\mmdoc.um -t %%f > !pat!
)

%run% em th_em_modulesrc %modules% >> %em_target%
%run% argarr th_em_modulenames %module_names% >> %em_target%
%run% em th_em_misc LICENSE version >> %em_target%
%run% em th_em_moduledocs %docs% >> %em_target%
%run% placeholders %images% >> %em_target%
