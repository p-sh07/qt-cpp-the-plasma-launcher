[Not Another Doom Launcher!!!]
  Yes, sadly, there is a ton of these. And still, here's me, writing a 100500-th one, to rule them all.
  Haha, but seriously: 
[The Idea and the Purpose]
  -I haven't managed to find a launcher that *just works*. *Just works* like an Apple product, or another very high-quality piece of software/hardware. That you open up and already intuitively know how to use. That *just works* without a ton of extra unnecessary stuff & is convenient to use! (on the steam deck/mac as well as pc) 
  *And...I'm too lazy to try and figure out how to use someone else's launcher, I'd rather write my own...* (just joking)
[The real Purpose]
  -Currently doing an extended c++ course, so I wanted a small, fun & practical project to enable me to use my freshly-improved programming skills, have a try at a UI design (which I've always enjoyed), and - possibly - get a fun / maybe-sort-of-useful program at the end. (project #2 is a photo library sorting/storage app, so stay tuned for that! Should be more useful in the practical sense) 
   
  -Bonus: save & settings sync between devices! To figure out how simple networking can be used in a c++ program.
  
[The Plan]
I.GZDML class -> gzdoom mod launcher. Used to launch the selected mod-set (collection of wads/pk3's) with required parameters(config/iwad/save folder). 

    +Basic control/operation from the console for testing. 
    +Polymorphism for cross-platform compatibility & possible integration of other source ports later.

II.PlasmaLauncher -> Simple & attractive Qt-based ui to operate the gzdml class.

    +special features: Only the most basic functions and parameters are on the main window. 
    +The more fancy and complicated stuff is all hidden in the "drop-down menu of DooM"

III.DGC (DooM game config) Reader/writer -> class to read and wrirte a simple plain text file with the mod presets.

DGC Format {
    [Modset Label: e.g.Brutal Doom]
    #comment - the following are optional:

        -gzdoom_path: "C:/path-to-gzdoom-exe" #default is "$PROG_DIR/gzdoom/gzdoom.exe"
        -iwad_path: "C:/path-to-single-iwad-to use by default with this mod-set" #default is "$PROG_DIR/iwads/DOOM.WAD"
        -mods_folder: "C:/path-to-mods" #default path is "#PROG_DIR/mods" 
        -config_path: "C:/path to the gzdoom .ini or .cfg file" #default is "$PROG_DIR/gzdoom/gzdoom_portable.ini"
 
    #the following is the main point of the whole program:
 
    -mods: brutal_doom.pk3, atmosphere_rain_and_snow.wad, doom_metal_vol5.wad; 

    [Next Mod set label or newline or end of file]
} //end of Format

IV.Save sync module! (using syncthing for now, but, again, for practice! and for fun).

[The End]
