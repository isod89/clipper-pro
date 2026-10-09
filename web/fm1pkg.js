// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
//
// FM-1 (and M-Vave SMK-37 Pro) .fwsc packages in the browser: the package identity and the image the
// device reads during an update.

const BLOCKS = 20, BLK = 0x30, KEEP = 0x2F, MAX_BLOCKS = 64;

// the package identity ("FM-1_904", "SMK-37 Pro_016"): one marker byte after each of the first 20 blocks
export function productOf(fwsc) {
  if (fwsc.length < BLOCKS * BLK) throw new Error("not an FM-1 package (too short)");
  let s = "";
  for (let i = 0; i < BLOCKS; i++) {
    const m = fwsc[i * BLK + KEEP];
    if (m !== 0x7D) s += String.fromCharCode((m - i - 1) & 0xFF);
  }
  return s;
}

// interleave blocks: 20, then more while the marker is the unused 0x7D (the stock SMK-37 Pro v016 has 36)
export function blockCount(fwsc) {
  let n = BLOCKS;
  while (n < MAX_BLOCKS && (n + 1) * BLK <= fwsc.length && fwsc[n * BLK + KEEP] === 0x7D) n++;
  return n;
}

function deinterleave(fwsc, blocks) {
  const out = new Uint8Array(fwsc.length - blocks);
  for (let i = 0; i < blocks; i++) out.set(fwsc.subarray(i * BLK, i * BLK + KEEP), i * KEEP);
  out.set(fwsc.subarray(blocks * BLK), blocks * KEEP);
  return out;
}

function crc16(d, c = 0) {                 // CRC-16/XMODEM, as the package and the device use
  for (const b of d) {
    c ^= b << 8;
    for (let k = 0; k < 8; k++) c = c & 0x8000 ? ((c << 1) ^ 0x1021) & 0xFFFF : (c << 1) & 0xFFFF;
  }
  return c;
}

function jlDec(b) {                        // JieLi ENC stream cipher (its own inverse)
  const out = Uint8Array.from(b);
  let key = 0xFFFF;
  for (let i = 0; i < out.length; i++) {
    out[i] ^= key & 0xFF;
    key = ((key << 1) ^ (key & 0x8000 ? 0x1021 : 0)) & 0xFFFF;
  }
  return out;
}

// the logical image's flash.bin (type 0) and ota.bin (type 100) match their CRCs (as fm1_install.py)
function filesOk(img) {
  if (img.length < 0x40) return false;
  const h = jlDec(img.subarray(0, 0x40)), rd = (a, o, n) => a.slice(o, o + n).reduce((v, x, i) => v + x * 2 ** (8 * i), 0);
  if (crc16(h.subarray(2, 0x40)) !== rd(h, 0, 2)) return false;
  const count = rd(h, 8, 2);
  if (count < 1 || count > 8 || img.length < 0x40 + 0x50 * count) return false;
  for (let i = 0; i < count; i++) {
    const e = jlDec(img.subarray(0x40 + i * 0x50, 0x90 + i * 0x50));
    const typ = rd(e, 0, 2), off = rd(e, 8, 4), size = rd(e, 12, 4);
    if (off + size > img.length) return false;
    if ((typ === 0 || typ === 100) && crc16(img.subarray(off, off + size)) !== rd(e, 4, 2)) return false;
  }
  return true;
}

// the image the device addresses during an update: the .fwsc without the marker bytes (20 blocks, or the
// detected count when the files' CRCs agree: a 20-block package can have 0x7D after block 20 by chance)
export function logicalImage(fwsc) {
  const n = blockCount(fwsc);
  if (n !== BLOCKS) {
    const img = deinterleave(fwsc, n);
    if (filesOk(img) || !filesOk(deinterleave(fwsc, BLOCKS))) return img;
  }
  return deinterleave(fwsc, BLOCKS);
}

// The official FM-1 V15 (M-VAVE Downloads -> PC Firmware -> FM-1 V15, FM-1.fwsc), for the return to the
// official firmware: only that exact file is accepted (its SHA-256 pins every byte: the loader, the app, the
// layout; the name of the file proves nothing). The digest is the one Felucca 1.0 pins.
export const STOCK_V15_SHA256 = "db1642b2b6fa5c2cccb11ffd13878068bb28601678d3644049f99dc40e7edb8a";
export const STOCK_V15_SIZE = 699956;
export const STOCK_V15_PRODUCT = "FM-1_015";
// The official SMK-37 Pro V016 (SMK-37_Pro_016.fwsc, 36 interleave blocks), for the return of an SMK-37 Pro.
export const STOCK_SMK016_SHA256 = "2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0";
export const STOCK_SMK016_SIZE = 705252;
export const STOCK_SMK016_PRODUCT = "SMK-37 Pro_016";

export async function sha256hex(bytes) {
  const d = await crypto.subtle.digest("SHA-256", bytes);
  return [...new Uint8Array(d)].map((x) => x.toString(16).padStart(2, "0")).join("");
}

export async function validateStockPackage(bytes) {
  for (const [size, sha, product] of [[STOCK_V15_SIZE, STOCK_V15_SHA256, STOCK_V15_PRODUCT],
                                      [STOCK_SMK016_SIZE, STOCK_SMK016_SHA256, STOCK_SMK016_PRODUCT]])
    if (bytes.length === size && await sha256hex(bytes) === sha && productOf(bytes) === product)
      return { product, image: logicalImage(bytes) };
  throw new Error("select the unmodified official FM-1 V15 file (FM-1.fwsc) or SMK-37 Pro V016 file (SMK-37_Pro_016.fwsc)");
}
