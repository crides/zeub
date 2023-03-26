from typing import TypeVar, Callable
import cadquery as cq, numpy as np, pcbnew
from cadquery.selectors import *
from kicaq import *

pcb_thick = 1.6
fillet_r = 3
top_cover_thick, bot_cover_thick = 2, 2
shell_thick = 1.5
led_wall = 0.5
led_pitch = 4
leds = np.array((8 + 32, 8))
shell_out = np.array((2.5, 7))
shell_out_tol = 0.5
top_shell_h, bot_shell_h = 6, 6

top_bottom_lip = 5
sdcard_h, sdcard_entry_off = 2, 3
oled_h, oled_win = 5, (26, 15)
encoder_h, encoder_off, encoder_win = 7.5, (0.7, 1), (15, 12.5)
speaker_h, speaker_grill, speaker_grill_pitch, speaker_grill_hole_d = 3.5, (4, 2), 4, 2
mic_h, mic_hole_d = 1.3, 2
reset_h, reset_hole_d = 2, 3
joystick_h, joystick_hole_d = 2.5, 9
jack_h, jack_hole_d = 2, 10
usbc_hole = (7, 13)
led_strip_z_off, led_strip_hole, led_strip_thick, led_strip_hole_len = 3, 7, 1.75, 10
screw_hole_depth, screw_hole_d, screw_hole_off = 6, 3.5, 4
led_hole = 3.5
jack_support_thick = 4
temp_slot = (7.65, 4.2)
pcb_size = np.array((180, 60))

T = TypeVar("T")

def partition(l: list[T], f: Callable[[T], bool]) -> tuple[list[T], list[T]]:
    t, n = [], []
    for i in l:
        if f(i):
            t.append(i)
        else:
            n.append(i)
    return t, n

def unwrap(v: T | None) -> T:
    if v is None:
        raise ValueError()
    return v

board = Board("zeub.kicad_pcb")
oled = unwrap(board.convert_shape([d for d in board.board.GetDrawings() if isinstance(d, pcbnew.PCB_SHAPE) and d.GetShape() == pcbnew.SHAPE_T_RECT and d.GetLayer() == pcbnew.F_SilkS][:1]))
speakers = [board.convert_shape([s]) for s in board.layer_raw(pcbnew.User_1)]
encoder = board.fps_with_val("RotaryEncoder_Switch")[0]
sdcard = board.fps_with_val("Micro_SD_Card_Det")[0]
joystick = board.fps_with_val("SKRHA")[0]
comp_shell = board.layer(pcbnew.F_CrtYd)
reset = board.fps_with_val("SW_Push")[0]
temp = board.fps_with_val("TMP117")[0]
led = board.fps_with_val("LED")[0]
edge = board.convert_shape([e for e in board.edges_raw() if e.GetShape() != pcbnew.SHAPE_T_CIRCLE])
dup = lambda a: [a, a]

def make_shell(base, hole, height, cb=None, faces: list[str] = []):
    base = base.add(hole).toPending().cutBlind(height)
    size = hole.BoundingBox()
    shell = (cq.Workplane().add(hole).toPending().extrude(height).faces(" or ".join(["<Z"] + faces)).shell(shell_thick, kind="intersection"))
    if cb is None:
        base = base.union(shell, clean=False)
    else:
        base = base.union(cb(shell.faces(">Z").workplane(centerOption="CenterOfMass")), clean=False)
    return base

jack_holes = cq.Workplane().transformed(offset=(*board.pos("J4"), -pcb_thick - 7)).transformed((0, 90, 0)).cylinder(50, 6).union(cq.Workplane().pushPoints([board.pos(fp) for fp in board.fps_with_val("AudioJack4")]).each(lambda p: cq.Workplane().transformed(offset=(*p.toTuple()[:2], -pcb_thick - 2)).transformed((0, 90, 0)).cylinder(50, 3).val()))

base_rect_size = pcb_size + shell_out * 2
pcb_center = unwrap(edge).faces().val().CenterOfBoundBox().toTuple()[:2]
base_sketch = lambda: (cq.Workplane().moveTo(*pcb_center).sketch().rect(*base_rect_size).vertices().fillet(fillet_r))
shell_sketch = (base_sketch().reset()
                .rect(*(pcb_size + dup(shell_out_tol)), mode="s", tag="in")
                .vertices(tag="in").fillet(fillet_r))
temp_wire = (cq.Workplane().moveTo(*board.pos(temp)).sketch()
    .rarray(temp_slot[0], 1, 2, 1).circle(temp_slot[1] / 2).wires().hull().clean().finalize())

def gen_bbox(comps: list[Component]) -> cq.Workplane:
    bb = cq.Workplane().placeSketch(*[board.courtyard(fp) for fp in comps]).extrude(1).val().BoundingBox()
    return cq.Workplane().moveTo(bb.xmin, bb.ymin).rect(bb.xlen, bb.ylen, centered=False).wires()

def top_cover():
    base = base_sketch().finalize().extrude(top_cover_thick)
    comp_body = cq.Workplane().add(comp_shell).extrude(2)
    base -= comp_body
    base += comp_body.shell(shell_thick, kind="intersection").faces("<Z").workplane(offset=-2).split(keepBottom=True)
    oled_x = oled.faces().val().Center().x
    base = make_shell(base, oled.faces().wires().val().translate((0, 0, 2)), oled_h - 2, lambda s: s.rect(*oled_win).cutThruAll())
    base -= cq.Workplane().moveTo(oled_x, -pcb_size[1]).rect(15, 3, centered=(True, False)).extrude(1)
    base.moveTo(*board.pos(joystick)).hole(joystick_hole_d)
    base = make_shell(base.transformed(offset=(0,0,2)), board.courtyard(encoder).wires().val().translate((0, 0, 2)), encoder_h - 2, lambda s: s.move(*encoder_off).rect(*encoder_win).cutThruAll()).end()
    for speaker in speakers:
        base = make_shell(base, speaker.wires().val().translate((0, 0, 2)), speaker_h - 2, lambda s:
                          s.rarray(*dup(speaker_grill_pitch), *speaker_grill).hole(speaker_grill_hole_d))
    base += shell_sketch.finalize().extrude(-pcb_thick - top_shell_h)

    sdcard_block = cq.Workplane().add(unwrap(board.courtyard(sdcard))).extrude(sdcard_h).wires("<Y").toPending().transformed((90, 0, 0)).extrude(sdcard_entry_off, "s")
    base -= sdcard_block
    base -= cq.Workplane().add(sdcard_block.wires("<Y").toPending().extrude(20, combine=False)).wires(">Z").toPending().extrude(10)
    # base -= entry
    base -= cq.Workplane().moveTo(*board.pos(led)).cylinder(10, led_hole / 2)
    base -= jack_holes
    base -= cq.Workplane().transformed(offset=(*board.pos("J3"), -pcb_thick - 1.5)).transformed((0, 90, 0)).box(*usbc_hole, 20)
    base = base.add(temp_wire).toPending().cutThruAll()

    # faster to extrude 1 2D shape
    led_center = board.pos("D257") + led_pitch * (leds - 1) / 2 * (1, -1)
    led_rect_size = leds * led_pitch
    base = base.moveTo(*led_center).rect(*(led_rect_size - led_wall)).cutThruAll()
    base += cq.Workplane().transformed(offset=led_center).rect(*led_rect_size).rarray(*dup(led_pitch), *leds).rect(*dup(led_pitch - led_wall)).extrude(top_cover_thick)
    base = base.faces("<Z").workplane(invert=True).transformed(offset=pcb_center).rarray(*(base_rect_size - 2 * screw_hole_off), 2, 2).circle(screw_hole_d / 2).cutBlind(screw_hole_depth)
    return (base
            .faces(">Z").fillet(1)
            # .faces("<Z[1]").faces("<X or >X").fillet(1)
            # .faces("<Z[2]").faces("not <Y").fillet(1)
            # .faces("<Z[3]").faces(">Y").fillet(1)
            )

def bot_cover():
    bot_back = cq.Workplane().transformed(offset=(0, 0, -top_shell_h - bot_shell_h))
    bot_back_center = bot_back.transformed(offset=pcb_center)
    back = bot_back_center.placeSketch(base_sketch()).extrude(-bot_cover_thick)
    temp_solid = temp_wire.extrude(-top_shell_h - bot_shell_h - bot_cover_thick, taper=-30)
    back = back.add(temp_solid.wires("<Z")).toPending().cutThruAll()
    back += temp_solid.faces("|Z").shell(bot_cover_thick)
    back += bot_back_center.placeSketch(shell_sketch).extrude(bot_shell_h)
    back = back.faces(">Z").workplane().rarray(*(base_rect_size - 2 * screw_hole_off), 2, 2).circle(screw_hole_d / 2).cutThruAll()
    back += bot_back.transformed(offset=jack_support_corners[0]).rect(*jack_support_size, False).extrude(bot_shell_h) - jack_model
    back = back.solids(">X")
    back -= jack_hole
    for rod_pos in [(150, -5), (15, -47)]:
        back += bot_back.transformed(offset=rod_pos).cylinder(top_shell_h + bot_shell_h, 3, centered=(True, True, False))
    return back

top = top_cover()
show_object(top, "top cover")
# show_object(bot, "bot cover")
#cq.exporters.export(top_cover(), "top-cover.step")
