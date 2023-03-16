from xml.etree import ElementTree as ET
from dataclasses import dataclass
from typing import TypeVar
import itertools, csv

T = TypeVar("T")

def unwrap(v: T | None) -> T:
    if v is None:
        raise ValueError()
    return v

@dataclass
class Comp:
    ref: str
    val: str
    fp: str

def convert_bom(inf: str, outf: str) -> None:
    tree = ET.parse(inf).getroot()
    comps = list(list(tree.iterfind("components"))[0])
    def parse_comp(e: ET.Element) -> Comp:
        return Comp(unwrap(e.get("ref")), unwrap(next(e.iterfind("value")).text), unwrap(next(e.iterfind("footprint")).text).split(":")[1])
    comps = sorted([parse_comp(e) for e in comps], key=lambda c: (c.val, c.fp))
    groups = [((gs := list(_gs))[0].val, gs[0].fp, [g.ref for g in gs]) for _, _gs in itertools.groupby(comps, lambda c: (c.val, c.fp))]
    with open(outf, "w") as out:
        out.write("\n".join(itertools.chain(["Comment,Footprint,Designator,LCSC Part #"], (",".join((p[0], p[1], f"\"{','.join(p[2])}\"", "")) for p in groups))))

def convert_pos(inf: str, outf: str, top: bool = True) -> None:
    csv_opts = {"delimiter": ',', "quotechar": '"', "quoting": csv.QUOTE_MINIMAL, "lineterminator": "\n"}
    with open(inf) as inp:
        with open(outf, "w") as out:
            pos_in = csv.DictReader(inp, **csv_opts)
            pos_out = csv.DictWriter(out, ["Designator", "Mid X", "Mid Y", "Layer", "Rotation"], **csv_opts)
            pos_out.writeheader()
            for row in pos_in:
                ref, rot = row["Ref"], float(row["Rot"])
                if ref == "U7" or ref.startswith("Q") and ref[1] in "1234":
                    rot += 180
                if ref == "U3":
                    rot -= 90
                data = {"Designator": ref, "Mid X": row["PosX"], "Mid Y": row["PosY"], "Rotation": rot, "Layer": row["Side"]}
                pos_out.writerow(data)

if __name__ == "__main__":
    import os
    os.system("kicad-cli sch export python-bom zeub.kicad_sch")
    os.system("mkdir -p gerbers")
    os.system("kicad-cli pcb export drill -o gerbers/ --excellon-separate-th --separate-files --units mm zeub.kicad_pcb")
    os.system("kicad-cli pcb export gerbers -o gerbers/ -l F.Cu,B.Cu,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts zeub.kicad_pcb")
    os.system("zip -r zeub gerbers")
    os.system("kicad-cli pcb export pos zeub.kicad_pcb --units mm --format csv")
    convert_bom("zeub-bom.xml", "zeub-bom.csv")
    convert_pos("zeub.csv", "zeub-pos.csv")
