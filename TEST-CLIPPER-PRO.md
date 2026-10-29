# Clipper Pro 1.3 — les essais sur le SMK-37

Tout ce qu'il faut, dans l'ordre. Le guide rapide : [DEMARRAGE-RAPIDE-PRO-FR.md](DEMARRAGE-RAPIDE-PRO-FR.md) ; la
référence : [CLIPPER-PRO.md](CLIPPER-PRO.md).

## 0. Ce qu'il faut

- Le même environnement que pour SLOOP Pro et Clipper : WSL **MinUI-Build** avec la chaîne JieLi dans
  `/root/fm1-codex-deps/jieli/toolchain`, Python sur Windows avec Pillow.
- Les fichiers du SDK : le script les prend dans `build\deps\ac79` de ce dossier, sinon dans le dossier voisin
  `..\CLIPPER`, `..\sloop-pro` ou `..\SLOOP`.
- Le clavier en firmware M-VAVE **V016** (sinon : M-UPGRADE d'abord).
- Chrome ou Edge, un câble USB **de données** branché directement sur le PC.

## 1. Compiler

Double-clic sur **`INSTALL-CLIPPER-PRO.bat`**. Trois compilations (Pro, Elite, MKE-P37), quelques minutes. On doit voir :

- `C L I P P E R   P R O   1.3   (8 voices)`
- « Building Clipper Pro for the SMK-37 Pro / SMK-37 Elite / MKE-P37 »
- les paquets et leur SHA-256 dans `build\release\` (`clipper-pro.fwsc`, `clipper-pro-elite.fwsc`, `clipper-pro-mke.fwsc`)
- l'installateur qui s'ouvre : `http://localhost:8768/webapp/installer/`

**Garder la fenêtre noire ouverte** pendant l'installation. C'est la première compilation réelle de Clipper Pro :
**une erreur, surtout « exceeds the app slot » (le firmware ne tient pas dans la flash), envoyez-moi les 30 dernières
lignes.**

## 2. Installer

1. Fermer les autres logiciels MIDI (et la fenêtre de SLOOP Pro s'il en reste une).
2. Modèle : **Automatique**, cocher la case, **Installer Clipper Pro**, autoriser le MIDI.
3. « Appareil : SMK-37 Pro_0xx (ou _9xx) → paquet SMK-37 Pro_900 », puis Vérification → mode mise à jour → Écriture →
   Redémarrage → « Terminé ».
4. Bloqué sous Windows : fermer Chrome complètement (Gestionnaire des tâches), rouvrir la page (`OUVRIR-INSTALLATEUR-PRO.bat`),
   Installer : ça reprend. Ou `INSTALLER-USB-PRO.bat`.

Vos presets utilisateur et réglages de SLOOP Pro restent (Clipper Pro les partage, comme sur le FM-1).

## 3. Les essais (noter OK / PAS OK et un détail)

| # | Action | Attendu |
|---|---|---|
| 1 | Allumer | Logo clipper, badge orange **PRO** en haut à droite, « 1.3 » dessous |
| 2 | PATCH maintenu (MENU) → SYSTEM → ABOUT | « CLIPPER » + « PRO » orange, « SMK-37 PORT: GENKIGENKI », la version, 2 CORES ou 1 CORE |
| 3 | Les 37 touches, doux puis fort | Toutes jouent le son avec la nuance, y compris les 5 graves et les 5 aiguës |
| 4 | K4 sur HOME, puis K3, en tapant des pads | Les sons défilent ; K3 saute au genre suivant (BASS, KEYS…) ; KNOB BANK clignote à chaque cran ; les pads **jouent toujours** le son (ils ne changent pas de son) |
| 5 | **KNOB BANK** | SONS : 16 cases de la couleur du genre, son actuel en blanc ; un pad charge le son + une courte note ; `<` `>` changent de page ; KNOB BANK referme |
| 6 | Les 16 pads (NOTES) | Le son joue ; bas F3–F4, haut G4–G5 ; nuance ; couleur du genre, plus vif quand ça sonne |
| 7 | SEL : QNT sur WHITE, une gamme | Les pads suivent la gamme, les toniques plus vives |
| 8 | **FADER BANK**, faders 1 à 4 (banque A) ; puis PATCH, et bouger un fader | Écran FADER BANK ; BRITE, TIMBR, MOTN, SPACE changent le son ; sur un son HYBRID : M1–M4 ; un fader bougé ramène l'écran avec sa valeur |
| 9 | Banque B | ATTACK, DECAY, SUSTAIN, RELEASE à l'oreille (une note longue) |
| 10 | Banque C | CUTOFF, RESONANCE, vitesse du LFO, GLIDE ; un moteur sans résonance : « - » |
| 11 | Banque D | envois CHORUS, DELAY, REVERB, LEVEL |
| 12 | Molette PITCH | Monte / descend, revient exactement au centre ; GLO > MIDI > BEND change l'amplitude |
| 13 | Molette MOD | Vibrato (ou ce que fait le son : page MOD > MW) |
| 14 | Pédale branchée **avant** d'allumer : accord, pédale, lâcher | Les notes tiennent, s'arrêtent au pied levé |
| 15 | SEQ tenu ; SEQ tapé (NOTES) | Les 16 pas sur les pads (haut 1–8) ; la page NOTES du séquenceur |
| 15b | GLOBE tenu ; BT tenu ; FX tenu | Pads : seules les cases utiles allumées (GLOBE : 9, 10, 16 ; BT : parties enregistrées, 5–8, 13, 14, 16 ; FX : les 16, et l'écran dit « hold + pad ») |
| 15c | PATCH maintenu → PADS → PAD LIGHTS | HIGH, MID, LOW changent la luminosité des pads ; KEYS n'apparaît plus |
| 15d | Pads (NOTES) : 20 frappes douces, 20 fortes, un doigt laissé posé après une frappe douce, un roulement ; puis PADS → PAD THRESHOLD sur LOW et HIGH ; si une frappe double : PADS → PAD SCOPE, frapper jusqu'à « DOUBLE », photo de l'écran | Une frappe = une note, même douce ; le pad frappé s'allume en blanc un instant ; le roulement joue chaque coup ; LOW : un effleurement joue ; HIGH : il faut appuyer plus |
| 15e | (correctifs de SLOOP 2.8.2) Le son en POLY (EDIT, VOICE) : REC, tenir une note grave, jouer 3 notes par-dessus, relâcher ; PLAY. Puis SEQ → NOTES, KNOB 3 LEN sur la note grave pour la prolonger sous les autres. Le son en MONO : une touche par-dessus une note tenue en enregistrant | La note tenue ne coupe pas et ne redémarre pas, ni en jouant ni à la relecture ; LEN la prolonge sous la mélodie ; MONO : « MONO: SET VOICE POLY » |
| 15f | (correctifs de SLOOP 2.8.3) Projet vide, REC : tenir une note grave (son en POLY ou UNI2), jouer 3 notes par-dessus, relâcher ; PLAY | La note tenue dure toute la prise, sous les autres |
| 15g | Un son SAMPLE (GRAND PNO, LOFI KEYS…), ATK un peu monté, la même note rejouée vite, en MONO puis en POLY | Pas de clic au début de chaque note |
| 15h | Le preset FM KICK (FMX), page EDIT : tourner ALG jusqu'au bout et au-delà | Pas de plantage (FELUCCA CRASH) ni d'écran brouillé |
| 15i | 2ND CORE ON : sauvegarde complète dans l'éditeur (Backup), 2 ou 3 fois | Pas de plantage ; s'il plante une fois, le clavier repart sur un cœur (ABOUT : 1 CORE) |
| 15j | Tapoter vite SEQ, SEL, FX (moins d'un tiers de seconde) | Pas de page qui clignote ; maintenus, la page s'affiche |
| 16 | Synthé externe sur la prise TRS (adaptateur TRS → DIN) | Il joue ce que jouent les touches, les pads, l'arpège |
| 17 | Un DAW sur USB : touches, molettes, pédale | Notes, pitch bend, CC 1, CC 64 reçus ; GLO > MIDI 2 > SEND CC : les faders envoient leurs CC |
| 18 | Un son modifié, SAVE > USER, éteindre, rallumer | Le preset est là |
| 19 | L'éditeur (`OPEN-EDITOR-PRO.bat` ou le lien de l'installateur) → Connect | Connecté ; le son s'édite en direct ; Backup enregistre un fichier |
| 20 | Le test CPU de Clipper (QUICKSTART.md, section 5) : CLIPPER, 8 notes, GLO > SYSTEM | Noter CPU / PEAK, avec 2ND CORE ON et OFF |
| 21 | Réinstaller Clipper Pro par-dessus | « Terminé », presets toujours là |
| 22 | (facultatif) SLOOP Pro puis de nouveau Clipper Pro | Les presets utilisateur suivent |

## 4. Ce qu'il faut renvoyer

- Le modèle et la version d'origine.
- Les numéros PAS OK, avec ce qui s'est passé.
- Une erreur de compilation ou d'installation : le texte, et le journal gris de la page.
