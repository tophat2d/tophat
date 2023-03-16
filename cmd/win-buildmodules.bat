@echo off

set modules=umka/anim.um umka/audio.um umka/csv.um umka/ent.um umka/image.um umka/input.um umka/misc.um umka/canvas.um umka/ray.um umka/rect.um umka/tilemap.um umka/window.um umka/ui.um umka/std/std.um umka/particles.um umka/lerp.um umka/font.um umka/th.um umka/signal.um umka/atlas.um umka/ui/label.um umka/ui/grid.um umka/ui/imagebox.um umka/shader.um umka/color.um umka/coll.um umka/placeholders.um umka/nav.um
set module_names=%modules:umka/=%
set umka=lib\umka\umka_windows_msvc\umka.exe
set em_target=src\staembed.c
set run=%umka% cmd\buildhelper.um
set images=etc/test.ff etc/icon.ff

echo. > %em_target%
echo #include "tophat.h" > %em_target%
%run% em th_em_modulesrc %modules% >> %em_target%
%run% argarr th_em_modulenames %module_names% >> %em_target%
%run% em th_em_misc LICENSE version >> %em_target%
%run% placeholders %images% >> %em_target%
