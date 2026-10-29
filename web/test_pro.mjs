// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
//
// The Clipper Pro installer page end to end, without a browser or a keyboard (run from the repo root):
//   node web/test_pro.mjs
// web/make_site.py --pro builds a site from three synthetic packages (one per SMK-37 model); the page's own script
// (smkpkg.js and smkota.js inlined) then runs in a VM with a small DOM and a simulated keyboard on WebMIDI, and the
// Install button is pressed: each model gets its own package, a keyboard left in update mode is finished and
// checked, an FM-1, ChanceTheMaker's SMK firmwares and a damaged download get nothing; every string in fr / en / ja.

import { execFileSync } from "node:child_process";
import { mkdtempSync, readFileSync, writeFileSync, existsSync } from "node:fs";
import { tmpdir } from "node:os";
import { join, resolve, dirname } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";
import { webcrypto } from "node:crypto";
import { pack7, unpack7 } from "./smkota.js";

let failed = 0;
const ok = (cond, what) => { console.log(`${what.padEnd(72)} ${cond ? "ok" : "FAIL"}`); if (!cond) failed++; };
const HERE = fileURLToPath(new URL(".", import.meta.url));
const ROOT = resolve(HERE, "..");
const PYTHON = process.env.PYTHON || (process.platform === "win32" ? "python" : "python3");
const eq = (a, b) => a.length === b.length && a.every((v, i) => v === b[i]);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

/* ---- three packages, as tools/build.py writes them (identity markers, header, file list, the loader's marker) ---- */
const tmp = mkdtempSync(join(tmpdir(), "clipper-pro-site-"));
const MODELS = [["SMK-37 Pro", "", 11], ["SMK-37 Elite", "-elite", 12], ["MKE-P37", "-mke", 13]];
const files = MODELS.map(([m, sfx]) => join(tmp, `clipper-pro${sfx}.fwsc`));
execFileSync(PYTHON, ["-c", `
import sys
sys.path.insert(0, ${JSON.stringify(join(ROOT, "tools"))})
import fm1_install as I
def package(product, seed, size=0x3000):
    img = bytearray((i * seed) & 0xFF for i in range(size))
    img[0x1800:0x1810] = I.LOADER_MARK
    data = bytes(img[0x400:])
    e = bytearray(0x50)
    e[0:16] = bytes(4) + I._crc16(data).to_bytes(2, "little") + bytes(2) + (0x400).to_bytes(4, "little") + len(data).to_bytes(4, "little")
    e[0x40:0x49] = b"flash.bin"
    lst = I._dec(bytes(e))
    hdr = bytearray(0x40)
    hdr[4:8] = size.to_bytes(4, "little"); hdr[8:10] = (1).to_bytes(2, "little")
    hdr[2:4] = I._crc16(lst).to_bytes(2, "little"); hdr[0:2] = I._crc16(bytes(hdr[2:0x40])).to_bytes(2, "little")
    img[0:0x40] = I._dec(bytes(hdr)); img[0x40:0x90] = lst
    raw = bytearray()
    for i in range(20):
        m = (ord(product[i]) + i + 1) & 0xFF if i < len(product) else 0x7D
        raw += img[i * I.KEEP:(i + 1) * I.KEEP] + bytes([m])
    return bytes(raw + img[20 * I.KEEP:])
for (model, seed), path in zip(${JSON.stringify(MODELS.map(([m, , s]) => [m, s]))}, sys.argv[1:]):
    open(path, "wb").write(package(model + "_928", seed))
`, ...files]);
const site = join(tmp, "site");
const made = execFileSync(PYTHON, [join(HERE, "make_site.py"), "--pro", "2.8", site, ...files], { encoding: "utf8" });
const inst = join(site, "webapp/installer/index.html");
ok(existsSync(inst) && /clipper-pro-2\.8\.fwsc.*clipper-pro-elite-2\.8\.fwsc.*clipper-pro-mke-2\.8\.fwsc/s.test(made)
   && existsSync(join(site, "webapp/editor/index.html")),
   "make_site --pro: installer, editor, one package per model");
{
  let refused = false;
  try { execFileSync(PYTHON, [join(HERE, "make_site.py"), "--pro", "2.8", join(tmp, "x"), files[1]], { stdio: "pipe" }); }
  catch (_) { refused = true; }
  ok(refused, "make_site --pro: refused without the SMK-37 Pro package");
}
const page = readFileSync(inst, "utf8");
const IMAGES = Object.fromEntries(MODELS.map(([m, sfx]) => {
  const raw = readFileSync(join(site, `firmware/clipper-pro${sfx}-2.8.fwsc`));
  return [m, raw];
}));

/* ---- every string in fr / en / ja ---- */
{
  const src = readFileSync(join(HERE, "index_pro.html"), "utf8");
  const tb = src.slice(src.indexOf("const TEXT = {"), src.indexOf("\n};", src.indexOf("const TEXT = {")) + 2);
  const TEXT = vm.runInNewContext(tb.replace("const TEXT =", "(") + ")");
  const used = new Set([...src.matchAll(/data-t="(\w+)"|sayK\("(\w+)"|fail\("(\w+)"/g)].map((x) => x[1] || x[2] || x[3]));
  const ota = readFileSync(join(HERE, "smkota.js"), "utf8");
  for (const m of ota.matchAll(/fail\("(\w+)"/g)) used.add(m[1]);   // every error code the library throws
  for (const k of ["device", "pkgFor"]) used.add(k);
  const langs = Object.keys(TEXT);
  const miss = [...used].filter((k) => langs.some((l) => !(k in TEXT[l])));
  const odd = langs.flatMap((l) => Object.keys(TEXT[l]).filter((k) => langs.some((m) => !(k in TEXT[m]))));
  ok(langs.join() === "fr,en,ja" && !miss.length && !odd.length,
     `installer page: every string in fr, en and ja (${used.size} used${miss.length ? ", missing " + miss : ""}${odd.length ? ", odd " + odd : ""})`);
}

/* ---- a keyboard on WebMIDI: identity, then "device asks, host answers" reads of the image ---- */
const HS = [0xF0, 0x00, 0x32, 0x45, 0x00, 0x00, 0x00, 0x40, 0x7F, 0xF7];
const UPGRADE = [0xF0, 0x22, 0x24, 0x35, 0x7F, 0xF7];
class FakeKeyboard {
  constructor({ identity, name = "SMK-37 Pro", loader = null, finalIdentity = null, loaderName = "Felucca Update" }) {
    this.loader = loader; this.finalIdentity = finalIdentity; this.loaderName = loaderName;
    this.served = 0; this.upgrades = 0; this.written = []; this.image = null;
    this.access = { inputs: new Map(), outputs: new Map() };
    this.boot(identity, name);
  }
  boot(identity, name) {
    this.identity = identity; this.waiting = null; this.queue = [];
    if (this.access.inputs.size && this.chrome) this.chrome.leave();   /* it leaves the USB (a restart) */
    for (const m of [this.access.inputs, this.access.outputs]) { for (const p of m.values()) p.state = "disconnected"; m.clear(); }
    const id = Math.random().toString(36).slice(2);
    this.input = { id: "i" + id, name, state: "connected", onmidimessage: null, open: async () => {} };
    this.output = { id: "o" + id, name, state: "connected", open: async () => {}, send: (d) => {
      if (this.output.state !== "connected") throw new Error("InvalidStateError");
      if (this.chrome && this.chrome.wedged) return;            /* Chrome's MIDI thread is stuck: nothing leaves */
      setTimeout(() => this.rx(Array.from(d)), 1);
    } };
    this.access.inputs.set(this.input.id, this.input);
    this.access.outputs.set(this.output.id, this.output);
  }
  tx(bytes) {
    const i = this.input;
    setTimeout(() => { if (i.state === "connected" && i.onmidimessage && !(this.chrome && this.chrome.wedged)) i.onmidimessage({ data: Uint8Array.from(bytes) }); }, 1);
  }
  rx(d) {
    if (eq(d, HS)) {
      const t = [...new TextEncoder().encode(this.identity)];
      this.tx([0xF0, ...pack7([0, 0x59, 0x11, 0, 0, 0, ...t, ...new Array(28 - t.length).fill(0)]), 0xF7]);
    } else if (eq(d, UPGRADE)) {
      this.upgrades++;
      this.queue = this.identity.startsWith("ota-")
        ? [...Array.from({ length: 8 }, (_, k) => [k * 512, 512]), [0xF0000000, 8]]
        : [[0, 64], [0x40, 160], [0x1000, 512], [0xE0000000, 8]];
      this.next();
    } else if (this.waiting) {
      const u = unpack7(d.slice(1, -1));
      const [addr, len] = this.waiting;
      if (addr < 0xE0000000 && this.identity.startsWith("ota-")) this.written.push(...u.slice(14, 14 + len));
      this.waiting = null;
      this.served++;
      if (addr === 0xE0000000) setTimeout(() => this.boot(this.loader, this.loaderName), 300);
      else if (addr === 0xF0000000) setTimeout(() => this.boot(this.finalIdentity, "SMK-37 Pro"), 300);
      else this.next();
    }
  }
  next() {
    const r = this.queue.shift();
    if (!r) return;
    this.waiting = r;
    const [addr, len] = r;
    const u = [0, 0x59, 0x30, 0, 0, 0, 0, addr & 0xFF, (addr >>> 8) & 0xFF, (addr >>> 16) & 0xFF, (addr >>> 24) & 0xFF, len & 0xFF, len >> 8, 0];
    let s = 0;
    for (let i = 6; i < 14; i++) s += u[i];
    u.push(~s & 0xFF);
    this.tx([0xF0, ...pack7(u), 0xF7]);
  }
}
/* Chrome's MIDI, as Chromium's sources do it (media/midi/midi_service.cc and midi_manager_win.cc, Blink's MIDIPort):
   the browser opens every MIDI device (on Windows its WinMM handles) while any document holds a MIDIAccess, whatever
   the page's port.open() / close() say (Blink: "TODO: Add blink API to perform a real open and close operation"), and
   lets them go when the last such document ends: an iframe removed, the page's own document only with the tab.
   windows: a keyboard that leaves the USB while the browser holds it wedges Chrome's MIDI thread, the freeze of
   sloop-pro #3 (ChanceTheMaker: "MIDI is not released when the device reboots into flashing mode"; the WinMM call
   that blocks is to be confirmed on hardware): from then on no MIDI message moves, no new MIDIAccess is given, and
   the end of a session blocks the browser (here the event loop, 1.5 s) */
class ChromeMidi {
  constructor(dev, { windows = false } = {}) {
    this.dev = dev; this.windows = windows; this.docs = new Set(); this.wedged = false; this.froze = 0; this.leaves = 0; this.heldAtLeave = 0;
    dev.chrome = this;
  }
  request(doc) {                                     /* navigator.requestMIDIAccess() of a document */
    if (this.wedged) return new Promise(() => {});
    this.docs.add(doc);
    return Promise.resolve(this.dev.access);
  }
  end(doc) {                                         /* that document is gone */
    if (!this.docs.delete(doc) || !this.wedged) return;
    const t = Date.now() + 1500;
    while (Date.now() < t);                          /* the browser waits for its stuck MIDI thread */
    this.froze++;
  }
  leave() { this.leaves++; if (this.docs.size) { this.heldAtLeave++; if (this.windows) this.wedged = true; } }
}

/* the bytes a package gives the device for its first 4 KiB (its logical image), to tell which package was written */
const logical = (raw) => { const out = []; for (let i = 0; i < 20; i++) out.push(...raw.subarray(i * 0x30, i * 0x30 + 0x2F)); out.push(...raw.subarray(20 * 0x30)); return out; };

/* ---- the page in a VM: a small DOM, fetch from the site, the fake keyboard as navigator.requestMIDIAccess ---- */
async function runPage(dev, { model = "auto", risk = true, tamper = null, lang = "en", chrome = new ChromeMidi(dev) } = {}) {
  const els = new Map();
  const el = (id, extra = {}) => {
    if (!els.has(id)) els.set(id, { id, textContent: "", className: "", disabled: false, hidden: false, checked: false, value: "",
      scrollTop: 0, dataset: {}, listeners: {}, attrs: {}, setAttribute(k, v) { this.attrs[k] = v; },
      addEventListener(t, f) { (this.listeners[t] ||= []).push(f); }, ...extra });
    return els.get(id);
  };
  el("risk").checked = risk;
  const dataT = [...page.matchAll(/data-t="(\w+)"/g)].map((m, i) => el("t" + i, { dataset: { t: m[1] } }));
  const langs = ["fr", "en", "ja"].map((l) => el("lang-" + l, { dataset: { l } }));
  const radios = ["auto", "SMK-37 Pro", "SMK-37 Elite", "MKE-P37"].map((v) => el("r-" + v, { value: v, checked: v === model }));
  const frames = new Set();                         /* iframes: each document its own MIDIAccess (ChromeMidi) */
  const doc = {
    documentElement: { lang: "" },
    createElement: (tag) => ({ tag, hidden: false, attrs: {}, contentWindow: null, setAttribute(k, v) { this.attrs[k] = v; },
      remove() { if (frames.delete(this)) { this.contentWindow = null; chrome.end(this); } } }),
    body: { appendChild(f) {
      frames.add(f);
      f.contentWindow = { navigator: { requestMIDIAccess: () => chrome.request(f) } };
      return f;
    } },
    getElementById: (id) => el(id),
    querySelectorAll: (s) => s === "[data-t]" ? dataT : s === ".lang" ? langs : s === "input" ? [el("risk"), ...radios] : [],
    querySelector: (s) => (s.includes(":checked") ? radios.find((r) => r.checked) : null),
  };
  const win = { listeners: {}, addEventListener(t, f) { (this.listeners[t] ||= []).push(f); } };
  const base = dirname(inst);
  const fetchFile = async (url) => {
    let b = readFileSync(resolve(base, url));
    if (tamper) b = tamper(new Uint8Array(b));
    return { ok: true, status: 200, arrayBuffer: async () => Uint8Array.from(b).buffer };
  };
  const script = page.slice(page.indexOf('<script type="module">') + 22, page.lastIndexOf("</script>"));
  const ctx = vm.createContext({
    document: doc, window: win, location: { protocol: "file:" }, fetch: fetchFile, crypto: webcrypto, self: { crypto: webcrypto },
    navigator: { language: lang, requestMIDIAccess: () => chrome.request(doc) },   /* the page's own document: never ends */
    localStorage: { getItem: () => null, setItem: () => {} },
    setTimeout, clearTimeout, setInterval, clearInterval, console, TextEncoder, TextDecoder, Uint8Array, Promise, Date, Math, JSON, Error, Map, Set,
  });
  vm.runInContext(script, ctx);
  for (let i = 0; i < 250 && !/Ready|Prêt|準備|damaged|endommagé|壊れ|load|Chargement/.test(el("status").textContent); i++) await sleep(20);
  const status = () => el("status").textContent;
  const loaded = status();
  const go = el("go").listeners.click?.[0];
  if (!el("go").disabled && go) await go();
  return { loaded, status: status(), device: el("device").textContent, go: el("go"), cls: el("status").className, log: el("log").textContent, frames };
}

const t0 = Date.now();
for (const [model] of MODELS) {          /* each model, running the stock firmware: its own package */
  const dev = new FakeKeyboard({ identity: `${model}_016`, name: `${model} MIDI`, loader: `ota-${model}_928`, finalIdentity: `${model}_928` });
  const r = await runPage(dev);
  ok(/^Ready: Clipper Pro 2\.8/.test(r.loaded) && r.status === `Done: the keyboard runs ${model}_928` && dev.upgrades === 2
     && eq(dev.written, logical(IMAGES[model]).slice(0, 4096)) && r.device.includes(`→ package ${model}_928`),
     `page: an ${model} (stock 016) gets its own package, checked after the restart`);
}
{
  const dev = new FakeKeyboard({ identity: "STARRYKEY-37 PLAY_016", loader: "ota-SMK-37 Pro_928", finalIdentity: "SMK-37 Pro_928" });
  const r = await runPage(dev);
  ok(r.status === "Done: the keyboard runs SMK-37 Pro_928" && eq(dev.written, logical(IMAGES["SMK-37 Pro"]).slice(0, 4096)),
     "page: a Starrykey 37 Play gets the SMK-37 Pro package");
}
{
  const dev = new FakeKeyboard({ identity: "ota-MKE-P37_928", name: "USB MIDI Device", finalIdentity: "MKE-P37_928" });
  const r = await runPage(dev);
  ok(r.status === "Done: the keyboard runs MKE-P37_928" && dev.upgrades === 1 && eq(dev.written, logical(IMAGES["MKE-P37"]).slice(0, 4096)),
     "page: left in update mode (a generic port name): the MKE-P37 write finished and checked");
}
{
  const dev = new FakeKeyboard({ identity: "ota-SMK-37 Elite_928", finalIdentity: "SMK-37 Pro_928" });
  const r = await runPage(dev);
  ok(/^Written, but the keyboard reports another version: SMK-37 Pro_928/.test(r.status) && r.cls === "bad",
     "page: a resumed write that comes back as another model is reported, not 'done'");
}
{
  const dev = new FakeKeyboard({ identity: "SMK-37 Elite_016", loader: "ota-SMK-37 Pro_928", finalIdentity: "SMK-37 Pro_928" });
  const r = await runPage(dev, { model: "SMK-37 Pro" });
  ok(r.status === "Done: the keyboard runs SMK-37 Pro_928" && eq(dev.written, logical(IMAGES["SMK-37 Pro"]).slice(0, 4096)),
     "page: the model ticked by hand wins (an Elite given the Pro package)");
}
for (const [identity, key, what] of [["FM-1_015", "The connected device is an FM-1", "an FM-1"],
                                     ["SMK-37_954", "This keyboard runs one of ChanceTheMaker", "ChanceTheMaker's Sloop SMK"],
                                     ["ota-SMK-37_931", "This keyboard runs one of ChanceTheMaker", "... its updater"]]) {
  const dev = new FakeKeyboard({ identity, name: identity.startsWith("FM") ? "FM-1" : "SMK-37 Pro" });
  const r = await runPage(dev);
  ok(r.status.startsWith(key) && dev.upgrades === 0 && dev.served === 0 && r.cls === "bad", `page: ${what}: refused, nothing sent`);
}
{
  const dev = new FakeKeyboard({ identity: "SMK-37 Pro_016" });
  const r = await runPage(dev, { risk: false });
  ok(r.status === "Tick the box above first." && dev.upgrades === 0, "page: nothing before the risk box is ticked");
}
{
  const dev = new FakeKeyboard({ identity: "SMK-37 Pro_016" });
  const r = await runPage(dev, { tamper: (b) => { const c = Uint8Array.from(b); c[0x2000] ^= 1; return c; } });
  ok(/damaged/.test(r.loaded) && r.go.disabled && dev.upgrades === 0, "page: a damaged download (SHA-256) is never offered");
}
{
  const r = await runPage(new FakeKeyboard({ identity: "SMK-37 Pro_016" }), { lang: "fr-CA", risk: false });
  ok(/^Prêt : Clipper Pro 2\.8/.test(r.loaded) && r.status === "Cochez d'abord la case ci-dessus.", "page: in French for a French browser");
}
{   /* sloop-pro #3 (ChanceTheMaker): "the installer freezes my Chrome browser when installing via usb". On Windows the
       keyboard restarts twice (into its loader, then into Clipper Pro); while a document holds a MIDIAccess, Chrome's
       MIDI holds the keyboard through those restarts, whatever ports the page closes. The page must let Chrome's MIDI
       go before each restart and take it again after: then the install finishes, nothing freezes, nothing is held */
  const dev = new FakeKeyboard({ identity: "SMK-37 Pro_016", loader: "ota-SMK-37 Pro_928", finalIdentity: "SMK-37 Pro_928" });
  const chrome = new ChromeMidi(dev, { windows: true });
  let last = Date.now(), lag = 0;
  const dog = setInterval(() => { const n = Date.now(); lag = Math.max(lag, n - last - 100); last = n; }, 100);   /* the watchdog */
  let tm;
  const r = await Promise.race([runPage(dev, { chrome }), new Promise((res) => { tm = setTimeout(() => res(null), 45000); })]);
  clearTimeout(tm); clearInterval(dog);
  ok(r && r.status === "Done: the keyboard runs SMK-37 Pro_928" && eq(dev.written, logical(IMAGES["SMK-37 Pro"]).slice(0, 4096)),
     "page (#3): Windows: the install through both restarts finishes");
  ok(chrome.leaves === 2 && chrome.heldAtLeave === 0 && !chrome.wedged && chrome.froze === 0 && lag < 1000,
     `page (#3): Chrome's MIDI let go at both restarts: nothing wedged or frozen (${chrome.heldAtLeave} held, ${lag} ms lag)`);
  ok(r && chrome.docs.size === 0 && r.frames.size === 0, "page (#3): nothing holds MIDI once it is done (its iframe removed)");
}
{   /* no keyboard: the page says so (with the MIDI ports it saw), and lets MIDI go */
  const dev = new FakeKeyboard({ identity: "SMK-37 Pro_016" });
  dev.access.inputs.clear(); dev.access.outputs.clear();
  const chrome = new ChromeMidi(dev);
  const r = await runPage(dev, { chrome });
  ok(/^Keyboard not found/.test(r.status) && r.cls === "bad" && chrome.docs.size === 0, "page: no keyboard: 'not found', MIDI let go");
}
console.log(`(${((Date.now() - t0) / 1000).toFixed(0)} s)`);
console.log(failed ? `CLIPPER PRO PAGE TESTS FAILED (${failed})` : "clipper pro page tests passed");
process.exit(failed ? 1 : 0);
