#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
"""Make the site (GitHub Pages):

  index.html                  redirect to the installer
  firmware/felucca-VER.fwsc   the package
  webapp/installer/index.html index_pkg.html with fm1pkg.js, fm1ota.js and the metadata inlined
  webapp/editor/index.html    editor.html (+ fukiai.ttf, FUKIAI-LICENSE.txt)
  src/                        not touched

  web/make_site.py build/felucca-X.Y.fwsc X.Y OUT_DIR [--beta]
  (--beta: web/beta_banner.html at the top of the installer: the beta channel, docs/beta)

  web/make_site.py --pro X.Y OUT_DIR build/clipper-pro.fwsc [build/clipper-pro-elite.fwsc build/clipper-pro-mke.fwsc]
  Clipper Pro (the SMK-37 build): webapp/installer/ from index_pro.html with smkpkg.js, smkota.js (genkigenki's
  Windows-safe update library, as SLOOP Pro's) and one package per model (it installs the one for the connected
  keyboard), firmware/clipper-pro[-elite|-mke]-X.Y.fwsc, the same editor.

The package identity (FM-1_9xx) is read from the package; the device reports it
after the install.
"""
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
BLOCKS, BLK, KEEP = 20, 0x30, 0x2F


def strip_module(src):
    src = re.sub(r"^export\s+", "", src, flags=re.M)
    return re.sub(r"^import .*?;\n", "", src, flags=re.M)


def product_of(raw):
    """the package identity: one marker byte after each of the first 20 blocks (fm1pkg.js productOf)"""
    return "".join(chr((m - i - 1) & 0xFF) for i in range(BLOCKS) if (m := raw[i * BLK + KEEP]) != 0x7D)


def main(pkg, version, out):
    pkg, out = Path(pkg), Path(out)
    raw = pkg.read_bytes()
    product = product_of(raw)
    if not re.fullmatch(r"(FM-1|SMK-37 Pro|SMK-37 Elite|MKE-P37)_9\d\d", product):
        raise SystemExit(f"{pkg}: identity {product!r} is not a Felucca package (FM-1_9xx, SMK-37 Pro_9xx, SMK-37 Elite_9xx, MKE-P37_9xx)")
    if b"FELUCCA-LOADER-1" not in raw:              # marker of firmware/loader
        raise SystemExit(f"{pkg}: no Felucca update loader in it")
    html = (HERE / "index_pkg.html").read_text(encoding="utf-8")
    lib = strip_module((HERE / "fm1pkg.js").read_text(encoding="utf-8")) + "\n" + \
        strip_module((HERE / "fm1ota.js").read_text(encoding="utf-8"))
    name = f"clipper-{re.sub(r'[^A-Za-z0-9.-]', '-', version)}.fwsc"
    meta = json.dumps({"version": version, "product": product, "pkg": "../../firmware/" + name,
                       "sha256": hashlib.sha256(raw).hexdigest()})   # the page checks the download against it
    for mark in ("/*LIB*/", "/*META*/"):
        if html.count(mark) != 1:
            raise SystemExit(f"index_pkg.html must contain {mark} once; update make_site.py")
    html = html.replace("/*LIB*/", lib).replace("/*META*/", meta)
    logo = HERE.parent / "assets" / "logo" / "clipper-logo.svg"   # the Clipper logo, inline
    html = html.replace("<!--LOGO-->", logo.read_text(encoding="utf-8") if logo.exists() else "<b>Clipper</b>")
    inst, ed, fw = out / "webapp" / "installer", out / "webapp" / "editor", out / "firmware"
    for d in (inst, ed, fw):
        d.mkdir(parents=True, exist_ok=True)
    guide = HERE.parent / "output/pdf/Studio-0.3-guide-rapide-FR.pdf"
    guide_link = ""
    if guide.exists() and "--studio-guide" in sys.argv:   # LIVE: that guide describes the removed RYTHME tab
        shutil.copy(guide, out / guide.name)
        guide_link = '<p lang="fr"><a href="../../' + guide.name + '">Guide rapide illustré (PDF, 4 pages)</a></p>'
    html = html.replace("<!--STUDIO_GUIDE-->", guide_link)
    banner = ""
    if "--beta" in sys.argv:                       # the beta channel (docs/beta): what it is, what it implies
        notes = HERE / "beta_banner.html"
        banner = notes.read_text(encoding="utf-8") if notes.exists() else "<p><b>BETA</b></p>"
    html = html.replace("<!--BANNER-->", banner)
    for old in list(fw.glob("felucca-*.fwsc")) + list(fw.glob("sloop-*.fwsc")) + list(fw.glob("clipper-*.fwsc")):   # one package: the current one
        old.unlink()
    (inst / "index.html").write_text(html, encoding="utf-8")
    shutil.copy(pkg, fw / name)
    shutil.copy(HERE / "editor.html", ed / "index.html")
    for f in ("fukiai.ttf", "FUKIAI-LICENSE.txt"):
        if (HERE / f).exists():
            shutil.copy(HERE / f, ed / f)
    (out / "index.html").write_text(
        '<!doctype html><meta charset="utf-8"><title>Clipper</title>'
        '<meta http-equiv="refresh" content="0; url=webapp/installer/">'
        '<a href="webapp/installer/">Clipper installer</a>\n', encoding="utf-8")
    print(f"site: {out}: webapp/installer ({len(html)} B), webapp/editor, firmware/{name} ({len(raw)} B, {product})")


PRO_MODELS = (("SMK-37 Pro", ""), ("SMK-37 Elite", "-elite"), ("MKE-P37", "-mke"))   # Clipper Pro: model -> file suffix


def main_pro(version, out, pkgs):
    """Clipper Pro's site: one package per SMK-37 model; the installer picks the one for the connected keyboard"""
    out = Path(out)
    vtag = re.sub(r"[^A-Za-z0-9.-]", "-", version)
    entries, names, raws = [], [], {}
    for pkg in map(Path, pkgs):
        raw = pkg.read_bytes()
        product = product_of(raw)
        m = re.fullmatch(r"(SMK-37 Pro|SMK-37 Elite|MKE-P37)_9\d\d", product)
        if not m:
            raise SystemExit(f"{pkg}: identity {product!r} is not a Clipper Pro package (SMK-37 Pro / SMK-37 Elite / MKE-P37_9xx)")
        if b"FELUCCA-LOADER-1" not in raw:
            raise SystemExit(f"{pkg}: no Felucca update loader in it")
        if m[1] in raws:
            raise SystemExit(f"{pkg}: a second package for {m[1]}")
        raws[m[1]] = (product, raw)
    if "SMK-37 Pro" not in raws:
        raise SystemExit("--pro: the SMK-37 Pro package is needed (it also serves the Starrykey 37 Play)")
    inst, ed, fw = out / "webapp" / "installer", out / "webapp" / "editor", out / "firmware"
    for d in (inst, ed, fw):
        d.mkdir(parents=True, exist_ok=True)
    for old in list(fw.glob("felucca-*.fwsc")) + list(fw.glob("sloop-*.fwsc")) + list(fw.glob("clipper-*.fwsc")):
        old.unlink()                                # the current packages only
    for model, sfx in PRO_MODELS:
        if model not in raws:
            continue
        product, raw = raws[model]
        name = f"clipper-pro{sfx}-{vtag}.fwsc"
        (fw / name).write_bytes(raw)
        names.append(name)
        entries.append({"product": product, "pkg": "../../firmware/" + name, "sha256": hashlib.sha256(raw).hexdigest()})
    html = (HERE / "index_pro.html").read_text(encoding="utf-8")
    lib = strip_module((HERE / "smkpkg.js").read_text(encoding="utf-8")) + "\n" + \
        strip_module((HERE / "smkota.js").read_text(encoding="utf-8"))
    for mark in ("/*LIB*/", "/*META*/"):
        if html.count(mark) != 1:
            raise SystemExit(f"index_pro.html must contain {mark} once; update make_site.py")
    html = html.replace("/*LIB*/", lib).replace("/*META*/", json.dumps({"version": version, "pkgs": entries}))
    logo = HERE.parent / "assets" / "logo" / "clipper-logo.svg"
    html = html.replace("<!--LOGO-->", logo.read_text(encoding="utf-8") if logo.exists() else "<b>Clipper</b>")
    banner = ""
    if "--beta" in sys.argv:
        notes = HERE / "beta_banner.html"
        banner = notes.read_text(encoding="utf-8") if notes.exists() else "<p><b>BETA</b></p>"
    html = html.replace("<!--BANNER-->", banner)
    (inst / "index.html").write_text(html, encoding="utf-8")
    shutil.copy(HERE / "editor.html", ed / "index.html")
    for f in ("fukiai.ttf", "FUKIAI-LICENSE.txt"):
        if (HERE / f).exists():
            shutil.copy(HERE / f, ed / f)
    (out / "index.html").write_text(
        '<!doctype html><meta charset="utf-8"><title>Clipper Pro</title>'
        '<meta http-equiv="refresh" content="0; url=webapp/installer/">'
        '<a href="webapp/installer/">Clipper Pro installer</a>\n', encoding="utf-8")
    print(f"site: {out}: webapp/installer ({len(html)} B), webapp/editor, firmware/" + ", ".join(
        f"{n} ({e['product']})" for n, e in zip(names, entries)))


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--pro":
        args = [a for a in sys.argv[2:] if a != "--beta"]
        if len(args) < 3:
            sys.exit(__doc__)
        main_pro(args[0], args[1], args[2:])
        sys.exit(0)
    args = [a for a in sys.argv[1:] if a not in ("--studio-guide", "--beta")]
    if len(args) != 3:
        sys.exit(__doc__)
    main(*args)
