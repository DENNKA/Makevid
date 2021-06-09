# Makevid
## Description
Make animated video from subtitles
## Uses
- sfml
- glsl shaders  
shaders taken from https://github.com/yadongx/glsl repository parsed from site http://glslsandbox.com
## How to
./Makevid -o 1000 -i subtitle.srt -m music.ogg -p -s save.ph -r  
- offset +1000ms
- input subtitle - subtitle.srt
- music - music.ogg
- process
- save dialogs to save.ph
- and render  

./Makevid -l save.ph -r
- load dialogs from file save.ph
- and render (output will be same)

## Structure
```
 srt  subtitle
 ──────┬──────
       │
       │
┌──────▼──────┐
│             │
│   -i        │
│   --input   │
│             │
│             │
└──────┬──────┘
       │
       │◄─────────────►┌──────────────┐
       │               │   -s         │
┌──────▼──────┐        │   --save     │
│             │        │   to file    │
│   -p        │        │              │
│  --process  │        │     OR       │
│  add modes  │        │              │
│   to text   │        │   -l         │
└──────┬──────┘        │   --load     │
       │               │   to file    │
       │◄─────────────►└──────────────┘
       │
┌──────▼──────┐
│             │
│   -r        │
│   --render  │
│             │
│             │
└──────┬──────┘
       │
       │
    ───▼───
    output
```
